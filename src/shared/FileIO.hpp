#include "Buffer.hpp"
#include <fstream>
#include <iostream>
#include <ios>
#include <string>
#include <filesystem>
bool readFile(CharBuffer &buffer, const std::string &fileName)
{
    using namespace std;

    ifstream in;
    filesystem::path path(fileName);
    in.open(path.string(), ios::in | ios::binary | ios::ate);
    if (!in) {
        cout << "open file failed " << fileName << endl;
        return false;
    }
    uint32_t size = (int)in.tellg();
    buffer.reserve(size);
    buffer.fill(0);
    in.seekg(0, ios::beg);
    in.read(buffer.data(), size);
    in.close();
    return true;
}

template<typename CharType>
bool writeFile(const Buffer<CharType> &buffer, const std::string &fileName)
{
    using namespace std;
    ofstream out;
    filesystem::path path(fileName);
    out.open(path.string(), ios::out | ios::binary);
    if (!out) {
        cout << "open file failed " << fileName << endl;
        return false;
    }
    out.write((const char *)buffer.data(), buffer.size());
    out.close();
    return true;
}
