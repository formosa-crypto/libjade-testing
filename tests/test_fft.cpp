#include <iostream>
#include <random>
#include <cstring>
#include <array>
#include <string>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/ntt.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;
using std::runtime_error;
using std::to_string;
using std::array;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void fft_jazz(int32_t f[N]);
	void ifft_to_mont_jazz(int32_t f[N]);
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

array<int32_t, N> random_poly(bool want_neg = false) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distrib(want_neg ? (1 - Q) : 0, Q - 1);
	array<int32_t, N> arr;
	for(int i = 0; i < N; ++i)
		arr[i] = distrib(gen);
	return arr;
}

void test_fft() {
	auto arr = random_poly();
	int32_t f[N];
	int32_t g[N];
	for(int i = 0; i < N; ++i) {
		f[i] = arr[i];
		g[i] = arr[i];
	}
	fft_jazz(f);
	ntt(g);

	if(memcmp(f, g, 4 * N) != 0) {
		cout << "f =" << endl;
		print_poly(arr.data());
		cout << endl << "fft_f =" << endl;
		print_poly(f);
		cout << endl << "fft_g =" << endl;
		print_poly(g);
		throw runtime_error("test failed at " + to_string(__LINE__));
	}
}

void test_ifft_to_mont() {
	auto arr = random_poly(true);
	int32_t f[N];
	int32_t g[N];
	for(int i = 0; i < N; ++i) {
		f[i] = arr[i];
		g[i] = arr[i];
	}
	ifft_to_mont_jazz(f);
	invntt_tomont(g);

	if(memcmp(f, g, 4 * N) != 0) {
		cout << "f =" << endl;
		print_poly(arr.data());
		cout << endl << "ifft_f =" << endl;
		print_poly(f);
		cout << endl << "ifft_g =" << endl;
		print_poly(g);
		throw runtime_error("test failed at " + to_string(__LINE__));
	}
}

int main() {
	test_fft();
	test_ifft_to_mont();
	return 0;
}
