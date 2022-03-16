#pragma once
#include <string>
#include <map>
#include <vector>
#include <list>
#include <utility>
#include <algorithm>

class Token
{
public:
	std::string type;
	std::map<std::string, std::string> options;
	std::string text;
	std::list<Token*> children;

	Token(const std::string& t) : type(t) {}
	Token(const std::string& t, const std::string& txt) : type(t), text(txt) {}
};

std::string tokenize(Token *root, const std::string& text);

void find_links(Token& tk, std::vector<std::string>& buffer);

void release(Token* tk);