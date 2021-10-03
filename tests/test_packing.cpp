#include <iostream>
#include <random>
#include <cstring>
#include <string>
#include <stdexcept>

extern "C" {
#include "../dilithium/ref/api.h"
#include "../dilithium/ref/params.h"
#include "../dilithium/ref/poly.h"
}

using std::cout;
using std::endl;
using std::vector;
using std::memcmp;
using std::runtime_error;
using std::to_string;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

extern "C" {
	void pack_t1_jazz(uint32_t p[N], uint8_t buf[POLYT1_PACKEDBYTES]);
	void polyz_unpack_jazz(int32_t p[N], uint8_t buf[POLYZ_PACKEDBYTES]);
	void polyeta_unpack_jazz(int32_t p[N], uint8_t buf[POLYETA_PACKEDBYTES]);
	void polyt0_unpack_jazz(int32_t p[N], uint8_t buf[POLYETA_PACKEDBYTES]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

uint32_t sample_t1_component() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  1023);
	return distrib(gen);
}

void test_pack_t1() {
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

	if(memcmp(buf_ref, buf_jazz, POLYT1_PACKEDBYTES) != 0) {
		throw runtime_error("test failed at " + to_string(__LINE__));
	}
}

void test_unpack_z() {
	poly p_ref;
	int32_t p_jazz[N];
	
	uint8_t buf[POLYZ_PACKEDBYTES];
	for(int i = 0; i < POLYZ_PACKEDBYTES; ++i) {
		buf[i] = sampleByte();
	}

	polyz_unpack(&p_ref, buf); 
	polyz_unpack_jazz(p_jazz, buf);


	for(int i = 0; i < N; ++i) {
		if(p_ref.coeffs[i] != p_jazz[i]) {
			PRINT(i);
			PRINT(p_ref.coeffs[i]);
			PRINT(p_jazz[i]);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}
	}
}

void test_unpack_eta() {
	poly p_ref;
	int32_t p_jazz[N];
	
	uint8_t buf[POLYETA_PACKEDBYTES];
	for(int i = 0; i < POLYETA_PACKEDBYTES; ++i) {
		buf[i] = sampleByte();
	}

	polyeta_unpack(&p_ref, buf); 
	polyeta_unpack_jazz(p_jazz, buf);

	for(int i = 0; i < N; ++i) {
		if(p_ref.coeffs[i] != p_jazz[i]) {
			PRINT(i);
			PRINT(p_ref.coeffs[i]);
			PRINT(p_jazz[i]);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}
	}
}

void test_unpack_t0() {
	poly p_ref;
	int32_t p_jazz[N];
	
	uint8_t buf[POLYT0_PACKEDBYTES];
	for(int i = 0; i < POLYT0_PACKEDBYTES; ++i) {
		buf[i] = sampleByte();
	}

	polyt0_unpack(&p_ref, buf); 
	polyt0_unpack_jazz(p_jazz, buf);

	for(int i = 0; i < N; ++i) {
		if(p_ref.coeffs[i] != p_jazz[i]) {
			PRINT(i);
			PRINT(p_ref.coeffs[i]);
			PRINT(p_jazz[i]);
			throw runtime_error("test failed at " + to_string(__LINE__));
		}
	}
}

int main() {
	test_pack_t1();
	test_unpack_z();
	test_unpack_eta();
	test_unpack_t0();
	return 0;
}
