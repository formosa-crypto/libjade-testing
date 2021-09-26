#include <iostream>
#include <random>
#include <cstring>
#include <array>
#include <string>
#include <stdexcept>

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
using std::to_string;
using std::runtime_error;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void mult_mat_vec_jazz(uint32_t m[K * L * N], uint32_t v[L * N], uint32_t prod[K * N]);
}

int main() {

	return 0;
}
