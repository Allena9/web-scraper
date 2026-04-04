#ifndef SCRAPER_H
#define SCRAPER_H

#include <string>
#include <vector>
#include <curl/curl.h>

// Holds data for one scraped book
class ScrapedData {
public:
    std::string title;
    std::string price;
    std::string rating;
    std::string availability;
    int page;

    ScrapedData(const std::string& title, const std::string& price,
                const std::string& rating, const std::string& availability,
                int page);

    std::string toCSV() const;
    void display() const;
};

// Parses raw HTML to extract book data
class HTMLParser {
public:
    std::vector<ScrapedData> parse(const std::string& html, int page);

private:
    std::string extractBetween(const std::string& text,
                               const std::string& startTag,
                               const std::string& endTag,
                               size_t& pos);
    std::string extractAttribute(const std::string& tag,
                                 const std::string& attr);
    std::string trim(const std::string& str);
    std::string stripTags(const std::string& str);
};

// Fetches web pages and orchestrates scraping
class WebScraper {
public:
    WebScraper();
    ~WebScraper();

    std::string fetch(const std::string& url);
    std::vector<ScrapedData> scrape(const std::string& url, int page);
    void saveToFile(const std::vector<ScrapedData>& data,
                    const std::string& filename);

private:
    CURL* curlHandle;
    HTMLParser parser;

    static size_t writeCallback(char* ptr, size_t size,
                                size_t nmemb, std::string* data);
};

#endif
