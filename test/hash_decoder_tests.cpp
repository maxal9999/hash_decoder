#include "decoder.hpp"
#include "md5.hpp"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestHashDecoder

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TestHashDecoder) // Начало набора тестов

namespace
{
    static const std::vector<char> ALL_CHARS = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
    };
}

struct Fixture
{
    Fixture()
        : mMD5Decoder(ALL_CHARS, 4)
    {
    }

    ~Fixture() = default;

    utils::MD5Decoder mMD5Decoder;
};

BOOST_AUTO_TEST_CASE(TestHashDecoderFirst)
{
    MD5 md5;
    auto origin_world = std::string("HI");
    auto encode = std::string(md5.digestString(const_cast<char*>(origin_world.c_str())));
    utils::MD5Decoder decoder(encode, {'1', '2', '3', 'A', 'B', 'H', 'I', 'a', 'b', 'h', 'i'}, 4);
    BOOST_CHECK_EQUAL(decoder.Execute().mResultStr, origin_world);
}

BOOST_FIXTURE_TEST_CASE(TestHashDecoderSecond, Fixture)
{
    auto origin_world = std::string("avac");
    auto encode = std::string("b67af3651497e0d3ef0fde55308a7d0e");
    mMD5Decoder.InitOriginalHash(encode);
    BOOST_CHECK_EQUAL(mMD5Decoder.Execute().mResultStr, origin_world);
}

BOOST_FIXTURE_TEST_CASE(TestHashDecoderThird, Fixture)
{
    MD5 md5;
    auto origin_world = std::string("HELO");
    auto encode = std::string(md5.digestString(const_cast<char*>(origin_world.c_str())));
    mMD5Decoder.InitOriginalHash(encode);
    BOOST_CHECK_EQUAL(mMD5Decoder.Execute().mResultStr, origin_world);
}

BOOST_AUTO_TEST_SUITE_END() // Конец набора тестов
