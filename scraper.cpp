#include "scraper.h"
#include <iostream>
#include <fstream>

// --- ScrapedData ---

ScrapedData::ScrapedData(const std::string& title, const std::string& price,
                         const std::string& rating, const std::string& availability,
                         int page)
    : title(title), price(price), rating(rating), availability(availability), page(page) {}

std::string ScrapedData::toCSV() const {
    return title + "," + price + "," + rating + "," + availability + "," + std::to_string(page);
}

void ScrapedData::display() const {
    std::cout << "Title:        " << title << std::endl;
    std::cout << "Price:        " << price << std::endl;
    std::cout << "Rating:       " << rating << std::endl;
    std::cout << "Availability: " << availability << std::endl;
    std::cout << "---" << std::endl;
}

// --- HTMLParser ---

std::string HTMLParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string HTMLParser::stripTags(const std::string& str) {
    std::string result;
    bool inTag = false;
    for (char c : str) {
        if (c == '<') {
            inTag = true;
        } else if (c == '>') {
            inTag = false;
        } else if (!inTag) {
            result += c;
        }
    }
    return result;
}

std::string HTMLParser::extractBetween(const std::string& text,
                                       const std::string& startTag,
                                       const std::string& endTag,
                                       size_t& pos) {
    size_t start = text.find(startTag, pos);
    if (start == std::string::npos) {
        return "";
    }
    start += startTag.length();
    size_t end = text.find(endTag, start);
    if (end == std::string::npos) {
        return "";
    }
    pos = end + endTag.length();
    return text.substr(start, end - start);
}

std::string HTMLParser::extractAttribute(const std::string& tag,
                                         const std::string& attr) {
    std::string search = attr + "=\"";
    size_t start = tag.find(search);
    if (start == std::string::npos) {
        return "";
    }
    start += search.length();
    size_t end = tag.find("\"", start);
    if (end == std::string::npos) {
        return "";
    }
    return tag.substr(start, end - start);
}

std::vector<ScrapedData> HTMLParser::parse(const std::string& html, int page) {
    std::vector<ScrapedData> results;
    std::string articleStart = "<article class=\"product_pod\">";
    std::string articleEnd = "</article>";
    size_t pos = 0;

    while (true) {
        // Find the next article block
        size_t blockStart = html.find(articleStart, pos);
        if (blockStart == std::string::npos) {
            break;
        }
        size_t blockEnd = html.find(articleEnd, blockStart);
        if (blockEnd == std::string::npos) {
            break;
        }
        blockEnd += articleEnd.length();
        std::string block = html.substr(blockStart, blockEnd - blockStart);

        // Extract rating from <p class="star-rating Three">
        std::string rating = "";
        size_t ratingPos = block.find("star-rating ");
        if (ratingPos != std::string::npos) {
            ratingPos += 12; // length of "star-rating "
            size_t ratingEnd = block.find("\"", ratingPos);
            if (ratingEnd != std::string::npos) {
                rating = block.substr(ratingPos, ratingEnd - ratingPos);
            }
        }

        // Extract title from <h3><a ... title="...">
        std::string title = "";
        size_t h3Pos = block.find("<h3>");
        if (h3Pos != std::string::npos) {
            size_t aEnd = block.find(">", h3Pos + 4);
            if (aEnd != std::string::npos) {
                std::string aTag = block.substr(h3Pos, aEnd - h3Pos);
                title = extractAttribute(aTag, "title");
            }
        }

        // Extract price from <p class="price_color">
        std::string price = "";
        size_t pricePos = 0;
        std::string priceText = extractBetween(block, "<p class=\"price_color\">",
                                               "</p>", pricePos);
        if (!priceText.empty()) {
            price = trim(priceText);
        }

        // Extract availability
        std::string availability = "";
        size_t availPos = 0;
        std::string availText = extractBetween(block, "<p class=\"instock availability\">",
                                               "</p>", availPos);
        if (!availText.empty()) {
            availability = trim(stripTags(availText));
        }

        results.push_back(ScrapedData(title, price, rating, availability, page));
        pos = blockEnd;
    }

    return results;
}

// --- WebScraper ---

WebScraper::WebScraper() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curlHandle = curl_easy_init();
}

WebScraper::~WebScraper() {
    if (curlHandle) {
        curl_easy_cleanup(curlHandle);
    }
    curl_global_cleanup();
}

size_t WebScraper::writeCallback(char* ptr, size_t size,
                                  size_t nmemb, std::string* data) {
    size_t totalSize = size * nmemb;
    data->append(ptr, totalSize);
    return totalSize;
}

std::string WebScraper::fetch(const std::string& url) {
    if (!curlHandle) {
        std::cerr << "Error: Failed to initialize curl." << std::endl;
        return "";
    }

    std::string response;
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "CSE310-WebScraper/1.0");

    CURLcode res = curl_easy_perform(curlHandle);
    if (res != CURLE_OK) {
        std::cerr << "Error: curl request failed: "
                  << curl_easy_strerror(res) << std::endl;
        return "";
    }

    return response;
}

std::vector<ScrapedData> WebScraper::scrape(const std::string& url, int page) {
    std::string html = fetch(url);
    if (html.empty()) {
        std::cerr << "Error: No HTML content retrieved." << std::endl;
        return {};
    }
    return parser.parse(html, page);
}

void WebScraper::saveToFile(const std::vector<ScrapedData>& data,
                            const std::string& filename) {
    // Check if file already exists with content
    bool needsHeader = true;
    {
        std::ifstream check(filename);
        if (check.good() && check.peek() != std::ifstream::traits_type::eof()) {
            needsHeader = false;
        }
    }

    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    if (needsHeader) {
        file << "Title,Price,Rating,Availability,Page" << std::endl;
    }
    for (const auto& item : data) {
        file << item.toCSV() << std::endl;
    }

    file.close();
    std::cout << "Results appended to " << filename << std::endl;
}
