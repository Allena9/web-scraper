#include "scraper.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    std::cout << "=== C++ Web Scraper ===" << std::endl;

    srand(static_cast<unsigned int>(time(nullptr)));
    int page = (rand() % 50) + 1;

    std::string url = "http://books.toscrape.com/catalogue/page-"
                    + std::to_string(page) + ".html";

    std::cout << "Scraping page " << page << " of 50..." << std::endl;
    std::cout << std::endl;

    WebScraper scraper;

    std::vector<ScrapedData> results = scraper.scrape(url, page);

    if (results.empty()) {
        std::cerr << "No data was scraped. Please check your connection." << std::endl;
        return 1;
    }

    std::cout << "Found " << results.size() << " books:" << std::endl;
    std::cout << std::endl;

    for (const auto& book : results) {
        book.display();
    }

    std::string filename = "results.csv";
    scraper.saveToFile(results, filename);

    return 0;
}
