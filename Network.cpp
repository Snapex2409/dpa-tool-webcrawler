#include "Network.h"

#include <urlmon.h>
#include <sstream>
#include <atlstr.h>
#include <regex>

static std::size_t replace_all(std::string& inout, std::string_view what, std::string_view with)
{
    std::size_t count{};
    for (std::string::size_type pos{};
        inout.npos != (pos = inout.find(what.data(), pos, what.length()));
        pos += with.length(), ++count) {
        inout.replace(pos, what.length(), with.data(), with.length());
    }
    return count;
}

static std::size_t remove_all(std::string& inout, std::string_view what) {
    return replace_all(inout, what, "");
}

/*Accesses the specified URL and returns the HTML resource.*/
std::string load_page(const wchar_t* url)
{
    IStream* stream;
    HRESULT result;
    char* buffer = nullptr;
    unsigned long bytes_read;
    std::stringstream ss;

    result = URLOpenBlockingStream(0, url, &stream, 0, 0);
    if (result != 0) return std::string("error");
    buffer = new char[1024];
    stream->Read(buffer, 1024, &bytes_read);
    while (bytes_read > 0U) {
        ss.write(buffer, (long long)bytes_read);
        stream->Read(buffer, 1024, &bytes_read);
    }

    stream->Release();
    delete[] buffer;

    std::string output = ss.str();
    remove_all(output, "\n");
    return output;
}

void create_urls(std::vector<std::string>& buffer, const std::string& domain, const std::string& protocol)
{
    const std::regex valid_pat("^((https)|(http))?:\\/\\/(www\\.)?[-a-zA-Z0-9@:%._\\+~#=]{1,256}\\.[a-zA-Z0-9()]{1,6}\\b([-a-zA-Z0-9()@:%_\\+.~#?&//=]*)");
    const std::regex dom_pat("^((https)|(http))?:\\/\\/" + domain + ".*");
    std::vector<std::string> i_buf;

    //check all hrefs
    for (const std::string& href : buffer) {
        std::string ref = href;
        remove_all(ref, "\n");
        remove_all(ref, "\"");

        //is section on page
        if (ref.find('#', 0) == 0) continue;
        //is URL: with https|http optionally and same domain
        if (std::regex_match(ref, valid_pat) && std::regex_match(ref, dom_pat)) i_buf.push_back(ref);
        //is ref to page on same domain
        if (ref.find('/', 0) == 0) i_buf.push_back(protocol + domain + ref);
    }

    buffer.clear();
    buffer.resize(i_buf.size());
    for (const std::string& ref : i_buf) buffer.push_back(ref);
}
