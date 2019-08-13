/*
 *
 *
*/

#include <decoder.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <vector>


namespace utils
{

namespace
{

// 65-90:97-122

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

namespace
{

using FutRes = std::future<Result>;

Result CaclAsyncHash(std::vector<FutRes>& futures)
{
    Result result;

    static auto is_future_valid = [](FutRes& fut)
    {
        return fut.valid();
    };

    static auto is_future_ready = [&](FutRes& fut)
    {
        return is_future_valid(fut) &&
               fut.wait_for(std::chrono::microseconds(100)) == std::future_status::ready;
    };

    static auto is_success = [&]() mutable
    {
        auto ready_it = std::find_if(futures.begin(), futures.end(), is_future_ready);
        if (ready_it == futures.end())
            return false;

        auto tmp_res = ready_it->get();
        if(tmp_res.mResultType == ResultType::SUCCESS)
        {
            result = std::move(tmp_res);
            return true;
        }

        return false;
    };

    std::for_each(futures.begin(), futures.end(), [&](FutRes& fut){
        auto tmp_res = fut.get();
        if(tmp_res.mResultType == ResultType::SUCCESS)
            result = std::move(tmp_res);
    });

    /*while (std::any_of(futures.begin(), futures.end(), is_future_valid))
    {
       if(is_success())
           break;

       bool all_ready = std::all_of(futures.begin(), futures.end(), is_future_ready);
       if(all_ready)
           break;
    }*/

    return result;
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

    auto thread_count = static_cast<size_t>(std::thread::hardware_concurrency());
    std::cout << "thread count: " << thread_count << "\n";

    std::vector<FutRes> futures;
    futures.resize(static_cast<size_t>(thread_count));

    constexpr auto all_chars_size = ALL_CHARS.size();

    Result result;

    auto impl_func = [&](size_t char_idx)
    {
        auto symbol = ALL_CHARS[char_idx];
        std::string tmp_str = {symbol};
        return Next(tmp_str);
    };

    for (size_t idx = 0; idx < all_chars_size; idx++)
    {
        auto created_thread = std::min(thread_count, all_chars_size - idx);
        for(size_t i = 0; i < created_thread; i++)
        {
            futures[i] = std::async(impl_func, idx);
            idx++;
        }

        idx--;

        auto tmp_res = CaclAsyncHash(futures);
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
