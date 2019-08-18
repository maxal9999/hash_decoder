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

EXAMPLE RUN:
./hash_decoder 3 5 65-90:97-122 b67af3651497e0d3ef0fde55308a7d0e
OUTPUT:
Result: avac

Also added a fifth parameter - the maximum length of the selection of an encrypted word. By default, the parameter is set to 4.

EXAMPLE RUN:
./hash_decoder 3 5 65-90:97-122 b67af3651497e0d3ef0fde55308a7d0e 5
OUTPUT:
Result: avac

UNIT TESTS RUN:
./test_hash_decoder
