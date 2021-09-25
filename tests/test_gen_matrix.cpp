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
	void gen_matrix_jazz(uint8_t rho[SEEDBYTES], uint32_t p[K * L * N]);
	void gen_matrix_entry_jazz(uint32_t out[N]);
}

array<uint32_t, N> poly_to_array(poly p) {
	array<uint32_t, N> arr;
	for(int i = 0; i < N; ++i) {
		arr[i] = p.coeffs[i];
	}
	return arr;
}

void test_gen_entry() {
	uint8_t rho[SEEDBYTES] = { 0 };
	int i = 3;
	int j = 4;
	poly p;
	poly_uniform(&p, rho, (i << 8) + j);

	uint32_t poly_jazz[N];
	gen_matrix_entry_jazz(poly_jazz);

	auto poly_ref_arr = poly_to_array(p);
	auto poly_ref = poly_ref_arr.data();

	for(int k = 0; k < N; ++k) {
		if(poly_ref[k] != poly_jazz[k]) {
			PRINT(k);
			PRINT(poly_ref[k]);
			PRINT(poly_jazz[k]);
			return;
		}
	}
	
	if(memcmp(poly_ref, poly_jazz, 4 * N) != 0)
		throw runtime_error("test failed at " + to_string(__LINE__));
}

array<uint32_t, K * L * N> mat_to_array(polyvecl mat[K]) {
	array<uint32_t, K * L * N> arr;
	for(int k = 0; k < K; ++k) {
		for(int v = 0; v < L; ++v) {
			for(int i = 0; i < N; ++i) {
				arr[k * L * N + v * N + i] = mat[k].vec[v].coeffs[i];
			}
		}
	}
	return arr;
}

void test_gen_matrix() {
	uint8_t rho[SEEDBYTES] = { 0 };

	// ref impl.
	polyvecl mat[K];
	pqcrystals_dilithium3_ref_polyvec_matrix_expand(mat, rho);

	//Jasmin
	uint32_t mat_jazz[K * L * N];
	gen_matrix_jazz(rho, mat_jazz);

	//memcmp test
	auto arr = mat_to_array(mat);
	if(memcmp(arr.data(), mat_jazz, 4 * K * L * N) != 0)
		throw runtime_error("test failed at " + to_string(__LINE__));
	//PRINT(memcmp(arr.data(), mat_jazz, 4 * K * L * N));
}

int main() {
	test_gen_entry();
	test_gen_matrix();

	return 0;
}
