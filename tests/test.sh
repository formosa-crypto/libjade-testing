#!/bin/bash

# Scratch test file for testing the integration of the other Dilithium variants.
#
# Author: Amber Sprenkels <amber@electricdusk.com>
# Date: 14 Sep 2022

export LD_LIBRARY_PATH="$HOME/jasmin-dilithium/dilithium/ref:$LD_LIBRARY_PATH"
readonly DEFAULT_TESTS=(
    test_keygen
    test_fft
    test_packing
    test_hashing
    test_sign_fragments
    test_expandmask
    test_sign
    test_rounding
)
export TESTS=("${TESTS[@]:-${DEFAULT_TESTS[@]}}")
LOGFILE=$(mktemp --tmpdir jasmin.XXXXXXXXXX.log)

make --directory=/home/dsprenkels/jasmin-dilithium/dilithium/ref libpqcrystals_dilithium{2,3,5}_ref.so libpqcrystals_fips202_ref.so || exit 125
make --trace "${TESTS[@]}" || exit 1

for test_name in "${TESTS[@]}"; do
    (set -x; "./$test_name" || echo -e "\033[1;31mtest failed: $test_name\033[0m")
done

echo >&2 "Tests done!"
(set -x; jasminc -wea "../src/${VARIANT:-dilithium2}/lib.jazz" >/dev/null 2>"$LOGFILE")
echo "$(grep -c -F 'extra assignment introduced' "$LOGFILE") extra assignment warnings in $LOGFILE"

exit 0
