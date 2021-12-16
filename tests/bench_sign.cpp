#include <iostream>
#include <random>
#include <cstring>
#include <chrono>
#include <tuple>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;
using std::chrono::high_resolution_clock;
using std::pair;
using std::make_pair;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void sign_jazz(uint8_t signature[pqcrystals_dilithium3_BYTES],
			uint8_t* msg,
			uint64_t m_len,
			uint8_t sk[pqcrystals_dilithium3_SECRETKEYBYTES]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

pair<double, int> bench_sign_jazz() {
	//forced side effect
	int accumulator = 0;

	auto start = high_resolution_clock::now();
	for(int i = 0; i < 5000; ++i) {
		uint8_t pk[pqcrystals_dilithium3_PUBLICKEYBYTES];
		uint8_t sk[pqcrystals_dilithium3_SECRETKEYBYTES];

		pqcrystals_dilithium3_ref_keypair(pk, sk);

		uint8_t m[1000];
		for(int i = 0; i < 1000; ++i)
			m[i] = sampleByte();

		uint8_t signature_jazz[pqcrystals_dilithium3_BYTES];
		sign_jazz(signature_jazz, m, 1000, sk);
		accumulator ^= signature_jazz[0];
	}
	auto end = high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	return make_pair(duration.count(), accumulator);
}

pair<double, int> bench_sign_ref() {
	//forced side effect
	int accumulator = 0;

	auto start = high_resolution_clock::now();
	for(int i = 0; i < 5000; ++i) {
		uint8_t pk[pqcrystals_dilithium3_PUBLICKEYBYTES];
		uint8_t sk[pqcrystals_dilithium3_SECRETKEYBYTES];

		pqcrystals_dilithium3_ref_keypair(pk, sk);

		uint8_t m[1000];
		for(int i = 0; i < 1000; ++i)
			m[i] = sampleByte();

		uint8_t signature_ref[pqcrystals_dilithium3_BYTES];
		size_t siglen;
		pqcrystals_dilithium3_ref_signature(signature_ref, &siglen, m, 1000, sk);
		accumulator ^= signature_ref[0];
	}
	auto end = high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	return make_pair(duration.count(), accumulator);

}

int main() {
	auto results_jazz = bench_sign_jazz();
	PRINT(results_jazz.first);
	PRINT(results_jazz.second);
	auto results_ref = bench_sign_ref();
	PRINT(results_ref.first);
	PRINT(results_ref.second);

	results_jazz = bench_sign_jazz();
	PRINT(results_jazz.first);
	PRINT(results_jazz.second);
	results_ref = bench_sign_ref();
	PRINT(results_ref.first);
	PRINT(results_ref.second);

	return 0;
}
