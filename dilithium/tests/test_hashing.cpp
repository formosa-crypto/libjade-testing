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
#include "macros.h"
void SAMPLE_IN_BALL_JAZZ(int32_t f[N], uint8_t seed[SEEDBYTES]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

int main() {
	uint8_t seed[SEEDBYTES];
	int32_t f_jazz[N] = {0};
	poly f_ref = {0};

	for(size_t i = 0; i < SEEDBYTES; i++) {
		seed[i] = sampleByte();
	}
	
	SAMPLE_IN_BALL_JAZZ(f_jazz, seed);
	poly_challenge(&f_ref, seed);

	PRINT(memcmp(f_ref.coeffs, f_jazz, N * sizeof(int32_t)));
	return memcmp(f_ref.coeffs, f_jazz, N * sizeof(int32_t)) != 0;
}
