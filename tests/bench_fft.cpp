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
using std::chrono::nanoseconds;
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

static double bench(void fn(int32_t*), int iterations=100000) {
	double total_time = 0.0;
	for(int i = 0; i < iterations; ++i) {
		auto arr = random_poly();
		auto start = high_resolution_clock::now();
		fn(arr.data());
		auto end = high_resolution_clock::now();
		auto duration = duration_cast<nanoseconds>(end - start);
		total_time += duration.count() / (double)iterations;
	}

	return total_time;
}

int main() {
	std::cout << "fft_jazz: " << bench(fft_jazz) << " ns\n";
	std::cout << "fft_ref: " << bench(ntt) << " ns\n";
	return 0;
}
