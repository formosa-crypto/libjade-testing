#include <iostream>
#include <random>
#include <cstring>
#include <chrono>
#include <tuple>

extern "C" {
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
using std::endl;
using std::vector;
using std::memcmp;
using std::make_pair;
using std::pair;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;

constexpr const int repetitions = 1000000;

extern "C" {
	void sampleInBall_jazz(int32_t f[256], const uint8_t seed[32]);
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
		uint8_t seed[32];
		poly poly;
		for (size_t j = 0; j < 32; j++) {
			seed[j] = sampleByte();
		}
		auto start = high_resolution_clock::now();
		fn(&poly, seed);
		auto end = high_resolution_clock::now();
		auto duration = duration_cast<nanoseconds>(end - start);
		total_time += duration.count() / (double)repetitions;
	}
	return total_time;
}

static void wrap_sampleInBall_jazz(poly *poly, const uint8_t *seed) {
	sampleInBall_jazz(poly->coeffs, seed);
}

int main() {
	std::cout << "sampleInBall_jazz: " << bench(wrap_sampleInBall_jazz) << " ns\n";
	std::cout << "sampleInBall_ref: " << bench(poly_challenge) << " ns\n";
	return 0;
}
