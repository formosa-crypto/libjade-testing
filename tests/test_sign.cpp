#include <iostream>
#include <random>
#include <cstring>

extern "C" {
	#include "../dilithium/ref/api.h"
	#include "../dilithium/ref/params.h"
	#include "macros.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;

extern "C" {
	int SIGN_REF(uint8_t *sig, size_t *siglen,
                 const uint8_t *m, size_t mlen,
                 const uint8_t *sk);
	int32_t SIGN_JAZZ(uint8_t signature[CRYPTO_BYTES],
	                  uint8_t* msg,
	                  uint64_t m_len,
	                  uint8_t sk[SECRETKEYBYTES]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

int main() {
	uint8_t pk[PUBLICKEYBYTES];
	uint8_t sk[SECRETKEYBYTES];
	size_t siglen;

	KEYGEN_REF(pk, sk);

	uint8_t m[1000];
	for(size_t i = 0; i < 1000; i++) {
		m[i] = sampleByte();
	}

	uint8_t signature_ref[CRYPTO_BYTES];
	uint8_t signature_jazz[CRYPTO_BYTES];

	int32_t status = SIGN_JAZZ(signature_jazz, m, 1000, sk);
	std::cout << std::hex << "status: 0x"  << status << "\n" << std::dec;

	SIGN_REF(signature_ref, &siglen, m, 1000, sk);

	for (size_t i = 0; i < CRYPTO_BYTES; i++) {
		if (signature_jazz[i] != signature_ref[i]) {
			PRINT(i);
			PRINT((int)signature_jazz[i]);
			PRINT((int)signature_ref[i]);
			break;
		}
	}

	PRINT(memcmp(signature_ref, signature_jazz, CRYPTO_BYTES));
	PRINT(memcmp(signature_ref, signature_jazz, 32));

	const int z_total_len = L * POLYZ_PACKEDBYTES;
	const int h_start = 32 + z_total_len;
	PRINT(memcmp(signature_ref + 32, signature_jazz + 32, z_total_len));

	PRINT(memcmp(signature_ref + h_start,
				signature_jazz + h_start,
				CRYPTO_BYTES - h_start));

	PRINT(VERIFY_REF(signature_ref, siglen, m, 1000, pk));
	PRINT(VERIFY_REF(signature_jazz, siglen, m, 1000, pk));

	return 0;
}
