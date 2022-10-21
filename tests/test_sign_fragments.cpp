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
	#include "macros.h"
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
	void compute_mu_jazz(uint8_t* m, uint64_t m_len, uint8_t* sk, uint8_t* mu);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

void compute_mu_ref(uint8_t *out,
                          const uint8_t *m,
                          size_t mlen,
                          const uint8_t *sk)
{
  uint8_t seedbuf[3*SEEDBYTES + 2*CRHBYTES];
  uint8_t *rho, *tr, *key, *mu;
  polyvecl s1;
  polyveck t0, s2;
  keccak_state state;

  rho = seedbuf;
  tr = rho + SEEDBYTES;
  key = tr + SEEDBYTES;
  mu = key + SEEDBYTES;
  unpack_sk(rho, tr, key, &t0, &s1, &s2, sk);

  /* Compute CRH(tr, msg) */
  shake256_init(&state);
  shake256_absorb(&state, tr, SEEDBYTES);
  shake256_absorb(&state, m, mlen);
  shake256_finalize(&state);
  shake256_squeeze(mu, CRHBYTES, &state);

  for(int i = 0; i < 64; ++i) {
	  out[i] = mu[i];
  }
}

void test_compute_mu(int mlen) {
	uint8_t pk[CRYPTO_PUBLICKEYBYTES];
	uint8_t sk[CRYPTO_SECRETKEYBYTES];

	KEYGEN_REF(pk, sk);

	uint8_t mu_ref[64];
	uint8_t mu_jazz[64];

	auto m = make_unique<uint8_t[]>(mlen);
	
	for(int i = 0; i < mlen; ++i)
		m[i] = sampleByte();

	compute_mu_jazz(m.get(), mlen, sk, mu_jazz);
	compute_mu_ref(mu_ref, m.get(), mlen, sk);

	for(int i = 0; i < 64; ++i) {
		if(mu_ref[i] != mu_jazz[i]) {
			PRINT(i);
			PRINT(mu_ref[i]);
			PRINT(mu_jazz[i]);
			PRINT(mlen);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}
	}
}

int main() {
	test_compute_mu(0);
	test_compute_mu(1);
	test_compute_mu(10);
	test_compute_mu(100);
	test_compute_mu(1000);
	test_compute_mu(10000);
	return 0;
}

