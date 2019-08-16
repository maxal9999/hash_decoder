/**
 * \file
 * \brief Executable cpp
 * \author Maksimovskiy A.S.
 */


#include <decoder.hpp>
#include <md5.hpp>

#include <iostream>
#include <string>

#include <thread>


namespace
{

// All supported characters
constexpr std::array<char, 10> INT_CHARS = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

constexpr std::array<char, 26> UPPER_CHARS = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

constexpr std::array<char, 26> LOWER_CHARS = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

// Default maximum of password length
constexpr int DEFAULT_MAX = 4;

constexpr size_t MIN_A = 65;
constexpr size_t MAX_Z = 90;
constexpr size_t MIN_a = 97;
constexpr size_t MAX_z = 122;

constexpr size_t HASH_SIZE = 32;

}

int main(int argc, char* argv[])
{
    if (argc < 5)
        throw std::invalid_argument("Not enough arguments");

    // Range of the integer symbols
    size_t from = 0, to = 0;
    try
    {
        from = static_cast<size_t>(std::stoi(std::string(argv[1])));
        to = static_cast<size_t>(std::stoi(std::string(argv[2])));
    }
    catch( ... )
    {
        throw std::invalid_argument("Failed to convert to integer");
    }

    if (from < 0 || from > 10 || to < from || to > 10)
        throw std::invalid_argument("Invalid integer range bound");

    // List of the analyzed characters
    std::vector<char> parsed_array;
    for (size_t idx = from; idx < to; idx++)
        parsed_array.emplace_back(INT_CHARS[idx]);

    // Lambda function to parse input argument (example, 65-90:97-122)
    auto split = [](std::vector<std::string>& res, std::string& input, const std::string& delim)
    {
        size_t pos = 0;
        if ((pos = input.find(delim)) != std::string::npos)
        {
            std::string token = input.substr(0, pos);
            res.emplace_back(std::move(token));
            input.erase(0, pos + delim.length());
            res.emplace_back(std::move(input));
        }
    };

    std::string char_range(argv[3]);
    std::vector<std::string> ranges_result;
    split(ranges_result, char_range, ":");
    if (ranges_result.size() != 2)
        throw std::invalid_argument("Invalid character range");

    std::vector<size_t> convert_ranges;
    for (auto& range: ranges_result)
    {
        std::vector<std::string> inner_range;
        split(inner_range, range, "-");
        if (inner_range.size() != 2)
            throw std::invalid_argument("Invalid character range");

        size_t from = 0, to = 0;
        try
        {
            from = static_cast<size_t>(std::stoi(inner_range[0]));
            to = static_cast<size_t>(std::stoi(inner_range[1]));
        }
        catch( ... )
        {
            throw std::invalid_argument("Failed to convert to integer");
        }

        convert_ranges.insert(convert_ranges.end(), { from, to });
    }

    if (convert_ranges[0] < MIN_A || convert_ranges[1] > MAX_Z ||
        convert_ranges[2] < MIN_a || convert_ranges[3] > MAX_z)
        throw std::invalid_argument("Invalid character range");

    for (size_t idx = convert_ranges[0] - MIN_A; idx <= convert_ranges[1] - MIN_A; idx++)
        parsed_array.emplace_back(UPPER_CHARS[idx]);

    for (size_t idx = convert_ranges[2] - MIN_a; idx <= convert_ranges[3] - MIN_a; idx++)
        parsed_array.emplace_back(LOWER_CHARS[idx]);

    std::string original_hash(argv[4]);
    if (original_hash.size() != HASH_SIZE)
        throw std::invalid_argument("Unsupported hash view");

    int password_max = DEFAULT_MAX;
    if (argc == 6)
    {
        try
        {
            password_max = std::stoi(argv[5]);
        }
        catch( ... )
        {
        }
    }

    utils::MD5Decoder decoder(original_hash, std::move(parsed_array), password_max);
    auto res = decoder.Execute();
    if (res.mResultType == utils::ResultType::SUCCESS)
        std::cout << "Result: " << res.mResultStr << "\n";
    else
        std::cout << "Result: failed to decode hash" << "\n";

    return 0;
}
