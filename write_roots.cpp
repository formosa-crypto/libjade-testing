#include <iostream>
#include <random>
#include <fstream>

using std::cout;
using std::endl;
using std::vector;
using std::ofstream;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

constexpr const int poly_deg = 256;
const uint64_t dilithium_N = (1 << 23) - (1 << 13) + 1;

uint64_t montgomery(uint32_t x) {
	return (uint64_t(x) << 32) % dilithium_N;
}

uint64_t montgomery_REDC(uint64_t x) {
	uint64_t twoTo32Minus1 = 4294967295;
	uint64_t dilithium_modulo_inv_neg = 4236238847;
	uint64_t m = ((x & twoTo32Minus1) * dilithium_modulo_inv_neg) & twoTo32Minus1;

	uint64_t t = (x + m * dilithium_N) >> 32;

	if(t >= dilithium_N) {
		t -= dilithium_N;
	}
	return t;

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

void printrow(vector<uint64_t> const& v, int r, ofstream& fout) {

	fout << '\t' << v[16 * r];
	for(int c = 1; c < 16; ++c) {
		fout << ", " << v[16 * r + c];
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


int main() {
	ofstream fout("roots_of_unity.jazz");
	//test_montgomery();
	fout << "#ifndef ROOTS_OF_UNITY" << endl;
	fout << "#define ROOTS_OF_UNITY" << endl;
	fout << "u64[256] roots_of_unity = {" << endl;

	auto mRoots = precomputeMRoots();

	printrow(mRoots, 0, fout);
	for(int i = 1; i < 16; ++i) {
		fout << "," << endl;
		printrow(mRoots, i, fout);
	}

	fout << endl << "};" << endl;
	fout << "#endif" << endl;
	return 0;
}
