#include <iostream>
#include <random>
#include <cstring>
#include <chrono>
#include <tuple>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/poly.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;
using std::make_pair;
using std::pair;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

constexpr const int repetitions = 500000;

extern "C" {
	void sampleInBall_jazz(int32_t f[256], uint8_t seed[32]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

pair<double, int> bench_hashing_jazz() {
	int accumulator = 0;
	auto start = high_resolution_clock::now();

	for(int i = 0; i < repetitions; ++i) {
		uint8_t seed[32];
		for(int i = 0; i < 32; ++i)
			seed[i] = sampleByte();

		int32_t f_jazz[256];
		sampleInBall_jazz(f_jazz, seed);

		accumulator += (f_jazz[0]);
		accumulator %= 1000;
	}

	auto end = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(end - start);

	return make_pair(duration.count(), accumulator);

}

pair<double, int> bench_hashing_ref() {
	int accumulator = 0;
	auto start = high_resolution_clock::now();

	for(int i = 0; i < repetitions; ++i) {
		uint8_t seed[32];
		for(int i = 0; i < 32; ++i)
			seed[i] = sampleByte();

		poly f_ref;
		poly_challenge(&f_ref, seed);

		accumulator += (f_ref.coeffs[0]);
		accumulator %= 1000;
	}

	auto end = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(end - start);

	return make_pair(duration.count(), accumulator);
}

int main() {
	auto results_jazz = bench_hashing_jazz();
	PRINT(results_jazz.first);
	PRINT(results_jazz.second);
	auto results_ref = bench_hashing_ref();
	PRINT(results_ref.first);
	PRINT(results_ref.second);

	results_jazz = bench_hashing_jazz();
	PRINT(results_jazz.first);
	PRINT(results_jazz.second);
	results_ref = bench_hashing_ref();
	PRINT(results_ref.first);
	PRINT(results_ref.second);
	return 0;
}
