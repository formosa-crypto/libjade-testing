#include <iostream>
#include <random>
#include <cstring>
#include <array>
#include <string>
#include <stdexcept>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/poly.h"
#include "../dilithium/ref/polyvec.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;
using std::array;
using std::to_string;
using std::runtime_error;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void mult_mat_vec_jazz(uint32_t m[K * L * N], uint32_t v[L * N], uint32_t prod[K * N]);
	void dot_prod_jazz(uint32_t v1[L * N], uint32_t v2[L * N], uint32_t prod[N]);
	void fft_poly_mult_jazz(uint32_t f[N], uint32_t g[N], uint32_t fg[N]);
	void poly_accumulate_jazz(uint32_t f[N], uint32_t s[N]);

	void probe_mult_mat_vec_jazz(uint32_t m[K * L * N], uint32_t v[L * N], uint32_t prod[K * N]);
}

template<typename T> void print_poly(T f[N]) {
	for(int i = 0; i < 16; ++i) {
		cout << f[16 * i];
		for(int j = 1; j < 16; ++j) {
			cout << ' ' << f[16 * i + j];
		}
		cout << endl;
	}
}

void test_dot_prod() {
	//[0, 1, 2, ...]
	uint32_t v1[L * N] = { 0 };
	for(int i = 0; i < L; ++i) {
		v1[i * N] = i;
	}
	/*
	cout << "v1 =";
	for(int i = 0; i < L; ++i) {
		cout << endl;
		print_poly(v1 + i * N);
	}
	*/


	//[1, x, x^2, ...]
	uint32_t v2[L * N] = { 0 };
	for(int i = 0; i < L; ++i) {
		v2[i * N + i] = 1;
	}
	/*
	cout << "v2 =";
	for(int i = 0; i < L; ++i) {
		cout << endl;
		print_poly(v2 + i * N);
	}
	*/

	//expecting all 0s
	uint32_t prod12[N];
	dot_prod_jazz(v1, v2, prod12);

	/*
	cout << "prod12 =" << endl;
	print_poly(prod12);
	*/

	for(int i = 0; i < N; ++i)
		if(prod12[i] != 0)
			throw runtime_error("test failed at " + to_string(__LINE__));

	//[1, 2, 4, 8, ...]
	uint32_t v3[L * N] = { 0 };
	for(int i = 0; i < L; ++i) {
		v3[i * N] = (1 << i);
	}

	/*
	cout << "v3 =";
	for(int i = 0; i < L; ++i) {
		cout << endl;
		print_poly(v3 + i * N);
	}
	*/

	uint32_t prod13[N];
	dot_prod_jazz(v1, v3, prod13);

	/*
	cout << "prod13 =" << endl;
	print_poly(prod13);
	*/
	//Expecting answer to be REDC(34)
	if((uint64_t(prod13[0]) << 32) % 8380417 != 98)
		throw runtime_error("test failed at " + to_string(__LINE__));

	for(int i = 1; i < N; ++i)
		if(prod13[i] != 0)
			throw runtime_error("test failed at " + to_string(__LINE__));

	/*
	// This works as expected too
	uint32_t prod11[N];
	dot_prod_jazz(v1, v1, prod11);
	cout << "prod11 =" << endl;
	print_poly(prod11);
	*/
}

void test_mat_vec_mult() {
	//[[0, 1, 2, ...
	//[l, l+1, l+2, ...
	uint32_t m[K * L * N] = { 0 };
	for(int k = 0; k < K; ++k) {
		for(int l = 0; l < L; ++l) {
			m[N * (k * L + l)] = k * L + l;
		}
	}

	/*
	cout << "m =";
	for(int k = 0; k < K * L; ++k) {
		cout << endl;
		print_poly(m + k * N);
	}

	cout << endl;
	*/

	// {1, x, x^2, ...};
	uint32_t v[L * N] = { 0 };
	for(int p = 0; p < L; ++p) {
		v[N * p + p] = 1;
	}

	uint32_t prod[K * N] = { 0 };
	mult_mat_vec_jazz(m, v, prod);

	for(int i = 0; i < K * N; ++i) {
		if(i % N == 0) {
			int k = i / N;
			unsigned int val = L * k;
			if((uint64_t(prod[i]) << 32) % 8380417 != val)
				throw runtime_error("test failed at " + to_string(__LINE__));
		} else {
			if(prod[i] != 0)
				throw runtime_error("test failed at " + to_string(__LINE__));
		}
	}

	// {0, 1, 2, 3, ...};
	uint32_t v2[L * N] = { 0 };
	for(int p = 0; p < L; ++p) {
		v2[N * p] = p;
	}
	mult_mat_vec_jazz(m, v2, prod);


	for(int i = 0; i < K * N; ++i) {
		if(i % N == 0) {
			int k = i / N;
			unsigned int val = 30 + 50 * k;
			if((uint64_t(prod[i]) << 32) % 8380417 != val)
				throw runtime_error("test failed at " + to_string(__LINE__));
		} else {
			if(prod[i] != 0)
				throw runtime_error("test failed at " + to_string(__LINE__));
		}
	}
}

void test_poly_mult() {
	uint32_t zeros[N] = { 0 };
	uint32_t fg[N];
	//0 * 1 = 0
	uint32_t f1[N] = { 0 };
	uint32_t g1[N] = { 1 };
	fft_poly_mult_jazz(f1, g1, fg);
	if(memcmp(zeros, fg, 4 * N) != 0)
		throw runtime_error("test failed at " + to_string(__LINE__));

	//1 * x = 0;
	uint32_t f2[N] = { 1 };
	uint32_t g2[N] = { 0, 1 };
	fft_poly_mult_jazz(f2, g2, fg);
	if(memcmp(zeros, fg, 4 * N) != 0)
		throw runtime_error("test failed at " + to_string(__LINE__));
	
	//2 * x^2 = 0
	uint32_t f3[N] = { 2 };
	uint32_t g3[N] = { 0, 0, 1 };
	fft_poly_mult_jazz(f3, g3, fg);
	if(memcmp(zeros, fg, 4 * N) != 0)
		throw runtime_error("test failed at " + to_string(__LINE__));

	//3 * x^3 = 0
	uint32_t f4[N] = { 3 };
	uint32_t g4[N] = { 0, 0, 0, 1 };
	fft_poly_mult_jazz(f4, g4, fg);
	if(memcmp(zeros, fg, 4 * N) != 0)
		throw runtime_error("test failed at " + to_string(__LINE__));
}

void test_poly_accumulate() {
	uint32_t zero[N] = { 0 };
	uint32_t f[N] = { 0 };
	poly_accumulate_jazz(f, f);
	poly_accumulate_jazz(f, f);
	poly_accumulate_jazz(f, f);
	poly_accumulate_jazz(f, f);
	if(memcmp(zero, f, 4 * N) != 0)
		throw runtime_error("test failed at " + to_string(__LINE__));
}

int main() {
	test_poly_accumulate();
	test_poly_mult();
	test_dot_prod();
	test_mat_vec_mult();
	return 0;
}
