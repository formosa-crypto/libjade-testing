#include <iostream>
#include <random>
#include <cstring>
#include <memory>
#include <string>
#include <stdexcept>
#include <array>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/polyvec.h"
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
	void expandMask_jazz(uint8_t seed[64], uint16_t nonce, int32_t y[N * L]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

array<int32_t, L * N> polyvecl_to_arr(polyvecl const& v) {
	array<int32_t, L * N> arr;
	for(int k = 0; k < L; ++k) {
		for(int i = 0; i < N; ++i) {
			arr[k * N + i] = v.vec[k].coeffs[i];
		}
	}
	return arr;
}


uint16_t sampleNonce() {
	uint16_t upper = sampleByte();
	upper <<= 8;
	return upper | sampleByte();
}

int main() {
	uint8_t seed[64];
	for(int i = 0; i < 64; ++i)
		seed[i] = sampleByte();
	uint16_t nonce = sampleNonce();
	
	polyvecl y_ref;
	polyvecl_uniform_gamma1(&y_ref, seed, nonce);
	auto y_ref_arr = polyvecl_to_arr(y_ref);

	int32_t y_jazz[N * L];
	expandMask_jazz(seed, nonce, y_jazz);

	for(int i = 0; i < N * L; ++i) {
		if(y_jazz[i] != y_ref_arr[i]) {
			PRINT(i);
			PRINT(y_jazz[i]);
			PRINT(y_ref_arr[i]);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}
	}

	return 0;
}
