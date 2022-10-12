#include <iostream>
#include <random>
#include <cstring>
#include <chrono>
#include <tuple>

extern "C"
{
#include "macros.h"
#ifdef DILITHIUM_ARCH_REF
#include "../dilithium/ref/sign.h"
#include "../dilithium/ref/params.h"
#elif DILITHIUM_ARCH_AVX2
#include "../dilithium/avx2/api.h"
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
		auto start = high_resolution_clock::now();
		keygen(pk, sk, seed);
		auto end = high_resolution_clock::now();
		auto duration = duration_cast<nanoseconds>(end - start);
		total_time += duration.count() / (double)repetitions;
	}
	return total_time;
}

static int wrap_keygen_jazz(uint8_t pk[CRYPTO_PUBLICKEYBYTES], uint8_t sk[CRYPTO_SECRETKEYBYTES], uint8_t seed[SEEDBYTES])
{
	SEEDED_KEYGEN_JAZZ(pk, sk, seed);
	return 0;
}

int main()
{
	std::cout << "keygen_jazz: " << bench(wrap_keygen_jazz) << " ns\n";
	std::cout << "keygen_ref: " << bench(crypto_sign_seeded_keypair) << " ns\n";
	return 0;
}
