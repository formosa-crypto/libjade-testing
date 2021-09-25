#include <iostream>
#include <random>
#include <cstring>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/poly.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

uint32_t sample_t1_component() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  1023);
	return distrib(gen);
}

int main() {
	uint8_t pk_ref[pqcrystals_dilithium3_PUBLICKEYBYTES];
	uint8_t sk_ref[pqcrystals_dilithium3_SECRETKEYBYTES];

	uint8_t randomness[32] = { 0 };
	/*
	for(int i = 0; i < 32; ++i) {
		randomness[i] = sampleByte();
	}
	*/

	pqcrystals_dilithium3_ref_seeded_keypair(pk_ref, sk_ref, randomness);


	/*
	PRINT(memcmp(sk_ref, sk_jazz, 64));
	PRINT(memcmp(sk_ref + 96, sk_jazz + 96, L * N / 2 + K * N / 2));
	PRINT(memcmp(pk_ref, pk_jazz, 32));
	PRINT(memcmp(pk_ref, pk_jazz, pqcrystals_dilithium3_PUBLICKEYBYTES));
	*/

	poly p;
	for(int i = 0; i < N; ++i) {
		p.coeffs[i] = sample_t1_component();
	}

	uint8_t buf[POLYT1_PACKEDBYTES];

	pqcrystals_dilithium3_ref_polyt1_pack(buf, &p);

	PRINT(int(buf[0]));
	PRINT(int(buf[1]));
	
	return 0;
}
