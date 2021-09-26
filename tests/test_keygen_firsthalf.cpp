#include <iostream>
#include <random>
#include <cstring>
#include <array>

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

}

polyveck keygen_upto_As1_ref(uint8_t seed[SEEDBYTES]) {
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

array<uint32_t, K * N> polyveck_to_arr(polyveck v) {
	array<uint32_t, K * N> arr;
	for(int k = 0; k < K; ++k) {
		for(int i = 0; i < N; ++i) {
			arr[k * N + i] = v.vec[k].coeffs[i];
		}
	}
	return arr;
}

int main() {
	uint8_t randomness[32] = { 0 };
	uint32_t as1_jazz[K * N];

	keygen_upto_As1_jazz(randomness, as1_jazz);

	polyveck as1_ref = keygen_upto_As1_ref(randomness);
	auto as1_ref_arr = polyveck_to_arr(as1_ref);

	PRINT(memcmp(as1_jazz, as1_ref_arr.data(), 4 * K * N));	

	return 0;
}
