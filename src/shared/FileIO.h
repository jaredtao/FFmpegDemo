#include "Buffer.h"
//#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>

bool readFile(CharBuffer &buffer, const std::wstring &fileName)
{
    using namespace std;

    ifstream in;
    in.open(fileName, ios::in | ios::binary | ios::ate);
    if (!in) {
        wcout << "open file failed " << fileName << endl;
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
template<typename char_type>
bool writeFile(const Buffer<char_type> &buffer, const std::wstring &fileName)
{
    using namespace std;
    ofstream out;
    out.open(fileName, ios::out | ios::binary);
    if (!out) {
        wcout << "open file failed " << fileName << endl;
        return false;
    }
    out.write((char *)buffer.data(), buffer.size());
    out.close();
    return true;
}