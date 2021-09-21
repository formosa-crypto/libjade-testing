#include <iostream>
#include <random>
#include <fstream>
#include <tuple>
#include <string>

using std::cout;
using std::endl;
using std::vector;
using std::ofstream;
using std::make_pair;
using std::pair;
using std::string;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

constexpr const int poly_deg = 256;
const uint64_t dilithium_N = (1 << 23) - (1 << 13) + 1;
const uint64_t inv_of_2 = (dilithium_N + 1) / 2;

uint64_t montgomery(uint32_t x) {
	return (uint64_t(x) << 32) % dilithium_N;
}

uint64_t montgomery_REDC(uint64_t x) {
	uint64_t twoTo32Minus1 = 4294967295;
	uint64_t dilithium_N_inv_neg = 4236238847;
	uint64_t m = ((x & twoTo32Minus1) * dilithium_N_inv_neg) & twoTo32Minus1;

	uint64_t t = (x + m * dilithium_N) >> 32;

	if(t >= dilithium_N) {
		t -= dilithium_N;
	}
	return t;
}

// Precompute roots of unity
// Results are in Montgomery form
vector<uint32_t> precomputeMRoots() {
	vector<uint32_t> m_roots;
	uint32_t prim_root = 1753;
	uint64_t m_prim_root = montgomery(prim_root);
	
	uint32_t r = 1;
	uint64_t mr = montgomery(r);
	
	for(int i = 0; i < 512; ++i) {
		m_roots.push_back(uint32_t(mr));
		mr = montgomery_REDC(mr * m_prim_root);
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

int montgomery_sub(int mx, int my) {
	int diff = mx - my;
	if(diff < 0)
		diff += dilithium_N;
	return diff;
}

pair<uint32_t, uint32_t> compute_tfs(int butterfly_level, int butterfly_group, vector<uint32_t>& mroots) {
	//PRINT(butterfly_group);
	//butterfly_level = 0 .. 8
	//butterfly_group = 0 .. (1 << butterfly_level)
	int exp1 = bitreverse(2 * butterfly_group, butterfly_level + 1);
	//exp1 = bitreverse( 0 .. (1 << (butterfly_level + 1)) )

	//PRINT(exp1);

	int scaled_exp = exp1 << (8 - butterfly_level);

	//PRINT(scaled_exp);
	
	uint32_t tf1 = mroots.at(scaled_exp);

	//PRINT(tf1);

	/*
	if(butterfly_level == 2) {
		PRINT(exp1);
		PRINT(9 - butterfly_level);
		PRINT(tf1);
	}
	*/
	//cout << endl;

	uint32_t tf2 = dilithium_N - tf1;
	return make_pair(tf1, tf2);
}

int compute_itf(int butterfly_level, int butterfly_group, vector<uint32_t>& mroots) {
	int tf_exp = bitreverse(2 * butterfly_group, butterfly_level + 1)
			<< (8 - butterfly_level);
	//PRINT(tf_exp);
	int inv_tf_exp = tf_exp == 0 ? 0 : 512 - tf_exp;
	//PRINT(inv_tf_exp);
	//PRINT(mroots[inv_tf_exp]);
	return montgomery_REDC(mroots[inv_tf_exp] * montgomery(inv_of_2));
}

pair<vector<uint32_t>, vector<uint32_t>> compute_all_tfs()
{
	auto mRoots = precomputeMRoots();
	vector<uint32_t> tf1s;
	vector<uint32_t> tf2s;

	int butterfly_level = 7;

	//for(int butterfly_level = 0; butterfly_level < 8; ++butterfly_level) {
		for(int butterfly_group = 0; butterfly_group < 128; ++butterfly_group) {
			if(butterfly_group < (1 << butterfly_level)) {
				auto tfs = compute_tfs(butterfly_level, butterfly_group, mRoots);
				tf1s.push_back(tfs.first);
				tf2s.push_back(tfs.second);
			} else {
				tf1s.push_back(0);
				tf2s.push_back(0);
			}
		}
	//}

	return make_pair(tf1s, tf2s);
}

vector<uint32_t> compute_all_itfs() {
	auto mRoots = precomputeMRoots();
	vector<uint32_t> itfs;

	int butterfly_level = 7;

	//for(int butterfly_level = 0; butterfly_level < 8; ++butterfly_level) {
		for(int butterfly_group = 0; butterfly_group < 128; ++butterfly_group) {
			if(butterfly_group < (1 << butterfly_level)) {
				itfs.push_back(compute_itf(butterfly_level, butterfly_group, mRoots));
			} else {
				itfs.push_back(0);
			}
		}
	//}
	return itfs;
}

void printrow(vector<uint32_t> const& v, int r, ofstream& fout) {

	fout << '\t' << v[16 * r];
	for(int c = 1; c < 16; ++c) {
		fout << ", " << v[16 * r + c];
	}
}

void print_tfs(vector<uint32_t> const& factors, string name, ofstream& fout) {
	fout << "u32[128] " << name << " = {" << endl;
	printrow(factors, 0, fout);
	for(int i = 1; i < 128 / 16; ++i) {
		fout << "," << endl;
		if(i % 8 == 0)
			fout << endl;
		printrow(factors, i, fout);
	}
	fout << endl << "};" << endl;
}

int main() {
	ofstream fout("twiddle_factors.jazz");

	auto tfs = compute_all_tfs();
	auto itfs = compute_all_itfs();

	print_tfs(tfs.first, "first_twiddle_factors", fout);
	fout << endl << endl;
	print_tfs(tfs.second, "second_twiddle_factors", fout);
	fout << endl << endl;
	print_tfs(itfs, "ifft_twiddle_factors", fout);

	return 0;
}
