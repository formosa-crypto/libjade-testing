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
	void sampleInBall_jazz(int32_t f[256], uint8_t seed[32]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

int main() {
	uint8_t seed[32];
	for(int i = 0; i < 32; ++i)
		seed[i] = sampleByte();

	int32_t f_jazz[256];
	sampleInBall_jazz(f_jazz, seed);

	poly f_ref;
	poly_challenge(&f_ref, seed);

	PRINT(memcmp(f_ref.coeffs, f_jazz, 4 * 256));

	return 0;
}
