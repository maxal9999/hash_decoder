/*
 *
 *
*/

#include <md5.hpp>

#include <string>
#include <vector>


namespace utils
{


enum class ResultType: int16_t
{
    FAILED,
    SUCCESS
};

struct Result
{
    std::string mResultStr;
    ResultType mResultType;
};


class MD5Decoder
{
public:
    MD5Decoder(const std::string& original, std::vector<char>&& range, int max_length);

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
