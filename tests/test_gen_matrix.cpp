#include <iostream>
#include <random>
#include <cstring>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void gen_noises_entry_test();
	void gen_noises_test(uint32_t s1[L * N]);
}

/*
uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}
*/

int main() {
	uint32_t s1[L * N] = { 0 };
	gen_noises_test(s1);
	for(int i = 0; i < 20; ++i) {
		PRINT(s1[i]);
	}

	return 0;
}
