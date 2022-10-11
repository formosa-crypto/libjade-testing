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

extern "C" {
	void dilithium3_sampleInBall_jazz(int32_t f[256], uint8_t seed[32]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

int main() {
	uint8_t seed[32];
	int32_t f_jazz[256] = {0};
	poly f_ref = {0};

	if (DILITHIUM_MODE != 3) {
		// TODO: This test is only implemented for Dilithium3 because of
		// hashing_export.jazz.  Implement it also for the other variants.
		std::cerr << "skipped\n";
		return 0;
	}

	for(size_t i = 0; i < SEEDBYTES; i++) {
		seed[i] = sampleByte();
	}
	
	dilithium3_sampleInBall_jazz(f_jazz, seed);
	poly_challenge(&f_ref, seed);

	PRINT(memcmp(f_ref.coeffs, f_jazz, N * sizeof(int32_t)));
	return memcmp(f_ref.coeffs, f_jazz, N * sizeof(int32_t)) != 0;
}
