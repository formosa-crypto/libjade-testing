#include <iostream>
#include <random>
#include <cstring>
#include <array>
#include <string>
#include "print_poly.h"

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/poly.h"
#include "../dilithium/ref/polyvec.h"
#include "../dilithium/ref/fips202.h"
}

using std::array;
using std::cout;
using std::endl;
using std::vector;
using std::memcmp;
using std::runtime_error;
using std::to_string;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void keygen_upto_As1_jazz(uint8_t randomness[SEEDBYTES],
			uint32_t as1[K * N]);
	void probe_keygen_jazz(uint32_t s1[L * N]);
}

// Copied from reference impl.
polyveck keygen_firsthalf_ref(uint8_t seed[SEEDBYTES]) {
	uint8_t seedbuf[2*SEEDBYTES + CRHBYTES];
	const uint8_t *rho, *rhoprime;
	polyvecl mat[K];
	polyvecl s1, s1hat;
	polyveck t1;

	/* Get randomness for rho, rhoprime and key */
	for(int i = 0; i < SEEDBYTES; ++i)
		seedbuf[i] = seed[i];
	shake256(seedbuf, 2*SEEDBYTES + CRHBYTES, seedbuf, SEEDBYTES);
	rho = seedbuf;
	rhoprime = rho + SEEDBYTES;

	/* Expand matrix */
	polyvec_matrix_expand(mat, rho);

	/* Sample short vectors s1 and s2 */
	polyvecl_uniform_eta(&s1, rhoprime, 0);

	/* Matrix-vector multiplication */

	s1hat = s1;
	polyvecl_ntt(&s1hat);

	polyvec_matrix_pointwise_montgomery(&t1, mat, &s1hat);

	polyveck_reduce(&t1);

	polyveck_invntt_tomont(&t1);

	return t1;
}

array<int32_t, K * N> polyveck_to_arr(polyveck v) {
	array<int32_t, K * N> arr;
	for(int k = 0; k < K; ++k) {
		for(int i = 0; i < N; ++i) {
			arr[k * N + i] = v.vec[k].coeffs[i];
		}
	}
	return arr;
}

array<int32_t, L * N> polyvecl_to_arr(polyvecl v) {
	array<int32_t, L * N> arr;
	for(int k = 0; k < L; ++k) {
		for(int i = 0; i < N; ++i) {
			arr[k * N + i] = v.vec[k].coeffs[i];
		}
	}
	return arr;
}

int main() {
	uint8_t randomness[32] = { 0 };
	uint32_t t_jazz[K * N];
//	probe_keygen_jazz(t_jazz);
	keygen_upto_As1_jazz(randomness, t_jazz);

	polyveck t_ref = keygen_firsthalf_ref(randomness);
	auto t_ref_arr = polyveck_to_arr(t_ref);

	/*
	for(int i = 0; i < K * N; ++i) {
		int32_t v = t_ref_arr[i];
		v = (v % Q + Q) % Q;
		t_ref_arr[i] = v;
	}

	for(int i = 0; i < K; ++i) {
		PRINT(i);
		print_poly(t_jazz + i * N);
		cout << endl;
		print_poly(t_ref_arr.data() + i * N);
		cout << endl << endl;
	}
	*/

	for(int i = 0; i < K * N; ++i) {
		uint32_t ref_i = ((t_ref_arr[i] % Q) + Q) % Q;
		if(t_jazz[i] % Q != ref_i) {
			PRINT(i);
			PRINT(t_jazz[i]);
			PRINT(ref_i);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}
	}



	return 0;
}
