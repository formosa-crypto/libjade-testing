#include <iostream>
#include <random>
#include <cstring>
#include <array>
#include <string>
#include <stdexcept>

extern "C"
{
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/poly.h"
#include "../dilithium/ref/polyvec.h"
}

using std::array;
using std::cout;
using std::endl;
using std::memcmp;
using std::runtime_error;
using std::to_string;
using std::vector;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C"
{
	void dilithium5_mult_mat_vec_jazz(int32_t m[K * L * N], int32_t v[L * N], int32_t prod[K * N]);
	// void dilithium_dot_prod_jazz(int32_t v1[L * N], int32_t v2[L * N], int32_t prod[N]);
	// void dilithium_poly_accumulate_jazz(int32_t f[N], int32_t s[N]);
	// void dilithium_probe_mult_mat_vec_jazz(int32_t m[K * L * N], int32_t v[L * N], int32_t prod[K * N]);
	void poly_pointwise_montgomery_jazz(int32_t f[N], int32_t g[N], int32_t h[N]);
}

int32_t samplei30()
{
	const int32_t bound = 1 << 30;
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(-bound, bound);
	return distrib(gen);
}

template <typename T>
void print_poly(T f[N])
{
	for (int i = 0; i < 16; ++i)
	{
		cout << f[16 * i];
		for (int j = 1; j < 16; ++j)
		{
			cout << ' ' << f[16 * i + j];
		}
		cout << endl;
	}
}

// void test_dot_prod()
// {
// 	//[0, 1, 2, ...]
// 	int32_t v1[L * N] = {0};
// 	for (int i = 0; i < L; ++i)
// 	{
// 		v1[i * N] = i;
// 	}
// 	/*
// 	cout << "v1 =";
// 	for(int i = 0; i < L; ++i) {
// 		cout << endl;
// 		print_poly(v1 + i * N);
// 	}
// 	*/

// 	//[1, x, x^2, ...]
// 	int32_t v2[L * N] = {0};
// 	for (int i = 0; i < L; ++i)
// 	{
// 		v2[i * N + i] = 1;
// 	}
// 	/*
// 	cout << "v2 =";
// 	for(int i = 0; i < L; ++i) {
// 		cout << endl;
// 		print_poly(v2 + i * N);
// 	}
// 	*/

// 	// expecting all 0s
// 	int32_t prod12[N];
// 	dilithium_dot_prod_jazz(v1, v2, prod12);

// 	/*
// 	cout << "prod12 =" << endl;
// 	print_poly(prod12);
// 	*/

// 	for (int i = 0; i < N; ++i)
// 		if (prod12[i] != 0)
// 			throw runtime_error("test failed at " + to_string(__LINE__));

// 	//[1, 2, 4, 8, ...]
// 	int32_t v3[L * N] = {0};
// 	for (int i = 0; i < L; ++i)
// 	{
// 		v3[i * N] = (1 << i);
// 	}

// 	cout << "v3 =";
// 	for (int i = 0; i < L; ++i)
// 	{
// 		cout << endl;
// 		print_poly(v3 + i * N);
// 	}

// 	int32_t prod13[N];
// 	dilithium_dot_prod_jazz(v1, v3, prod13);

// 	cout << "prod13 =" << endl;
// 	print_poly(prod13);

// 	// Expecting answer to be REDC(34)
// 	if ((uint64_t(prod13[0]) << 32) % 8380417 != 98)
// 	{
// 		PRINT(uint64_t(prod13[0]) << 32);
// 		throw runtime_error("test failed at " + to_string(__LINE__));
// 	}

// 	for (int i = 1; i < N; ++i)
// 		if (prod13[i] != 0)
// 			throw runtime_error("test failed at " + to_string(__LINE__));

// 	/*
// 	// This works as expected too
// 	uint32_t prod11[N];
// 	dilithium_dot_prod_jazz(v1, v1, prod11);
// 	cout << "prod11 =" << endl;
// 	print_poly(prod11);
// 	*/
// }

// void test_mat_vec_mult()
// {
// 	//[[0, 1, 2, ...
// 	//[l, l+1, l+2, ...
// 	int32_t m[K * L * N] = {0};
// 	for (int k = 0; k < K; ++k)
// 	{
// 		for (int l = 0; l < L; ++l)
// 		{
// 			m[N * (k * L + l)] = k * L + l;
// 		}
// 	}

// 	// void polyvec_matrix_pointwise_montgomery(polyveck *t, const polyvecl mat[K], const polyvecl *v);

// 	/*
// 	cout << "m =";
// 	for(int k = 0; k < K * L; ++k) {
// 		cout << endl;
// 		print_poly(m + k * N);
// 	}

// 	cout << endl;
// 	*/

