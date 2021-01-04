#pragma once
#include <cmath>
#include <vector>
#include <type_traits>

//平均数
template<typename T, typename Iter>
T average(Iter first, Iter last)
{
    if (first == last) {
        return T(0);
    }
    int n = last - first;
    T sum = 0;
    while (first != last) {
        sum += *first;
        first++;
    }
    return sum / n;
}
//方差
template <typename T, typename Iter >
T variance(Iter first, Iter last)
{
    if (first == last) {
        return T(0);
    }
    int n = last - first;
    if (n == 1) { 
        return T(0);
    }
    T avg = average<T>(first, last);
    T sum = 0;
    while (first != last) { 
        sum += std::pow(*first - avg, 2);
        first++;
    }
    sum /= n - 1;
    return sum;
}
//标准差
template<typename T, typename Iter>
T standardDeviation(Iter first, Iter last)
{
    return std::sqrt(variance<T>(first, last));
}