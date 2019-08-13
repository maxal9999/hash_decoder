/**
 * \file
 * \brief Hash decoder API
 * \author Maksimovskiy A.S.
 */

#include <md5.hpp>

#include <string>
#include <vector>


namespace utils
{

// Two types of the result
enum class ResultType: int16_t
{
    FAILED,
    SUCCESS
};

// Result struct for hash decoder
struct Result
{
    std::string mResultStr;
    ResultType mResultType;
};


class MD5Decoder
{
public:
    // Constructor
    MD5Decoder(const std::vector<char>& range, int max_length);
    MD5Decoder(const std::string& original, std::vector<char>&& range, int max_length);

    // Init hash to decode
    void InitOriginalHash(const std::string& original);

    // Method to decode hash
    Result Execute();

private:
    std::string mOriginal;
    bool mIsUpperOriginal;
    std::vector<char> mRange;
    size_t mMax;
    MD5 mMD5;

    bool IsEqualOriginal(const std::string& cacl_str);
    std::string MD5String(const std::string& calc_str);
    Result Next(const std::string& cacl_str);
};

}
