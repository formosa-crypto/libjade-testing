#include <iostream>
#include <random>
#include <cstring>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void keygen_jazz(uint8_t pk[pqcrystals_dilithium3_PUBLICKEYBYTES],
			uint8_t sk[pqcrystals_dilithium3_SECRETKEYBYTES],
			uint8_t randomness[SEEDBYTES]);
}

/*
uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}
*/

int main() {
	uint8_t pk_ref[pqcrystals_dilithium3_PUBLICKEYBYTES];
	uint8_t sk_ref[pqcrystals_dilithium3_SECRETKEYBYTES];
	uint8_t pk_jazz[pqcrystals_dilithium3_PUBLICKEYBYTES];
	uint8_t sk_jazz[pqcrystals_dilithium3_SECRETKEYBYTES];

	uint8_t randomness[32] = { 0 };
	/*
	for(int i = 0; i < 32; ++i) {
		randomness[i] = sampleByte();
	}
	*/

	pqcrystals_dilithium3_ref_seeded_keypair(pk_ref, sk_ref, randomness);
	keygen_jazz(pk_jazz, sk_jazz, randomness);

	//PRINT(memcmp(pk_ref, pk_jazz, pqcrystals_dilithium3_PUBLICKEYBYTES));

	PRINT(memcmp(sk_ref, sk_jazz, 64));
	PRINT(memcmp(sk_ref + 96, sk_jazz + 96, L * N / 2 + K * N / 2));
	PRINT(memcmp(pk_ref, pk_jazz, 32));
	//PRINT(memcmp(pk_ref, pk_jazz, pqcrystals_dilithium3_PUBLICKEYBYTES));

	return 0;
}
