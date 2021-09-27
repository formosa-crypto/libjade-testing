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
	void shake256_32_320_32_jazz(uint8_t rho[32], uint8_t t1[320], uint8_t out[32]);
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
	uint8_t rho[32] = { 0 };
	uint8_t t1[320] = { 0 };
	uint8_t out[32];

	shake256_32_320_32_jazz(rho, t1, out);

	PRINT(int(out[0]));
	PRINT(int(out[1]));
	PRINT(int(out[2]));
	PRINT(int(out[3]));

	return 0;
}
