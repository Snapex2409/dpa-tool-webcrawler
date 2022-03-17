#pragma once
#include <string>
#include <map>
#include <vector>
#include <list>
#include <utility>
#include <algorithm>

/*Models a HTML Token.*/
class Token
{
public:
	std::string type;
	std::map<std::string, std::string> options;
	std::string text;
	std::list<Token*> children;

	Token(const Token& other) = delete;
	Token(const std::string& t) : type(t) {}
	Token(const std::string& t, const std::string& txt) : type(t), text(txt) {}
};

/*Tokenizes string of html tokens. Provide empty wrapper token. Works recursively on parts.
Returns text of higher level token, with all internal tokens removed.**/
std::string tokenize(Token *root, const std::string& text);

/*Finds all href attributes in the token tree. Stores the references into buffer.*/
void find_links(Token& tk, std::vector<std::string>& buffer);

/*Frees the entire Token tree.*/
void release(Token* tk);

/*Prints text of token tree.*/
void print_token(Token* tk);