#include <iostream>
#include <random>
#include <cstring>
#include <array>

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

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void gen_matrix_jazz(uint8_t rho[SEEDBYTES], uint32_t p[K * L * N]);
}

/*
template<typename T> void print_poly(T f[N]) {
	for(int i = 0; i < 16; ++i) {
		cout << f[16 * i];
		for(int j = 1; j < 16; ++j) {
			cout << ' ' << f[16 * i + j];
		}
		cout << endl;
	}
}

typedef struct {
  poly vec[L];
} polyvecl;

typedef struct {
  int32_t coeffs[N];
} poly;
*/

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

int main() {
	uint8_t rho[SEEDBYTES] = { 0 };

	// ref impl.
	polyvecl mat[K];
	pqcrystals_dilithium3_ref_polyvec_matrix_expand(mat, rho);

	//Jasmin
	uint32_t mat_jazz[K * L * N];
	gen_matrix_jazz(rho, mat_jazz);

	//comp.
	auto arr = mat_to_array(mat);
	
	PRINT(memcmp(arr.data(), mat_jazz, 4 * K * L * N));

	return 0;
}
