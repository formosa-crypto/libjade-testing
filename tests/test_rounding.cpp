#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <cstring>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/rounding.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;
using std::runtime_error;
using std::to_string;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void decompose_jazz(int32_t a, int32_t* a0, int32_t* a1);
	uint32_t make_hint_jazz(int32_t a0, int32_t a1);
}

uint8_t sample() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  Q - 1);
	return distrib(gen);
}

int main() {
	for(int i = 0; i < 10000; ++i) {
		int32_t a, a0_ref, a1_ref, a0_jazz, a1_jazz;

		a = sample();
		decompose_jazz(a, &a0_jazz, &a1_jazz);
		a1_ref = decompose(&a0_ref, a);

		if(a0_ref != a0_jazz) {
			PRINT(a);
			PRINT(a0_ref);
			PRINT(a0_jazz);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}

		if(a1_ref != a1_jazz) {
			PRINT(a);
			PRINT(a1_ref);
			PRINT(a1_jazz);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}

		unsigned int hint_ref = make_hint(a0_ref, a1_ref);
		uint32_t hint_jazz = make_hint_jazz(a0_jazz, a1_jazz);

		if(hint_ref != hint_jazz) {
			PRINT(a0_ref);
			PRINT(a1_ref);
			PRINT(hint_ref);
			PRINT(hint_jazz);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}
	}
}
