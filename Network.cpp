#include "Network.h"

#include <urlmon.h>
#include <sstream>
#include <atlstr.h>

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
    return output;
}
