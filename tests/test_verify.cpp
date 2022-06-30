#include <iostream>
#include <random>
#include <cstring>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	int verify_jazz(
            uint8_t signature[pqcrystals_dilithium3_BYTES],
			uint8_t* msg,
			uint64_t m_len,
			uint8_t pk[pqcrystals_dilithium3_PUBLICKEYBYTES]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

static int test_functional() {
	uint8_t pk[pqcrystals_dilithium3_PUBLICKEYBYTES];
	uint8_t sk[pqcrystals_dilithium3_SECRETKEYBYTES];

	pqcrystals_dilithium3_ref_keypair(pk, sk);

	uint8_t m[1000];
	for(int i = 0; i < 1000; ++i) {
		m[i] = sampleByte();
	}

	uint8_t sig1[pqcrystals_dilithium3_BYTES];
	uint8_t sig2[pqcrystals_dilithium3_BYTES];
    size_t siglen;

	pqcrystals_dilithium3_ref_signature(sig1, &siglen, m, 1000, sk);
	memcpy(sig2, sig1, pqcrystals_dilithium3_BYTES);
    int ref = pqcrystals_dilithium3_ref_verify(sig1, siglen, m, 1000, pk);
    int jazz = verify_jazz(sig2, m, 1000, pk);

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
