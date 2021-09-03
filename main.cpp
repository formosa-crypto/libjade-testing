#include <iostream>
#include <random>

using std::cout;
using std::endl;
using std::vector;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

constexpr const int poly_deg = 256;

extern "C" {
	uint64_t montgomery_REDC(uint64_t mx);
	uint64_t montgomery_mult(uint64_t mx, uint64_t my);
	//64 bits int only for pointer params...? Why?
	//untested fft; this probably ain't gonna work...
	void fft(uint64_t f[poly_deg], uint64_t precomputed_mroots[256]);
}

const uint64_t dilithium_N = (1 << 23) - (1 << 13) + 1;

uint64_t montgomery(uint32_t x) {
	return (uint64_t(x) << 32) % dilithium_N;
}

uint32_t sampleInt() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  dilithium_N - 1);
	return distrib(gen);
}

// Precompute roots of unity
// Results are in Montgomery form
vector<uint64_t> precomputeMRoots() {
	vector<uint64_t> m_roots;
	uint32_t prim_root = 1753;
	uint64_t m_prim_root = montgomery(prim_root);
	
	uint32_t r = 1;
	uint64_t mr = montgomery(r);
	
	for(int i = 0; i < 256; ++i) {
		m_roots.push_back(mr);
		mr = montgomery_REDC(mr * m_prim_root);
	}
	return m_roots;
}

void samplePoly(uint32_t f[poly_deg]) {
	for(int i = 0; i < poly_deg; ++i)
		f[i] = sampleInt();
}

void montgomery_of_poly(uint32_t f[poly_deg], uint64_t mf[poly_deg]) {
	for(int i = 0; i < poly_deg; ++i)
		mf[i] = montgomery(f[i]);
}

template<typename T> void zero_poly(T f[poly_deg]) {
	for(int i = 0; i < poly_deg; ++i)
		f[i] = 0;
}

template<typename T> void print_poly(T f[poly_deg]) {
	for(int i = 0; i < 16; ++i) {
		cout << f[16 * i];
		for(int j = 1; j < 16; ++j) {
			cout << ' ' << f[16 * i + j];
		}
		cout << endl;
	}
}

void multiply_polys(uint32_t f[poly_deg], uint32_t g[poly_deg], uint32_t product[poly_deg]) {
	zero_poly(product);
	for(int i = 0; i < poly_deg; ++i) {
		for(int fdeg = 0; fdeg < poly_deg; ++fdeg) {
			vector<int> gdegs { i - fdeg, i - fdeg + poly_deg };
			for(int gdeg : gdegs) {
				if(gdeg >= 0 && gdeg < poly_deg) {
					uint64_t prod = uint64_t(f[fdeg]) * g[gdeg];
					prod %= dilithium_N;
					product[i] += prod;
				}
			}
		}
	}
}

void test_montgomery() {
	uint64_t z = 823719;
	PRINT(z);
	uint64_t mZ = montgomery(z);
	PRINT(montgomery_REDC(mZ));

	uint32_t x = 374824;
	uint32_t y = 8047392;
	uint64_t mX = montgomery(x);
	uint64_t mY = montgomery(y);
	uint64_t mProd = montgomery_REDC(mX * mY);
	uint64_t prod = montgomery_REDC(mProd);
	PRINT(prod);

	uint64_t answer = (uint64_t(x) * y) % dilithium_N;
	PRINT(answer);
}

void test_poly_product() {
	uint32_t f[poly_deg];
	uint32_t g[poly_deg];
	zero_poly(f);
	zero_poly(g);
	f[0] = 1;
	f[1] = 1;
	g[0] = 1;
	g[1] = 1;
	
	uint32_t prod[poly_deg];
	multiply_polys(f, g, prod);

	cout << "Expect: 1 2 1 0 0" << endl;
	cout << prod[0];
	for(int i = 1; i < 5; ++i) {
		cout << ' ' << prod[i];
	}
	cout << endl;

	g[0] = 2;
	multiply_polys(f, g, prod);
	cout << "Expect: 2 3 1 0 0" << endl;
	cout << prod[0];
	for(int i = 1; i < 5; ++i) {
		cout << ' ' << prod[i];
	}
	cout << endl;

	f[2] = 3;
	multiply_polys(f, g, prod);
	cout << "Expect: 2 3 7 3 0" << endl;
	cout << prod[0];
	for(int i = 1; i < 5; ++i) {
		cout << ' ' << prod[i];
	}
	cout << endl;
}

void test_fft() {
	auto mroots = precomputeMRoots();
	uint64_t f[poly_deg];


	cout << "fft of 1:" << endl;
	zero_poly(f);
	f[0] = montgomery(1);
	fft(f, mroots.data());
	print_poly(f);

	cout << "fft of x:" << endl;
	zero_poly(f);
	f[1] = montgomery(1);
	fft(f, mroots.data());
	print_poly(f);
	
	cout << "fft of 1+x:" << endl;
	zero_poly(f);
	f[0] = montgomery(1);
	f[1] = montgomery(1);
	fft(f, mroots.data());
	print_poly(f);
}

int main() {
	cout << "testing montgomery..." << endl;
	test_montgomery();
	cout << "testing poly product..." << endl;
	test_poly_product();
	cout << "testing FFT..." << endl;
	test_fft();
	return 0;
}
