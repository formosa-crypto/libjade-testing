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
	//[1, x, x^2, ...]
	uint32_t v2[L * N] = { 0 };
	for(int i = 0; i < L; ++i) {
		v2[i * N + i] = 1;
	}

	uint32_t prod[N];
	dot_prod_jazz(v1, v2, prod);
	print_poly(prod);
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

	// {1, x, x^2, ...};
	uint32_t v[L * N] = { 0 };
	for(int p = 0; p < L; ++p) {
		v[N * p + p] = p;
	}

	uint32_t prod[K * N] = { 0 };
	mult_mat_vec_jazz(m, v, prod);

	cout << "prod =";

	for(int k = 0; k < K; ++k) {
		cout << endl;
		print_poly(prod + k * N);
	}
}

int main() {
	test_dot_prod();
	//test_mat_vec_mult();
	return 0;
}
