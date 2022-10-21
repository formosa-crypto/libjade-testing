#include <iostream>
#include <random>
#include <cstring>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "macros.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;

extern "C" {
	void SEEDED_KEYGEN_JAZZ(uint8_t pk[CRYPTO_PUBLICKEYBYTES],
			                uint8_t sk[CRYPTO_SECRETKEYBYTES],
			                uint8_t randomness[SEEDBYTES]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

int main() {
	uint8_t pk_ref[CRYPTO_PUBLICKEYBYTES];
	uint8_t sk_ref[CRYPTO_SECRETKEYBYTES];
	uint8_t pk_jazz[CRYPTO_PUBLICKEYBYTES];
	uint8_t sk_jazz[CRYPTO_SECRETKEYBYTES];

	uint8_t randomness[32] = { 0 };
	for(int i = 0; i < 32; ++i) {
		randomness[i] = sampleByte();
	}

	SEEDED_KEYGEN_REF(pk_ref, sk_ref, randomness);
	SEEDED_KEYGEN_JAZZ(pk_jazz, sk_jazz, randomness);

	for(int i = 0; i < CRYPTO_PUBLICKEYBYTES; ++i) {
		if(pk_ref[i] != pk_jazz[i]) {
			PRINT(i);
			PRINT((int)pk_ref[i]);
			PRINT((int)pk_jazz[i]);
			return 1;
		}
	}

	for(int i = 96; i < CRYPTO_SECRETKEYBYTES; ++i) {
		if(sk_ref[i] != sk_jazz[i]) {
			PRINT(i);
			PRINT((int)sk_ref[i]);
			PRINT((int)sk_jazz[i]);
			return 1;
		}
	}

	return 0;
}
