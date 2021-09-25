#include <iostream>
#include <random>
#include <cstring>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/poly.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void pack_t1_jazz(uint32_t p[N], uint8_t buf[POLYT1_PACKEDBYTES]);
}

uint32_t sample_t1_component() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  1023);
	return distrib(gen);
}

int main() {
	poly p;
	uint32_t uint_p[N];
	for(int i = 0; i < N; ++i) {
		uint32_t val = sample_t1_component(); 
		p.coeffs[i] = val;
		uint_p[i] = val;
	}

	uint8_t buf_ref[POLYT1_PACKEDBYTES];

	for(int i = 0; i < K; ++i) {
		pqcrystals_dilithium3_ref_polyt1_pack(buf_ref, &p);
	}

	uint8_t buf_jazz[POLYT1_PACKEDBYTES];
	pack_t1_jazz(uint_p, buf_jazz);

	PRINT(memcmp(buf_ref, buf_jazz, POLYT1_PACKEDBYTES));
	/*
	PRINT(int(buf_ref[0]));
	PRINT(int(buf_ref[1]));
	PRINT(int(buf_ref[2]));
	*/
	
	return 0;
}
