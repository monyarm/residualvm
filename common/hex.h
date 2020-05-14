#ifndef HEX_H
#define HEX_H

#include "common/str.h"
#include <vector>

const char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                       '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};


#define toBytes(x) static_cast<byte*>(static_cast<void*>(&x))
#define toChars(x) static_cast<char*>(static_cast<void*>(&x))



inline const char* VectorToHex(std::vector<byte> _data) 
{
    int len = _data.size();
    Common::String s = "0x";
    for (int i = 0; i < len; ++i)
    {
        s += hexmap[(_data[i] & 0xF0) >> 4];
        s += hexmap[_data[i] & 0x0F];
    }
    return s.c_str();
}


template<typename T>
inline const char* BytesToHex(T _data) 
{
    byte* data = toBytes(_data);
    return BytesToHex(data);
}

template<>
inline const char* BytesToHex(byte* _data) 
{
    byte* data = _data;
    int len = sizeof(_data);
    Common::String s = "0x";
    for (int i = 0; i < len; ++i)
    {
        s += hexmap[(data[i] & 0xF0) >> 4];
        s += hexmap[data[i] & 0x0F];
    }
    return s.c_str();
}


#endif;