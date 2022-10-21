#include <iostream>
#include <random>
#include <cstring>
#include <memory>
#include <string>
#include <stdexcept>

extern "C"
{
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/fips202.h"
#include "../dilithium/ref/polyvec.h"
#include "../dilithium/ref/poly.h"
#include "../dilithium/ref/packing.h"
}

using std::cout;
using std::endl;
using std::make_unique;
using std::memcmp;
using std::runtime_error;
using std::to_string;
using std::unique_ptr;
using std::vector;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C"
{
	void dilithium5_sign_scratch_jazz(uint8_t sig[CRYPTO_BYTES],
									  uint8_t *m,
									  uint64_t mlen,
									  uint8_t sk[CRYPTO_SECRETKEYBYTES]);
}

static void probe(uint8_t *m,
				  size_t mlen,
				  uint8_t *sk)
{
	uint8_t sig_ref[CRYPTO_BYTES], sig_jazz[CRYPTO_BYTES];

	unsigned int n;
	uint8_t seedbuf[3 * SEEDBYTES + 2 * CRHBYTES];
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
	polyveck_pack_w1(sig_ref, &w1);

	shake256_init(&state);
	shake256_absorb(&state, mu, CRHBYTES);
	shake256_absorb(&state, sig_ref, K * POLYW1_PACKEDBYTES);
	shake256_finalize(&state);
	shake256_squeeze(sig_ref, SEEDBYTES, &state);
	poly_challenge(&cp, sig_ref);

#if 0
	poly_ntt(&cp);

	// Compute z, reject if it reveals secret
	polyvecl_pointwise_poly_montgomery(&z, &cp, &s1);
	polyvecl_invntt_tomont(&z);
	polyvecl_add(&z, &z, &y);
	polyvecl_reduce(&z); // z =? cs1+y

	// PRINT(polyvecl_chknorm(&z, GAMMA1 - BETA));

	if (polyvecl_chknorm(&z, GAMMA1 - BETA))
		goto rej;

	// Check that subtracting cs2 does not change high bits of w and low bits
	// do not reveal secret information
	polyveck_pointwise_poly_montgomery(&h, &cp, &s2);
	polyveck_invntt_tomont(&h); // h = cs2 tested

	polyveck_sub(&w0, &w0, &h);
	polyveck_reduce(&w0); // correct up to here as w0 = r0

	if (polyveck_chknorm(&w0, GAMMA2 - BETA))
		goto rej;

	// Compute hints for w1
	polyveck_pointwise_poly_montgomery(&h, &cp, &t0);
	polyveck_invntt_tomont(&h);
	polyveck_reduce(&h); // h = ct0

	if (polyveck_chknorm(&h, GAMMA2))
		goto rej;

	polyveck_add(&w0, &w0, &h);
	n = polyveck_make_hint(&h, &w0, &w1);

	if (n > OMEGA)
		goto rej;

	// Write signature
	pack_sig(sig_ref, sig_ref, &z, &h);
#endif

	dilithium5_sign_scratch_jazz(sig_jazz, m, mlen, sk);

	// Observations:
	//   * w1 is still completely correct
	//   * ctilde is completely wrong (even first bytes)	
	//   => Either:
	//     - Hashing operation is incorrect

	// PRINT(((uint32_t *)sig_ref)[0]);
	// PRINT(((uint32_t *)sig_jazz)[0]);
	// PRINT(memcmp(sig_jazz, sig_ref, K*POLYW1_PACKEDBYTES));

	// PRINT((int) sig_ref[0]);
	// PRINT((int) sig_jazz[0]);
	// PRINT(memcmp(sig_jazz, sig_ref, SEEDBYTES));

	PRINT((int) sig_ref[0]);
	PRINT((int) sig_jazz[0]);
	PRINT(memcmp(sig_ref, sig_jazz, SEEDBYTES));
}

int main()
{
	uint8_t pk[CRYPTO_PUBLICKEYBYTES];
	uint8_t sk[CRYPTO_SECRETKEYBYTES];
	uint8_t m[1000] = {0};
	uint8_t randomness[32] = {0};
	for (int i = 0; i < 100; i++)
	{
		randomness[0] = i;
		pqcrystals_dilithium5_ref_seeded_keypair(pk, sk, randomness);
		probe(m, 1000, sk);
	}

	return 0;
}
