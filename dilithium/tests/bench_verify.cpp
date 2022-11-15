#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>

extern "C"
{
#ifdef DILITHIUM_ARCH_REF
#include "../dilithium/ref/sign.h"
#include "../dilithium/ref/params.h"
#elif DILITHIUM_ARCH_AVX2
#include "../dilithium/avx2/sign.h"
#include "../dilithium/avx2/params.h"
#else
#error "None of DILITHIUM_ARCH_REF or DILITHIUM_ARCH_AVX2 is set"
#endif
}

using std::cout;
using std::memcmp;
using std::vector;

constexpr const int repetitions = 10000;

extern "C"
{
#include "macros.h"
#include <immintrin.h>
    int VERIFY_JAZZ(
        const uint8_t signature[CRYPTO_BYTES],
        const uint8_t *msg,
        uint64_t m_len,
        const uint8_t pk[CRYPTO_PUBLICKEYBYTES]);
}

uint8_t sampleByte()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distrib(0, 255);
    return distrib(gen);
}

static double bench(int verify(const uint8_t *sig, size_t siglen, const uint8_t *m, size_t mlen, const uint8_t *pk))
{
    double total_time = 0.0;
    for (int i = 0; i < repetitions; i++)
    {
        uint8_t sig[CRYPTO_BYTES] = {};
        size_t siglen;
        uint8_t pk[CRYPTO_PUBLICKEYBYTES] = {};
        uint8_t sk[CRYPTO_SECRETKEYBYTES] = {};
        crypto_sign_keypair(pk, sk);
        crypto_sign_signature(sig, &siglen, nullptr, 0, sk);
        auto start = __rdtsc();
        verify(sig, siglen, nullptr, 0, pk);
        auto end = __rdtsc();
        auto delta = end - start;
        total_time += delta / (double)repetitions;
    }
    return total_time;
}

static int wrap_verify_jazz(const uint8_t *sig, size_t siglen, const uint8_t *m, size_t mlen, const uint8_t *sk)
{
    (void)siglen;
    return VERIFY_JAZZ(sig, m, mlen, sk);
}

int main()
{
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "verify_jazz: " << 1e-3 * bench(wrap_verify_jazz) << " kcc\n";
    std::cout << "verify_ref: " << 1e-3 * bench(crypto_sign_verify) << " kcc\n";
    return 0;
}
