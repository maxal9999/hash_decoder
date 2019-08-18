/**
 * \file
 * \brief Implementation of the hash decoder
 * \author Maksimovskiy A.S.
 */

#include <decoder.hpp>
#include <thread_pool.hpp>

#include <algorithm>
#include <iostream>
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

    auto all_chars_size = mRange.size();

    Result result;
    using FutRes = std::future<Result>;

    auto impl_func = [](MD5Decoder& obj, char symbol)
    {
        std::string tmp_str = {symbol};
        return obj.Next(tmp_str);
    };

    for (size_t idx = 0; idx < all_chars_size; idx++)
    {
        auto created_thread = std::min(thread_count, all_chars_size - idx);

        pool::ThreadPool pool(thread_count);
        std::vector<FutRes> results;
        results.reserve(created_thread);
        for (size_t i = 0; i < created_thread; i++)
        {
            results.emplace_back(
                pool.AddFunc(impl_func, *this, mRange[idx])
            );
            idx++;
        }

        idx--;

        for (auto& res: results)
        {
            auto tmp_res = res.get();
            if (tmp_res.mResultType == ResultType::SUCCESS)
            {
                result = tmp_res;
                break;
            }
        }

        if (result.mResultType == ResultType::SUCCESS)
            break;
    }

    time (&f_end);
    std::cout << "Decoded in: " << difftime(f_end, f_begin) << " seconds" << "\n";
    return result;
}

}
