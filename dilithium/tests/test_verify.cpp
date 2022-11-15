#include <iostream>
#include <random>
#include <cstring>

extern "C" {
	#include "macros.h"
	#include "../dilithium/ref/api.h"
	#include "../dilithium/ref/params.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	int VERIFY_JAZZ(
            uint8_t signature[CRYPTO_BYTES],
			uint8_t* msg,
			uint64_t m_len,
			uint8_t pk[CRYPTO_PUBLICKEYBYTES]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

static int test_functional() {
	uint8_t pk[CRYPTO_PUBLICKEYBYTES];
	uint8_t sk[CRYPTO_SECRETKEYBYTES];

	KEYGEN_REF(pk, sk);

	uint8_t m[1000];
	for(int i = 0; i < 1000; ++i) {
		m[i] = sampleByte();
	}

	uint8_t sig1[CRYPTO_BYTES];
	uint8_t sig2[CRYPTO_BYTES];
    size_t siglen;

	SIGN_REF(sig1, &siglen, m, 1000, sk);
	memcpy(sig2, sig1, CRYPTO_BYTES);
    int ref = VERIFY_REF(sig1, siglen, m, 1000, pk);
    int jazz = VERIFY_JAZZ(sig2, m, 1000, pk);

	if (ref != jazz) {
		std::cout << std::hex << "ref:  0x" << ref << std::endl;
		std::cout << std::hex << "jazz: 0x" << jazz << std::endl;
		return 1;
	}

	return 0;
}

int main() {
    for (int i = 0; i < 1000; i++) {
        int status = test_functional();
        if (status != 0) {
            return status;
        }
    }
    return 0;
}
