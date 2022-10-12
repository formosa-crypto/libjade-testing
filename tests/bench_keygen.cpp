#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>
#include <tuple>

extern "C"
{
#include "macros.h"
#include <immintrin.h>
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
using std::endl;
using std::make_pair;
using std::memcmp;
using std::pair;
using std::vector;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::nanoseconds;

constexpr const int repetitions = 10000;

extern "C"
{
	void SEEDED_KEYGEN_JAZZ(uint8_t pk[CRYPTO_PUBLICKEYBYTES],
							uint8_t sk[CRYPTO_SECRETKEYBYTES],
							uint8_t randomness[SEEDBYTES]);
}

uint8_t sampleByte()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0, 255);
	return distrib(gen);
}

static double bench(int keygen(uint8_t pk[CRYPTO_PUBLICKEYBYTES], uint8_t sk[CRYPTO_SECRETKEYBYTES], uint8_t seed[SEEDBYTES]))
{
	double total_time = 0.0;
	for (int i = 0; i < repetitions; i++)
	{
		uint8_t pk[CRYPTO_PUBLICKEYBYTES] = {};
		uint8_t sk[CRYPTO_SECRETKEYBYTES] = {};
		uint8_t seed[SEEDBYTES] = {};
		for (int i = 0; i < SEEDBYTES; ++i)
		{
			seed[i] = sampleByte();
		}
		auto start = __rdtsc();
		keygen(pk, sk, seed);
		auto end = __rdtsc();
		auto delta = end - start;
		total_time += delta / (double)repetitions;
	}
	return total_time;
}

static int wrap_keygen_jazz(uint8_t pk[CRYPTO_PUBLICKEYBYTES], uint8_t sk[CRYPTO_SECRETKEYBYTES], uint8_t seed[SEEDBYTES])
{
	SEEDED_KEYGEN_JAZZ(pk, sk, seed);
	return 0;
}

static int wrap_keygen_ref(uint8_t pk[CRYPTO_PUBLICKEYBYTES], uint8_t sk[CRYPTO_SECRETKEYBYTES], uint8_t seed[SEEDBYTES])
{
	// The Dilithium avx2 implementation never implemented the 'seeded'
	// function, so in that case we call the unseeded function and throw away
	// the seed.
#ifdef DILITHIUM_ARCH_REF
	crypto_sign_seeded_keypair(pk, sk, seed);
#elif DILITHIUM_ARCH_AVX2
	(void)seed;
	crypto_sign_keypair(pk, sk);
#else
#error
#endif
	return 0;
}

int main()
{
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "keygen_jazz: " << 1e-3 * bench(wrap_keygen_jazz) << " kcc\n";
	std::cout << "keygen_ref: " << 1e-3 * bench(wrap_keygen_ref) << " kcc\n";
	return 0;
}
