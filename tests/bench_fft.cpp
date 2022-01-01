#include <iostream>
#include <random>
#include <cstring>
#include <array>
#include <string>
#include <chrono>
#include <tuple>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/ntt.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;
using std::runtime_error;
using std::to_string;
using std::array;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::make_pair;
using std::pair;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void fft_jazz(int32_t f[N]);
	void ifft_to_mont_jazz(int32_t f[N]);
}


array<int32_t, N> random_poly(bool want_neg = false) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distrib(want_neg ? (1 - Q) : 0, Q - 1);
	array<int32_t, N> arr;
	for(int i = 0; i < N; ++i)
		arr[i] = distrib(gen);
	return arr;
}

pair<double, int> bench_fft_jazz() {
	int accumulator = 0;

	auto start = high_resolution_clock::now();
	for(int i = 0; i < (1 << 18); ++i) {
		auto arr = random_poly();
		fft_jazz(arr.data());
		accumulator += arr[0];
		accumulator %= 1000;
	}

	auto end = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(end - start);

	return make_pair(duration.count(), accumulator);
}

pair<double, int> bench_fft_ref() {
	int accumulator = 0;

	auto start = high_resolution_clock::now();
	for(int i = 0; i < (1 << 18); ++i) {
		auto arr = random_poly();
		ntt(arr.data());
		accumulator += arr[0];
		accumulator %= 1000;
	}

	auto end = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(end - start);

	return make_pair(duration.count(), accumulator);
}

int main() {
	auto results_jazz = bench_fft_jazz();
	PRINT(results_jazz.first);
	PRINT(results_jazz.second);
	auto results_ref = bench_fft_ref();
	PRINT(results_ref.first);
	PRINT(results_ref.second);

	results_jazz = bench_fft_jazz();
	PRINT(results_jazz.first);
	PRINT(results_jazz.second);
	results_ref = bench_fft_ref();
	PRINT(results_ref.first);
	PRINT(results_ref.second);
	return 0;
}
