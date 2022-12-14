#!/bin/bash

# Scratch test file for testing the integration of the other Dilithium variants.
#
# Author: Amber Sprenkels <amber@electricdusk.com>
# Date: 14 Sep 2022

export LD_LIBRARY_PATH
LD_LIBRARY_PATH="$(dirname "$0")/../dilithium/ref:$LD_LIBRARY_PATH"
LD_LIBRARY_PATH="$(dirname "$0")/../dilithium/avx2:$LD_LIBRARY_PATH"
readonly DEFAULT_TESTS=(
    # test_expandmask
    test_ref_fft
    test_avx2_fft
    test_hashing
    test_keygen
    test_matrix_mult
    test_packing
    test_rounding
    test_sign
    test_verify
    # bench_fft
    # bench_hashing
    # bench_keygen
    # bench_sign
    # bench_verify
)
readonly MAKE="make"
export TESTS=("${TESTS[@]:-${DEFAULT_TESTS[@]}}")
LOGFILE=$(mktemp --tmpdir jasmin.XXXXXXXXXX.log)
ASMFILE=$(mktemp --tmpdir jasmin.XXXXXXXXXX.s)

# export ARCH=avx2

$MAKE -j --directory="$(dirname "$0")/../dilithium/ref" libpqcrystals_dilithium{2,3,5}_ref.so libpqcrystals_fips202_ref.so || exit 125
$MAKE -j --directory="$(dirname "$0")/../dilithium/avx2" libpqcrystals_dilithium{2,3,5}_avx2.so libpqcrystals_fips202_avx2.so libpqcrystals_fips202x4_avx2.so || exit 125
$MAKE "${TESTS[@]}" || exit 1

for test_name in "${TESTS[@]}"; do
    (set -x; "./$test_name" || echo -e "\033[1;31mtest failed: $test_name\033[0m")
done

echo >&2 "Tests done!"
(
    set -x
    if ! jasminc -I Jade:../libjade/src -intel -wea -pasm -o "$ASMFILE" "../libjade/src/crypto_sign/dilithium/${VARIANT:-dilithium2}/amd64/${ARCH:-ref}/lib.jazz" 2>"$LOGFILE"; then
        cat "$LOGFILE" >&2
    fi
)
echo "$(grep -c -F 'extra assignment introduced' "$LOGFILE") extra assignment warnings in $LOGFILE"
echo "$(wc -l <"$ASMFILE" || true) lines in $ASMFILE"

exit 0
