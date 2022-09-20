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
	void SEEDED_KEYGEN_JAZZ(uint8_t pk[PUBLICKEYBYTES],
			                uint8_t sk[SECRETKEYBYTES],
			                uint8_t randomness[SEEDBYTES]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

int main() {
	uint8_t pk_ref[PUBLICKEYBYTES];
	uint8_t sk_ref[SECRETKEYBYTES];
	uint8_t pk_jazz[PUBLICKEYBYTES];
	uint8_t sk_jazz[SECRETKEYBYTES];

	uint8_t randomness[32] = { 0 };
	for(int i = 0; i < 32; ++i) {
		randomness[i] = sampleByte();
	}

	SEEDED_KEYGEN_REF(pk_ref, sk_ref, randomness);
	SEEDED_KEYGEN_JAZZ(pk_jazz, sk_jazz, randomness);

	PRINT(memcmp(pk_ref, pk_jazz, PUBLICKEYBYTES));
	PRINT(memcmp(sk_ref, sk_jazz, SECRETKEYBYTES));

	for(int i = 0; i < PUBLICKEYBYTES; ++i) {
		if(pk_ref[i] != pk_jazz[i]) {
			PRINT(i);
			break;
		}
	}

	for(int i = 96; i < SECRETKEYBYTES; ++i) {
		if(sk_ref[i] != sk_jazz[i]) {
			PRINT(i);
			break;
		}
	}

	return 0;
}
