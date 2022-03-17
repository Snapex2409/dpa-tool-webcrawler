#pragma once
#include <string>
#include <vector>
#pragma comment(lib, "urlmon.lib")

/*Accesses the specified URL and returns the HTML resource.*/
std::string load_page(const wchar_t* url);

/*Searches for relevant links in buffer and creates URLs from them.
  buffer: vector of hrefs
  domain: domain of page to be searched
  returns results in buffer
*/
void create_urls(std::vector<std::string>& buffer, const std::string& domain, const std::string& protocol);