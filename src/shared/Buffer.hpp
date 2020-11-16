#pragma once
#include<cstdint>
template<class CharType>
class Buffer
{
public:
    Buffer(const CharType *buf, int size) noexcept
    {
        init(size);

        memcpy(pBuf, buf, size);
        mSize = size;
    }
    Buffer() noexcept { }
    ~Buffer() noexcept
    {
        delete[] pBuf;
        mSize = 0;
        mCapacity = 0;
    }
    void setData(const CharType *buf, int size) noexcept
    {
        if (mCapacity < size) {
            delete[] pBuf;
            init(size);
        }
        memcpy(pBuf, buf, size);
        mSize = size;
    }
    void reserve(int capacity) noexcept
    {
        if (capacity > mCapacity) {
            mCapacity = capacity;
            CharType *newBuf = new CharType[mCapacity];
            memset(newBuf, 0, mCapacity);

            memcpy(newBuf, pBuf, mSize);
            delete[] pBuf;
            pBuf = newBuf;
        }
    }
    void fill(CharType value) noexcept
    {
        memset(pBuf, value, mCapacity);
        mSize = mCapacity;
    }
    CharType *data() const noexcept { return pBuf; }
    const CharType *constData() const noexcept { return pBuf; }
    int size() const noexcept { return mSize; }
    int capacity() const noexcept { return mCapacity; }

private:
    void init(int size) noexcept
    {
        mCapacity = size;
        pBuf = new CharType[mCapacity];
        memset(pBuf, 0, mCapacity);
    }

private:
    CharType *pBuf = nullptr;
    int mSize = 0;
    int mCapacity = 0;
};

using CharBuffer = Buffer<char>;
using UCharBuffer = Buffer<unsigned char>;
using Uint8Buffer = Buffer<uint8_t>;
