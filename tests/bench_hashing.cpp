#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>

extern "C" {
#include "macros.h"
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

constexpr const int repetitions = 1000000;

extern "C" {
	void SAMPLE_IN_BALL_JAZZ(int32_t f[N], const uint8_t seed[SEEDBYTES]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

static double bench(void fn(poly*, const uint8_t*)) {
	double total_time = 0.0;
	for(int i = 0; i < repetitions; ++i) {
		uint8_t seed[SEEDBYTES];
		poly poly;
		for (size_t j = 0; j < SEEDBYTES; j++) {
			seed[j] = sampleByte();
		}
		auto start = __rdtsc();
		fn(&poly, seed);
		auto end = __rdtsc();
		auto delta = end - start;
		total_time += delta / (double)repetitions;
	}
	return total_time;
}

static void wrap_sampleInBall_jazz(poly *poly, const uint8_t *seed) {
	SAMPLE_IN_BALL_JAZZ(poly->coeffs, seed);
}

int main() {
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "sampleInBall_jazz: " << bench(wrap_sampleInBall_jazz) << " kcc\n";
	std::cout << "sampleInBall_ref: " << bench(poly_challenge) << " kcc\n";
	return 0;
}
