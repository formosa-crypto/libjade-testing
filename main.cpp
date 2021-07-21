#include <iostream>

using std::cout;
using std::endl;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	uint64_t montgomery(uint32_t x);
	uint32_t montgomery_REDC(uint64_t mx);
}

const uint64_t dilithium_N = (1 << 23) - (1 << 13) + 1;

int main() {
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

	return 0;
}
