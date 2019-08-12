#include <decoder.hpp>
#include <md5.hpp>

#include <iostream>
#include <string>

#include <thread>


int main(int argc, char* argv[])
{
    MD5 md5;
    //auto encode = md5.digestString("HELLO THERE I AM MD5!");
    auto encode = md5.digestString("HEL");
    std::cout << "original hash: " << encode << "\n";

    int max = 3;

    utils::MD5Decoder decoder(encode, max);
    auto res = decoder.Execute();
    std::cout << "Result: " << res.mResultStr << "\n";

    return 0;
}
