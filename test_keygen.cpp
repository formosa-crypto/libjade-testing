#include <iostream>
#include <random>

extern "C" {
#include "dilithium/ref/api.h"
}

using std::cout;
using std::endl;
using std::vector;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void keygen_jazz(uint8_t pk[pqcrystals_dilithium5_PUBLICKEYBYTES],
			uint8_t sk[pqcrystals_dilithium5_SECRETKEYBYTES],
			uint8_t randomness[SEEDBYTES]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

int main() {
	uint8_t pk[pqcrystals_dilithium5_PUBLICKEYBYTES];
	uint8_t sk[pqcrystals_dilithium5_SECRETKEYBYTES];
	uint8_t randomness[32];
	for(int i = 0; i < 32; ++i) {
		randomness[i] = sampleByte();
	}

	pqcrystals_dilithium5_ref_seeded_keypair(pk, sk, randomness);

	PRINT(int(pk[1]));
	PRINT(int(sk[1]));

	return 0;
}
