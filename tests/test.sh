#!/bin/bash

# Scratch test file for testing the integration of the other Dilithium variants.
#
# Author: Amber Sprenkels <amber@electricdusk.com>
# Date: 14 Sep 2022

export LD_LIBRARY_PATH="$HOME/jasmin-dilithium/dilithium/ref:$LD_LIBRARY_PATH"
readonly DEFAULT_TESTS="test_keygen test_fft test_packing test_hashing test_sign_fragments test_expandmask test_sign test_rounding"
export TESTS=${TESTS:-$DEFAULT_TESTS}

make --directory=/home/dsprenkels/jasmin-dilithium/dilithium/ref libpqcrystals_dilithium{2,3,5}_ref.so libpqcrystals_fips202_ref.so || exit 125
make --trace || exit 1

for test_name in $TESTS; do
    "./$test_name" || echo -e "\033[1;31mtest failed: $test_name\033[0m"
done

echo >&2 "Done!"
exit 0
