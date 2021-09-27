#include <iostream>
#include <random>
#include <cstring>
#include <array>
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

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void keygen_upto_As1_jazz(uint8_t randomness[SEEDBYTES],
			uint32_t as1[K * N]);
	void probe_keygen_jazz(uint32_t s1[L * N]);
}

// Copied from reference impl.
polyvecl keygen_firsthalf_ref(uint8_t seed[SEEDBYTES]) {
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
	//polyvecl_ntt(&s1hat);
	return s1hat;
	/*
	polyvec_matrix_pointwise_montgomery(&t1, mat, &s1hat);
	polyveck_reduce(&t1);
	polyveck_invntt_tomont(&t1);

	return t1;
	*/
}

array<uint32_t, K * N> polyveck_to_arr(polyveck v) {
	array<uint32_t, K * N> arr;
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
	uint32_t s1_jazz[K * N];
	probe_keygen_jazz(s1_jazz);

	uint8_t randomness[32] = { 0 };
	polyvecl s1_ref = keygen_firsthalf_ref(randomness);
	auto s1_ref_arr = polyvecl_to_arr(s1_ref);

	for(int i = 0; i < L * N; ++i) {
		uint32_t ref_i = (s1_ref_arr[i] + Q) % Q;
		if(s1_jazz[i] % Q != ref_i) {
			PRINT(i);
			PRINT(s1_jazz[i]);
			PRINT(s1_ref_arr[i]);
			return 0;
		}
	}

	print_poly(s1_jazz);

	return 0;
}
