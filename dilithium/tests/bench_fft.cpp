#include <array>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

extern "C" {
#include <immintrin.h>
#ifdef DILITHIUM_ARCH_REF
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/poly.h"
#elif DILITHIUM_ARCH_AVX2
#include "../dilithium/avx2/api.h"
#include "../dilithium/avx2/params.h"
#include "../dilithium/avx2/poly.h"
#else
#error "None of DILITHIUM_ARCH_REF or DILITHIUM_ARCH_AVX2 is set"
#endif
}

using std::cout;
using std::vector;
using std::memcmp;
using std::runtime_error;
using std::to_string;
using std::array;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;

constexpr const int repetitions = 1000000;

extern "C" {
	void fft_jazz(int32_t f[N]);
	void ifft_to_mont_jazz(int32_t f[N]);
}

poly random_poly(bool want_neg = false) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distrib(want_neg ? (1 - Q) : 0, Q - 1);
	poly poly;
	for(int i = 0; i < N; ++i)
		poly.coeffs[i] = distrib(gen);
	return poly;
}

static double bench(void fn(poly*)) {
	double total_time = 0.0;
	for(int i = 0; i < repetitions; ++i) {
		auto poly = random_poly();
		auto start = __rdtsc();
		fn(&poly);
		auto end = __rdtsc();
		auto delta = end - start;
		total_time += delta / (double)repetitions;
	}
	return total_time;
}

static void wrap_fft_jazz(poly *poly) {
	fft_jazz(poly->coeffs);
}

int main() {
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "fft_jazz: " << 1e-3 * bench(wrap_fft_jazz) << " kcc\n";
	std::cout << "fft_ref: " << 1e-3 * bench(poly_ntt) << " kcc\n";
	return 0;
}
