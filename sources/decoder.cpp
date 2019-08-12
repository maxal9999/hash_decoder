/*
 *
 *
*/

#include <decoder.hpp>

#include <algorithm>
#include <array>
#include <iostream>
#include <thread>


namespace utils
{

namespace
{

constexpr std::array<char, 60> ALL_CHARS = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'x', 'y', 'z'
};

bool IsUpper(const std::string& tmp_str)
{
    for(auto symbol: tmp_str)
        if(std::islower(symbol))
            return false;

    return true;
}

void ToUpper(std::string& tmp_str)
{
    std::transform(tmp_str.begin(), tmp_str.end(), tmp_str.begin(), ::toupper);
}

}

MD5Decoder::MD5Decoder(char* original, int max_length)
    : mOriginal(original),
      mMax(max_length)
{
    mIsUpperOriginal = IsUpper(mOriginal);
}

bool MD5Decoder::IsEqualOriginal(const std::string& cacl_str)
{
    auto md5_str = MD5String(cacl_str);
    if(mIsUpperOriginal)
        ToUpper(md5_str);

    return md5_str == mOriginal;
}

std::string MD5Decoder::MD5String(const std::string& calc_str)
{
    auto chars = mMD5.digestString(const_cast<char*>(calc_str.c_str()));
    return std::string(chars);
}

Result MD5Decoder::Next(const std::string& cacl_str)
{
    if (IsEqualOriginal(cacl_str))
        return Result{cacl_str, ResultType::SUCCESS};

    if (cacl_str.size() == mMax)
        return Result{cacl_str, ResultType::FAILED};

    for (auto symbol: ALL_CHARS)
    {
        std::string tmp_str = cacl_str;
        tmp_str += symbol;
        auto tmp_res = Next(tmp_str);
        if (tmp_res.mResultType == ResultType::SUCCESS)
            return tmp_res;
    }

    return Result();
}

Result MD5Decoder::Execute()
{
    time_t f_begin, f_end;
    time(&f_begin);

    Result result;

    for (auto symbol: ALL_CHARS)
    {
        std::string tmp_str = { symbol };
        auto tmp_res = Next(tmp_str);
        if (tmp_res.mResultType == ResultType::SUCCESS)
        {
            result = tmp_res;
            break;
        }
    }

    time (&f_end);
    std::cout << "\nCracked in: " << difftime(f_end, f_begin) << " seconds" << "\n";
    return result;
}

}
