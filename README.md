C++ HashDecoder
=========================

## Usage
This decoder is used to crack MD5 hash.

## Description
COMPILE:
mkdir build
cd build
cmake ..
make 

In executable file hash_decoder.cpp constant variable MAX is responsible for the maximum length of the encoded word. You can change it to change the analyzed range.

EXAMPLE RUN:
./hash_decoder 3 5 65-90:97-122 b67af3651497e0d3ef0fde55308a7d0e
OUTPUT:
Result: avac

UNIT TESTS RUN:
./test_hash_decoder
