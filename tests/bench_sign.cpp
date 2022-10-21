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
	uint32_t SIGN_JAZZ(uint8_t sig[CRYPTO_BYTES],
					   const uint8_t *m,
					   uint64_t m_len,
					   const uint8_t sk[CRYPTO_SECRETKEYBYTES]);
}

uint8_t sampleByte()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0, 255);
	return distrib(gen);
}

static double bench(int sign(uint8_t *sig, size_t *siglen, const uint8_t *m, size_t mlen, const uint8_t *sk))
{
	double total_time = 0.0;
	for (int i = 0; i < repetitions; i++)
	{
		uint8_t sig[CRYPTO_BYTES] = {};
		size_t siglen;
		uint8_t pk[CRYPTO_PUBLICKEYBYTES] = {};
		uint8_t sk[CRYPTO_SECRETKEYBYTES] = {};
		crypto_sign_keypair(pk, sk);
		auto start = __rdtsc();
		sign(sig, &siglen, nullptr, 0, sk);
		auto end = __rdtsc();
		auto delta = end - start;
		total_time += delta / (double)repetitions;
	}
	return total_time;
}

static int wrap_sign_jazz(uint8_t *sig, size_t *siglen, const uint8_t *m, size_t mlen, const uint8_t *sk)
{
	(void)siglen;
	return SIGN_JAZZ(sig, m, mlen, sk);
}

int main()
{
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "sign_jazz: " << 1e-3 * bench(wrap_sign_jazz) << " kcc\n";
	std::cout << "sign_ref: " << 1e-3 * bench(crypto_sign_signature) << " kcc\n";
	return 0;
}
