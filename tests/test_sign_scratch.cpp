#include <iostream>
#include <random>
#include <cstring>
#include <memory>
#include <string>
#include <stdexcept>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/fips202.h"
#include "../dilithium/ref/polyvec.h"
#include "../dilithium/ref/poly.h"
#include "../dilithium/ref/packing.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;
using std::unique_ptr;
using std::make_unique;
using std::to_string;
using std::runtime_error;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void test_jazz(uint8_t m[1000],
			uint8_t sk[pqcrystals_dilithium3_SECRETKEYBYTES],
//			uint8_t out_buf[5000]);
			uint32_t out_buf32[5000]);
}

void probe(uint8_t *sig,
		uint8_t *m,
		size_t mlen,
		uint8_t *sk)
{
	unsigned int n;
	uint8_t seedbuf[3*SEEDBYTES + 2*CRHBYTES];
	uint8_t *rho, *tr, *key, *mu, *rhoprime;
	uint16_t nonce = 0;
	polyvecl mat[K], s1, y, z;
	polyveck t0, s2, w1, w0, h;
	poly cp;
	keccak_state state;

	rho = seedbuf;
	tr = rho + SEEDBYTES;
	key = tr + SEEDBYTES;
	mu = key + SEEDBYTES;
	rhoprime = mu + CRHBYTES;
	unpack_sk(rho, tr, key, &t0, &s1, &s2, sk);

	shake256_init(&state);
	shake256_absorb(&state, tr, SEEDBYTES);
	shake256_absorb(&state, m, mlen);
	shake256_finalize(&state);
	shake256_squeeze(mu, CRHBYTES, &state);

	shake256(rhoprime, CRHBYTES, key, SEEDBYTES + CRHBYTES);

	// Expand matrix and transform vectors
	polyvec_matrix_expand(mat, rho);
	polyvecl_ntt(&s1);
	polyveck_ntt(&s2);
	polyveck_ntt(&t0);

rej:
	// Sample intermediate vector y
	polyvecl_uniform_gamma1(&y, rhoprime, nonce++);

	// Matrix-vector multiplication
	z = y;
	polyvecl_ntt(&z);
	polyvec_matrix_pointwise_montgomery(&w1, mat, &z);
	polyveck_reduce(&w1);
	polyveck_invntt_tomont(&w1);

	// Decompose w and call the random oracle
	polyveck_caddq(&w1);
	polyveck_decompose(&w1, &w0, &w1);
	polyveck_pack_w1(sig, &w1);

	shake256_init(&state);
	shake256_absorb(&state, mu, CRHBYTES);
	shake256_absorb(&state, sig, K*POLYW1_PACKEDBYTES);
	shake256_finalize(&state);
	shake256_squeeze(sig, SEEDBYTES, &state);
	poly_challenge(&cp, sig);

	poly_ntt(&cp);

	// Compute z, reject if it reveals secret
	polyvecl_pointwise_poly_montgomery(&z, &cp, &s1);
	polyvecl_invntt_tomont(&z);
	polyvecl_add(&z, &z, &y);
	polyvecl_reduce(&z);

	uint32_t out_buf32[5000];
	test_jazz(m, sk, out_buf32);

	PRINT(out_buf32[0]);
	PRINT(out_buf32[1]);
	PRINT(out_buf32[2]);

	PRINT(z.vec[0].coeffs[0]);
	PRINT(z.vec[0].coeffs[1]);
	PRINT(z.vec[0].coeffs[2]);

	for(int i = 0; i < L; ++i) {
		for(int j = 0; j < N; ++j) {
			int zval = ((z.vec[i].coeffs[j] % Q) + Q) % Q;
			if(zval != out_buf32[i * N + j]) {
				PRINT(i);
				PRINT(j);
				PRINT(out_buf32[i * N + j]);
				PRINT(zval);
				return;
			}
		}
	}

	/*
	for(int i = 0; i < N; ++i) {
		uint32_t cval = (cp.coeffs[i] % Q + Q) % Q;
		if(cval != out_buf32[i]) {
			PRINT(i);
			PRINT(out_buf32[i]);
			PRINT(cp.coeffs[i]);
			PRINT(cval);
			return;
		}
	}
	*/

	/*
	if(polyvecl_chknorm(&z, GAMMA1 - BETA))
		goto rej;

	// Check that subtracting cs2 does not change high bits of w and low bits
	// do not reveal secret information
	polyveck_pointwise_poly_montgomery(&h, &cp, &s2);
	polyveck_invntt_tomont(&h);
	polyveck_sub(&w0, &w0, &h);
	polyveck_reduce(&w0);
	if(polyveck_chknorm(&w0, GAMMA2 - BETA))
		goto rej;

	// Compute hints for w1
	polyveck_pointwise_poly_montgomery(&h, &cp, &t0);
	polyveck_invntt_tomont(&h);
	polyveck_reduce(&h);
	if(polyveck_chknorm(&h, GAMMA2))
		goto rej;

	polyveck_add(&w0, &w0, &h);
	n = polyveck_make_hint(&h, &w0, &w1);
	if(n > OMEGA)
		goto rej;

	// Write signature
	pack_sig(sig, sig, &z, &h);
*/
}

int main() {
	uint8_t pk[pqcrystals_dilithium3_PUBLICKEYBYTES];
	uint8_t sk[pqcrystals_dilithium3_SECRETKEYBYTES];
	uint8_t randomness[32] = { 0 };
	pqcrystals_dilithium3_ref_seeded_keypair(pk, sk, randomness);

	uint8_t sig[pqcrystals_dilithium3_BYTES];

	uint8_t m[1000] = { 0 };
	probe(sig, m, 1000, sk);

	return 0;
}

