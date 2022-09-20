#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <cstring>

extern "C" {
	#include "../dilithium/ref/api.h"
	#include "../dilithium/ref/params.h"
	#include "../dilithium/ref/rounding.h"
	#include "../dilithium/ref/polyvec.h"
	#include "macros.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;
using std::runtime_error;
using std::to_string;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void DECOMPOSE_JAZZ(int32_t a, int32_t* a0, int32_t* a1);
	uint32_t make_hint_jazz(int32_t a0, int32_t a1);
	void power2round_jazz(int32_t a, int32_t* a0, int32_t* a1);
}

int32_t sample() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  Q - 1);
	return distrib(gen);
}

void test_power2round() {
	for(int i = 0; i < 500; ++i) {
		int32_t a = sample();
		int32_t h_jazz, l_jazz, h_ref, l_ref;
		power2round_jazz(a, &h_jazz, &l_jazz);
		h_ref = power2round(&l_ref, a);
		if(h_ref != h_jazz || l_ref != l_jazz) {
			PRINT(a);
			PRINT(h_ref);
			PRINT(h_jazz);
			PRINT(l_ref);
			PRINT(l_jazz);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}
	}
}

int main() {
	for(int i = 0; i < 10000; ++i) {
		int32_t a, a0_ref, a1_ref, a0_jazz, a1_jazz;

		a = sample();
		DECOMPOSE_JAZZ(a, &a0_jazz, &a1_jazz);
		a1_ref = DECOMPOSE_REF(&a0_ref, a);

		if(a0_ref != a0_jazz) {
			PRINT(a);
			PRINT(a0_ref);
			PRINT(a0_jazz);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}

		if(a1_ref != a1_jazz) {
			PRINT(a);
			PRINT(a1_ref);
			PRINT(a1_jazz);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}

		test_power2round();

		// int32_t vec_jazz[K * N];
		// int32_t vec0_jazz[K * N];
		// int32_t vec1_jazz[K * N];

		// polyveck vec_ref;
		// polyveck vec0_ref;
		// polyveck vec1_ref;

		// for(int i = 0; i < K; ++i) {
		// 	for(int j = 0; j < N; ++j) {
		// 		int s = sample();
		// 		vec_jazz[i * N + j] = s;
		// 		vec_ref.vec[i].coeffs[j] = s;
		// 	}
		// }

		// decompose_vec_jazz(vec_jazz, vec0_jazz, vec1_jazz);
		// polyveck_decompose(&vec1_ref, &vec0_ref, &vec_ref);
		
		// for(int i = 0; i < K; ++i) {
		// 	for(int j = 0; j < N; ++j) {
		// 		if(vec0_jazz[i * N + j] != vec0_ref.vec[i].coeffs[j]) {
		// 			PRINT(i);
		// 			PRINT(j);
		// 			PRINT(vec0_jazz[i * N + j]);
		// 			PRINT(vec0_ref.vec[i].coeffs[j]);
		// 			PRINT(vec_jazz[i * N + j]);
		// 			throw runtime_error("test failed at " + to_string(__LINE__));
		// 		}
		// 		if(vec1_jazz[i * N + j] != vec1_ref.vec[i].coeffs[j]) {
		// 			PRINT(i);
		// 			PRINT(j);
		// 			PRINT(vec1_jazz[i * N + j]);
		// 			PRINT(vec1_ref.vec[i].coeffs[j]);
		// 			throw runtime_error("test failed at " + to_string(__LINE__));
		// 		}
		// 	}
		// }

		// unsigned int hint_ref = make_hint(a0_ref, a1_ref);
		// uint32_t hint_jazz = make_hint_jazz(a0_jazz, a1_jazz);

		// if(hint_ref != hint_jazz) {
		// 	PRINT(a0_ref);
		// 	PRINT(a1_ref);
		// 	PRINT(hint_ref);
		// 	PRINT(hint_jazz);
		// 	throw runtime_error("test failed at " + to_string(__LINE__));
		// }
	}
}
