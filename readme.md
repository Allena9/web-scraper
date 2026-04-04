# C++ Web Scraper

A command-line web scraper built in C++ that fetches a webpage, parses the HTML to extract structured book data (titles, prices, ratings, availability), and saves the results to a CSV file. Built to demonstrate core C++ concepts including conditionals, loops, functions, classes, and STL containers.

## Instructions for Build and Use

[Software Demo](Put_Your_Video_Link_Here)

Steps to build and/or run the software:

1. Open a terminal and navigate to the project directory
2. Run `make` to compile the project
3. Run `./scraper` to execute the web scraper

Instructions for using the software:

1. Run the program and it will automatically fetch book data from books.toscrape.com
2. View the scraped results printed to the terminal
3. Check the generated `results.csv` file for the saved data

## Development Environment

To recreate the development environment, you need the following software and/or libraries with the specified versions:

* macOS with Xcode Command Line Tools (provides clang++ and libcurl)
* C++17 compatible compiler (clang++ or g++)
* libcurl (included with macOS by default)

## Useful Websites to Learn More

I found these websites useful in developing this software:

* [libcurl - C/C++ HTTP Library](https://curl.se/libcurl/)
* [books.toscrape.com - Web Scraping Sandbox](http://books.toscrape.com)
* [C++ std::string Reference](https://cplusplus.com/reference/string/string/)

## Future Work

The following items I plan to fix, improve, and/or add to this project in the future:

* [ ] Add support for scraping multiple pages (pagination)
* [ ] Add command-line arguments to specify a custom URL
* [ ] Handle HTML edge cases and special characters in book titles
