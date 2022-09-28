#include <iostream>
#include <random>
#include <cstring>
#include <memory>
#include <string>
#include <stdexcept>
#include <array>

extern "C" {
	#include "macros.h"
	#include "../dilithium/ref/params.h"
	#include "../dilithium/ref/poly.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;
using std::unique_ptr;
using std::make_unique;
using std::to_string;
using std::runtime_error;
using std::array;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void POLY_UNIFORM_GAMMA1_REF(poly *a, const uint8_t seed[CRHBYTES], uint16_t nonce);
	void POLY_UNIFORM_GAMMA1_JAZZ(uint8_t seed[64], uint16_t nonce, int32_t y[N]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

array<int32_t, N> poly_to_arr(poly const& v) {
	array<int32_t, N> arr;
	for(int i = 0; i < N; i++) {
		arr[i] = v.coeffs[i];
	}
	return arr;
}


uint16_t sampleNonce() {
	uint16_t upper = sampleByte();
	upper |= sampleByte() << 8;
	return upper;
}

int main() {
	uint8_t seed[64];
	for(int i = 0; i < 64; i++) {
		seed[i] = sampleByte();
	}
	uint16_t nonce = sampleNonce();
	
	poly y_ref;
	POLY_UNIFORM_GAMMA1_REF(&y_ref, seed, nonce);
	auto y_ref_arr = poly_to_arr(y_ref);

	int32_t y_jazz[N];
	POLY_UNIFORM_GAMMA1_JAZZ(seed, nonce, y_jazz);

	for(int i = 0; i < N; i++) {
		if(y_jazz[i] != y_ref_arr[i]) {
			PRINT(i);
			PRINT(y_jazz[i]);
			PRINT(y_ref_arr[i]);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}
	}

	return 0;
}
