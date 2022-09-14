#!/bin/bash

# Scratch test file for testing the integration of the other Dilithium variants.
#
# Author: Amber Sprenkels <amber@electricdusk.com>
# Date: 14 Sep 2022

export LD_LIBRARY_PATH="$HOME/jasmin-dilithium/dilithium/ref:$LD_LIBRARY_PATH"
export TESTS="test_keygen test_fft test_packing test_hashing test_sign_fragments test_expandmask test_sign"

make --jobs=8 --directory=/home/dsprenkels/jasmin-dilithium/dilithium/ref libpqcrystals_dilithium{2,3,5}_ref.so libpqcrystals_fips202_ref.so || exit 125
make --jobs=8 --directory=/home/dsprenkels/jasmin-dilithium/src/dilithium3 || exit 1
make --jobs=8 --trace || exit 1
for test_name in $TESTS; do
    (set -x; "./$test_name" || exit 1)
done

echo >&2 "Done!"
exit 0