// 	// {1, x, x^2, ...};
// 	int32_t v[L * N] = {0};
// 	for (int p = 0; p < L; ++p)
// 	{
// 		v[N * p + p] = 1;
// 	}

// 	int32_t prod[K * N] = {0};
// 	dilithium_mult_mat_vec_jazz(m, v, prod);

// 	for (int i = 0; i < K * N; ++i)
// 	{
// 		if (i % N == 0)
// 		{
// 			int k = i / N;
// 			unsigned int val = L * k;
// 			if ((uint64_t(prod[i]) << 32) % 8380417 != val)
// 				throw runtime_error("test failed at " + to_string(__LINE__));
// 		}
// 		else
// 		{
// 			if (prod[i] != 0)
// 				throw runtime_error("test failed at " + to_string(__LINE__));
// 		}
// 	}

// 	// {0, 1, 2, 3, ...};
// 	int32_t v2[L * N] = {0};
// 	for (int p = 0; p < L; ++p)
// 	{
// 		v2[N * p] = p;
// 	}
// 	dilithium_mult_mat_vec_jazz(m, v2, prod);

// 	for (int i = 0; i < K * N; ++i)
// 	{
// 		if (i % N == 0)
// 		{
// 			int k = i / N;
// 			unsigned int val = 30 + 50 * k;
// 			if ((uint64_t(prod[i]) << 32) % 8380417 != val)
// 				throw runtime_error("test failed at " + to_string(__LINE__));
// 		}
// 		else
// 		{
// 			if (prod[i] != 0)
// 				throw runtime_error("test failed at " + to_string(__LINE__));
// 		}
// 	}
// }

void test_poly_pointwise()
{
	for (int test_idx = 0; test_idx < 10000; test_idx++)
	{
		poly f = {0};
		poly g = {0};
		poly h_ref = {0};
		poly h_jazz = {0};

		for (size_t i = 0; i < N; i++)
		{
			f.coeffs[i] = samplei30();
			g.coeffs[i] = samplei30();
		}

		poly_pointwise_montgomery(&h_ref, &f, &g);
		poly_pointwise_montgomery_jazz(f.coeffs, g.coeffs, h_jazz.coeffs);

		for (size_t i = 0; i < N; i++)
		{
			if (h_jazz.coeffs[i] != h_ref.coeffs[i])
			{
				std::cout << "test_idx: " << test_idx << ", i: " << i << "\n";
				std::cout << "h_ref.coeffs:\n";
				print_poly(h_ref.coeffs);
				std::cout << "h_jazz.coeffs:\n";
				print_poly(h_jazz.coeffs);
				throw runtime_error("test failed at line " + to_string(__LINE__));
			}
		}
	}
}

void test_matrix_pointwise_montgomery()
{
	for (int test_idx = 0; test_idx < 1000; test_idx++)
	{
		uint8_t rho[SEEDBYTES] = {0};
		uint8_t rhoprime[CRHBYTES] = {0};
		polyvecl mat[K];
		polyvecl y = {0};
		polyvecl y_ref = {0};
		polyvecl y_jazz = {0};
		polyveck w_ref = {0};
		polyveck w_jazz = {0};

		// Generate seeds
		for (int16_t i = 0; i < 4; i++)
		{
			rho[i] = rhoprime[i] = test_idx >> (8 * i);
		}
		// Domain separate seeds
		rho[4] = 0x0A;
		rhoprime[4] = 0xA0;

		// Expand values
		polyvec_matrix_expand(mat, rho);
		polyvecl_uniform_gamma1(&y, rhoprime, 0);

		polyvecl_ntt(&y);
		y_ref = y;
		y_jazz = y;
		polyvec_matrix_pointwise_montgomery(&w_ref, mat, &y_ref);
		dilithium5_mult_mat_vec_jazz(mat->vec->coeffs, y_jazz.vec->coeffs, w_jazz.vec->coeffs);

		for (size_t i = 0; i < K; i++)
		{
			for (size_t j = 0; j < N; j++)
			{
				if (w_jazz.vec[i].coeffs[j] != w_ref.vec[i].coeffs[j])
				{
					PRINT(test_idx);
					PRINT(i);
					PRINT(j);
					std::cout << "w_ref.coeffs:\n";
					print_poly(w_ref.vec[i].coeffs);
					std::cout << "w_jazz.coeffs:\n";
					print_poly(w_jazz.vec[i].coeffs);
					throw runtime_error("test failed at line " + to_string(__LINE__));
				}
			}
		}
	}
}

int main()
{
	test_poly_pointwise();
	test_matrix_pointwise_montgomery();
	// test_dot_prod();
	// test_mat_vec_mult();
	return 0;
}
