#include <iostream>
#include <random>
#include <cstring>
#include <array>
#include <string>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/ntt.h"
#include "../dilithium/ref/reduce.h"
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
	int32_t montgomery_REDC_jazz(int64_t x);
}

int64_t random_field_ele() {
	random_device rd;
	mt19937 gen(rd());
	int64_t bd = int64_t(1 << 31) * Q;
	uniform_int_distribution<int64_t> distrib(-bd, bd);
	return distrib(gen);
}

void test_redc() {
	for(int i = 0; i < 1000; ++i) {
		int64_t x = random_field_ele();
		int32_t rx_ref = montgomery_reduce(x);
		int32_t rx_jazz = montgomery_REDC_jazz(x);
		if(rx_ref != rx_jazz) {
			PRINT(x);
			PRINT(rx_ref);
			PRINT(rx_jazz);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}
	}
}

int main() {
	test_redc();
	return 0;
}
