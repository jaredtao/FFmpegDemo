#pragma once
#include <cstdint>

template<class char_type>
class Buffer
{
public:
    Buffer(const char_type *buf, int size)
    {
        init(size);

        memcpy(pBuf, buf, size);
        mSize = size;
    }
    Buffer() { }
    ~Buffer()
    {
        delete[] pBuf;
        mSize = 0;
        mCapacity = 0;
    }
    void setData(const char_type *buf, int size)
    {
        if (mCapacity < size) {
            delete[] pBuf;
            init(size);
        }
        memcpy(pBuf, buf, size);
        mSize = size;
    }
    void reserve(int capacity)
    {
        if (capacity > mCapacity) {
            mCapacity = capacity;
            char_type *newBuf = new char_type[mCapacity];
            memset(newBuf, 0, mCapacity);

            memcpy(newBuf, pBuf, mSize);
            delete[] pBuf;
            pBuf = newBuf;
        }
    }
    void fill(char_type value)
    {
        memset(pBuf, value, mCapacity);
        mSize = mCapacity;
    }
    char_type *data() const { return pBuf; }
    const char_type *constData() const { return pBuf; }
    int size() const { return mSize; }
    int capacity() const { return mCapacity; }

private:
    void init(int size)
    {
        mCapacity = size;
        pBuf = new char_type[mCapacity];
        memset(pBuf, 0, mCapacity);
    }

private:
    char_type *pBuf = nullptr;
    int mSize = 0;
    int mCapacity = 0;
};

using CharBuffer = Buffer<char>;
using U8Buffer = Buffer<uint8_t>;
