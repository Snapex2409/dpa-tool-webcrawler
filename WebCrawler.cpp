// WebCrawler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include <iostream>
#include <locale>
#include <codecvt>
#include <string>
#include <list>
#include <chrono>
#include <thread>

#include "Network.h"
#include "Token.h"

void crawl(std::map<std::string, Token*>& pages, const std::string& domain, const std::string& protocol, const std::string& path, int limit);

int main()
{
    std::map<std::string, Token*> pages;
    std::string domain = "de.wikipedia.org";
    std::string protocol = "https://";
    std::string path = "/wiki/Haushuhn";
    int limit = 2;
    crawl(pages, domain, protocol, path, limit);
    
    for (const auto& [k, v] : pages) print_token(v);
    return 0;
}

static std::wstring convert(const std::string& str) {
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide = converter.from_bytes(str);
    return wide;
}

void crawl(std::map<std::string, Token*>& pages, const std::string& domain, const std::string& protocol, const std::string& path, int limit) {
    std::list<std::string> urls;
    urls.push_back(protocol + domain + path);

    while (urls.size() > 0) {
        if (limit <= 0) return;
        limit--;

        //get url
        std::string url = urls.front();
        urls.pop_front();

        std::cout << "Accessing: " << url << std::endl;

        //create tokens
        std::string doc = load_page(convert(url).c_str());
        Token* tk = new Token("ROOT");
        tokenize(tk, doc);
        pages[doc] = tk;

        //handle links
        std::vector<std::string> buffer;
        find_links(*tk, buffer);
        create_urls(buffer, domain, protocol);

        for (const std::string& str : buffer) {
            if (pages.contains(str)) continue;
            urls.push_back(str);
        }

        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
}
