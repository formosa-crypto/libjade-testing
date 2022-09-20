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
	void SIGN_JAZZ(uint8_t signature[pqcrystals_dilithium3_BYTES],
	               uint8_t* msg,
	               uint64_t m_len,
	               uint8_t sk[pqcrystals_dilithium3_SECRETKEYBYTES]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

int main() {
	uint8_t pk[pqcrystals_dilithium3_PUBLICKEYBYTES];
	uint8_t sk[pqcrystals_dilithium3_SECRETKEYBYTES];

	/*
	uint8_t randomness[32] = { 0 };
	for(int i = 0; i < 32; ++i)
		randomness[i] = sampleByte();
		*/

	KEYGEN_REF(pk, sk);

	uint8_t m[1000];
	for(int i = 0; i < 1000; ++i)
		m[i] = sampleByte();


	uint8_t signature_ref[pqcrystals_dilithium3_BYTES];
	uint8_t signature_jazz[pqcrystals_dilithium3_BYTES];

	SIGN_JAZZ(signature_jazz, m, 1000, sk);

	size_t siglen;

	SIGN_REF(signature_ref, &siglen, m, 1000, sk);

	PRINT(memcmp(signature_ref, signature_jazz, pqcrystals_dilithium3_BYTES));

	PRINT(memcmp(signature_ref, signature_jazz, 32));

	int z_total_len = L * N * 20 / 8;
	int h_start = 32 + z_total_len;

	PRINT(memcmp(signature_ref + 32, signature_jazz + 32, z_total_len));

	PRINT(memcmp(signature_ref + h_start,
				signature_jazz + h_start,
				pqcrystals_dilithium3_BYTES - h_start));

	return 0;
}
