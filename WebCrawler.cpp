// WebCrawler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include "Network.h"
#include "Token.h"

int main()
{
    std::string document = load_page(L"https://de.wikipedia.org/wiki/Haushuhn");

    Token* tkn = new Token("ROOT");
    tokenize(tkn, document);

    std::vector<std::string> buffer;
    find_links(*tkn, buffer);

    std::for_each(buffer.begin(), buffer.end(), [](const std::string& s) {std::cout << s << std::endl; });
    return 0;
}
