#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "iostream"
#include <unordered_map>

void ThrowFailed(HRESULT v) {
    if (v != S_OK) {
        throw(v);
    }
}
template<typename T>
inline void SafeReleaseAll(T& ptr)
{
    if (ptr != nullptr)
    {

#ifdef _DEBUG
        //long v = ptr->Release();
     



#else
     while (ptr->Release());
#endif
     while (ptr->Release());

        ptr = nullptr;
    }
}
template<typename T>
inline void SafeRelease(T& ptr)
{
    if (ptr != nullptr)
    {

#ifdef _DEBUG
        long v = ptr->Release();
        if (v != 0)
        {
            //         std::cout << "uh oh, refrence of: " + std::to_string(v) + "\n";
        }


#else
        //ptr->Release();
#endif
        ptr = nullptr;
    }
}

template<typename T, typename X, typename Hash>
inline void ReleaseUMap(std::unordered_map<X,T,Hash>& m) {
    for (auto& i : m) {
        SafeRelease(i.second);
    }
}


template<typename T>
inline void SafeReleaseAlt(T& ptr)
{
    if (ptr != nullptr)
    {

#ifdef _DEBUG
        long v = ptr->Release();
        if (v != 0)
        {
            //         std::cout << "uh oh, refrence of: " + std::to_string(v) + "\n";
        }


#else
        //ptr->Release();
#endif
        ptr = nullptr;
    }
}