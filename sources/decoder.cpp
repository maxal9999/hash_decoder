/**
 * \file
 * \brief Implementation of the hash decoder
 * \author Maksimovskiy A.S.
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

bool IsUpper(const std::string& tmp_str)
{
    for (auto symbol: tmp_str)
        if (std::islower(symbol))
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

// Function for correct getting result from futures
Result CaclAsyncHash(std::vector<FutRes>& futures)
{
    Result result;

    // TODO. Now process wait all futures ending.
    // This is not entirely true. It is necessary to wait for one successful thread.
    /*static auto is_future_valid = [](FutRes& fut)
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
        if (tmp_res.mResultType == ResultType::SUCCESS)
        {
            result = std::move(tmp_res);
            return true;
        }

        return false;
    };*/

    std::for_each(futures.begin(), futures.end(), [&](FutRes& fut){
        auto tmp_res = fut.get();
        if(tmp_res.mResultType == ResultType::SUCCESS)
            result = std::move(tmp_res);
    });

    /*while (std::any_of(futures.begin(), futures.end(), is_future_valid))
    {
       if (is_success())
           break;

       bool all_ready = std::all_of(futures.begin(), futures.end(), is_future_ready);
       if (all_ready)
           break;
    }*/

    return result;
}

}

MD5Decoder::MD5Decoder(const std::vector<char>& range, int max_length)
    : mRange(std::move(range)),
      mMax(max_length)
{
}

MD5Decoder::MD5Decoder(const std::string& original, std::vector<char> &&range, int max_length)
    : mOriginal(original),
      mRange(std::move(range)),
      mMax(max_length)
{
    mIsUpperOriginal = IsUpper(mOriginal);
}

void MD5Decoder::InitOriginalHash(const std::string& original)
{
    mOriginal = original;
    mIsUpperOriginal = IsUpper(mOriginal);
}

bool MD5Decoder::IsEqualOriginal(const std::string& cacl_str)
{
    auto md5_str = MD5String(cacl_str);
    if (mIsUpperOriginal)
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

    for (auto symbol: mRange)
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
    std::cout << "Thread count: " << thread_count << "\n";

    std::vector<FutRes> futures;
    futures.resize(static_cast<size_t>(thread_count));

    auto all_chars_size = mRange.size();

    Result result;

    auto impl_func = [&](size_t char_idx)
    {
        auto symbol = mRange[char_idx];
        std::string tmp_str = {symbol};
        return Next(tmp_str);
    };

    for (size_t idx = 0; idx < all_chars_size; idx++)
    {
        auto created_thread = std::min(thread_count, all_chars_size - idx);
        for (size_t i = 0; i < created_thread; i++)
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
    std::cout << "Decoded in: " << difftime(f_end, f_begin) << " seconds" << "\n";
    return result;
}

}
