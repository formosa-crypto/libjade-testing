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
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::make_pair;
using std::pair;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void keygen_jazz(uint8_t pk[pqcrystals_dilithium3_PUBLICKEYBYTES],
			uint8_t sk[pqcrystals_dilithium3_SECRETKEYBYTES],
			uint8_t randomness[SEEDBYTES]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

pair<double, int> bench_keygen_jazz() {
	int accumulator = 0;

	auto start = high_resolution_clock::now();
	for(int i = 0; i < 5000; ++i) {

		uint8_t randomness[32] = { 0 };
		for(int i = 0; i < 32; ++i) {
			randomness[i] = sampleByte();
		}

		uint8_t pk_jazz[pqcrystals_dilithium3_PUBLICKEYBYTES];
		uint8_t sk_jazz[pqcrystals_dilithium3_SECRETKEYBYTES];
		keygen_jazz(pk_jazz, sk_jazz, randomness);

		accumulator ^= pk_jazz[0] ^ sk_jazz[0];
	}

	auto end = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(end - start);

	return make_pair(duration.count(), accumulator);
}

pair<double, int> bench_keygen_ref() {
	int accumulator = 0;

	auto start = high_resolution_clock::now();
	for(int i = 0; i < 5000; ++i) {

		uint8_t randomness[32] = { 0 };
		for(int i = 0; i < 32; ++i) {
			randomness[i] = sampleByte();
		}

		uint8_t pk_ref[pqcrystals_dilithium3_PUBLICKEYBYTES];
		uint8_t sk_ref[pqcrystals_dilithium3_SECRETKEYBYTES];

		pqcrystals_dilithium3_ref_seeded_keypair(pk_ref, sk_ref, randomness);

		accumulator ^= pk_ref[0] ^ sk_ref[0];
	}

	auto end = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(end - start);

	return make_pair(duration.count(), accumulator);
}

int main() {

	auto results_jazz = bench_keygen_jazz();
	PRINT(results_jazz.first);
	PRINT(results_jazz.second);
	auto results_ref = bench_keygen_ref();
	PRINT(results_ref.first);
	PRINT(results_ref.second);

	results_jazz = bench_keygen_jazz();
	PRINT(results_jazz.first);
	PRINT(results_jazz.second);
	results_ref = bench_keygen_ref();
	PRINT(results_ref.first);
	PRINT(results_ref.second);


	return 0;
}
