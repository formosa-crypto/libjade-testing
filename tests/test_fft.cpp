#include <iostream>
#include <random>
#include <cstring>
#include <string>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;
using std::runtime_error;
using std::to_string;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void fft_jazz(uint32_t f[N]);
	void ifft_jazz(uint32_t f[N]);
	uint32_t montgomery_REDC_jazz(uint64_t mx);
	void fft_poly_mult_jazz(uint32_t f[N], uint32_t g[N], uint32_t fg[N]);
}

uint32_t montgomery(uint32_t x) {
	return (uint64_t(x) << 32) % Q;
}

// Precompute roots of unity
// Results are in Montgomery form
vector<uint64_t> precomputeMRoots() {
	vector<uint64_t> m_roots;
	uint32_t prim_root = 1753;
	uint64_t m_prim_root = montgomery(prim_root);
	
	uint64_t mr = montgomery(1);
	
	for(int i = 0; i < 512; ++i) {
		m_roots.push_back(mr);
		mr = montgomery_REDC_jazz(mr * m_prim_root);
	}
	return m_roots;
}

int bitreverse(int x, int numbits) {
	int result = 0;
	for(int i = 0; i < numbits; ++i) {
		int bit = (x >> i) & 1;
		result |= bit << (numbits - 1 - i);
	}
	return result;
}

template<typename T> void zero_poly(T f[N]) {
	for(int i = 0; i < N; ++i)
		f[i] = 0;
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

/* Not Montgomery */
void multiply_polys(uint32_t f[N], uint32_t g[N], uint32_t product[N]) {
	zero_poly(product);
	for(int i = 0; i < N; ++i) {
		for(int fdeg = 0; fdeg < N; ++fdeg) {
			vector<int> gdegs { i - fdeg, i - fdeg + N };
			for(int gdeg : gdegs) {
				if(gdeg >= 0 && gdeg < N) {
					uint64_t prod = uint64_t(f[fdeg]) * g[gdeg];
					prod %= N;
					product[i] += prod;
				}
			}
		}
	}
}

void test_montgomery() {
	uint64_t z = 823719;
	uint64_t mZ = montgomery(z);

	if(z != montgomery_REDC_jazz(mZ))
		throw runtime_error("test failed at " + to_string(__LINE__));

	uint32_t x = 374824;
	uint32_t y = 8047392;
	uint64_t mX = montgomery(x);
	uint64_t mY = montgomery(y);
	uint64_t mProd = montgomery_REDC_jazz(mX * mY);
	uint64_t prod = montgomery_REDC_jazz(mProd);
	uint64_t answer = (uint64_t(x) * y) % Q;

	if(prod != answer)
		throw runtime_error("test failed at " + to_string(__LINE__));
}

void test_poly_product() {
	uint32_t f[N] = { 15 };
	uint32_t g[N];
	zero_poly(f);

	if(vector<uint32_t>(f, f + N) != vector<uint32_t>(N, 0))
		throw runtime_error("test failed at " + to_string(__LINE__));

	zero_poly(g);
	f[0] = 1;
	f[1] = 1;
	g[0] = 1;
	g[1] = 1;
	
	uint32_t prod[N];
	multiply_polys(f, g, prod);

	if(vector<uint32_t>(prod, prod + 5) != vector<uint32_t> {1, 2, 1, 0, 0})
		throw runtime_error("test failed at " + to_string(__LINE__));

	g[0] = 2;
	multiply_polys(f, g, prod);
	if(vector<uint32_t>(prod, prod + 5) != vector<uint32_t> {2, 3, 1, 0, 0})
		throw runtime_error("test failed at " + to_string(__LINE__));

	f[2] = 3;
	multiply_polys(f, g, prod);
	if(vector<uint32_t>(prod, prod + 5) != vector<uint32_t> {2, 3, 7, 3, 0})
		throw runtime_error("test failed at " + to_string(__LINE__));
}

void test_ifft() {
	auto mroots = precomputeMRoots();
	uint32_t mf[N], f[N];

	zero_poly(f);
	f[0] = 1;
	f[1] = 31;
	f[2] = 5;
	f[3] = 200;

	for(int i = 0; i < N; ++i)
		mf[i] = montgomery(f[i]);

	fft_jazz(mf);
	ifft_jazz(mf);

	vector<uint32_t> answer {
		montgomery(1),
		montgomery(31),
		montgomery(5),
		montgomery(200),
		0,
		0,
		0
	};

	if(vector<uint32_t>(mf, mf + 7) != answer)
		throw runtime_error("test failed at " + to_string(__LINE__));

	uint32_t g[N], mg[N];

	zero_poly(g);
	g[0] = 82;
	g[1] = 50;
	g[2] = 5;
	g[3] = 591;
	for(int i = 0; i < N; ++i)
		mg[i] = montgomery(g[i]);

	fft_jazz(mf);
	fft_jazz(mg);

	uint32_t mfg[N];
	fft_poly_mult_jazz(mf, mg, mfg);

	PRINT(mf[0]);
	PRINT(mg[0]);
	PRINT(mf[1]);
	PRINT(mg[1]);
	PRINT(montgomery_REDC_jazz(uint64_t(mf[0]) * mg[0]));
	PRINT(mfg[0]);
	PRINT(montgomery_REDC_jazz(uint64_t(mf[1]) * mg[1]));
	PRINT(mfg[1]);

	ifft_jazz(mfg);

	uint32_t fg[N];
	multiply_polys(f, g, fg);

	for(int i = 0; i < N; ++i)
		if(montgomery(fg[i]) != mfg[i])
			throw runtime_error("test failed at " + to_string(__LINE__));

}

void test_fft() {
	uint32_t f[N];
	zero_poly(f);
	f[0] = montgomery(1);
	fft_jazz(f);

	if(vector<uint32_t>(f, f + N) != vector<uint32_t>(N, montgomery(1)))
		throw runtime_error("test failed at " + to_string(__LINE__));

	uint32_t g[N];
	zero_poly(g);
	g[1] = montgomery(1);
	fft_jazz(g);

	auto mRoots = precomputeMRoots();
	for(int i = 0; i < 10; ++i) {
		if(mRoots[bitreverse(i, 9)] != g[i])
			throw runtime_error("test failed at " + to_string(__LINE__));
	}
	
	uint32_t h[N];

	zero_poly(h);
	h[0] = montgomery(1);
	h[1] = montgomery(1);

	fft_jazz(h);

	for(int i = 0; i < 10; ++i) {
		if((f[i] + g[i]) % Q != h[i])
			throw runtime_error("test failed at " + to_string(__LINE__));
	}
}

void test_roots_of_unity() {
	auto v = precomputeMRoots();
}

int main() {
	test_montgomery();
	test_poly_product();
	test_roots_of_unity();
	test_fft();
	//test_ifft();
	return 0;
}
