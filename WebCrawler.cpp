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
#include <algorithm>

#include "Network.h"
#include "Token.h"

static void crawl(std::map<std::string, Token*>& pages, const std::string& domain, const std::string& protocol, const std::string& path, int limit);
static char* getCmdOption(char** begin, char** end, const std::string& option);
static bool cmdOptionExists(char** begin, char** end, const std::string& option);

#define cmd_oe(A) cmdOptionExists(argv, argv + argc, (A))
#define get_cmd_o(A) getCmdOption(argv, argv + argc, (A))


int main(int argc, char **argv)
{
    if (cmd_oe("-h") || cmd_oe("--help")) {
        //print help
        return EXIT_SUCCESS;
    }

    std::string domain = "";
    std::string protocol = "https://";
    std::string path = "";
    int limit = 1;

    if (cmd_oe("-d")) domain = get_cmd_o("-d");
    if (cmd_oe("--domain")) domain = get_cmd_o("--domain");
    if (cmd_oe("-P")) protocol = get_cmd_o("-P");
    if (cmd_oe("--protocol")) protocol = get_cmd_o("--protocol");
    if (cmd_oe("-p")) path = get_cmd_o("-p");
    if (cmd_oe("--path")) path = get_cmd_o("--path");
    if (cmd_oe("-l")) limit = atoi(get_cmd_o("-l"));
    if (cmd_oe("--limit")) limit = atoi(get_cmd_o("--limit"));


    std::map<std::string, Token*> pages;
    crawl(pages, domain, protocol, path, limit);
    
    for (const auto& [k, v] : pages) print_token(v);
    return 0;
}

static std::wstring convert(const std::string& str) {
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide = converter.from_bytes(str);
    return wide;
}

static void crawl(std::map<std::string, Token*>& pages, const std::string& domain, const std::string& protocol, const std::string& path, int limit) {
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
        pages[url] = tk;

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

static char* getCmdOption(char** begin, char** end, const std::string& option)
{
    char** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

static bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}
