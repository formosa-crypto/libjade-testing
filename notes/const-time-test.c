#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define DUDECT_VISIBLITY_STATIC
#define DUDECT_IMPLEMENTATION
#include "dudect.h"

#include "dilithium/ref/api.h"
#include "dilithium/ref/params.h"

uint8_t do_one_computation(uint8_t *data) {
	uint8_t result[pqcrystals_dilithium3_BYTES];
	size_t siglen;
	uint8_t* sk = data;
	pqcrystals_dilithium3_ref_signature(result,
			&siglen,
			data + pqcrystals_dilithium3_SECRETKEYBYTES,
			1000,
			data);

	return result[0] ^ result[1];
}

void prepare_inputs(dudect_config_t *c, uint8_t *input_data, uint8_t *classes) {

	uint8_t pk0[pqcrystals_dilithium3_PUBLICKEYBYTES];
	uint8_t sk0[pqcrystals_dilithium3_SECRETKEYBYTES];
	pqcrystals_dilithium3_ref_keypair(pk0, sk0);

	uint8_t pk1[pqcrystals_dilithium3_PUBLICKEYBYTES];
	uint8_t sk1[pqcrystals_dilithium3_SECRETKEYBYTES];
	pqcrystals_dilithium3_ref_keypair(pk1, sk1);

	for (size_t i = 0; i < c->number_measurements; i++) {
		char* loc = input_data + i * c->chunk_size;
		bool c = randombit();
		classes[i] = c;
		//key
		memcpy(loc, c ? sk0 : sk1, pqcrystals_dilithium3_SECRETKEYBYTES);
		//random message
		randombytes(loc + pqcrystals_dilithium3_SECRETKEYBYTES, 1000);
	}
}

int main() {
	dudect_config_t config = {
		.chunk_size = pqcrystals_dilithium3_SECRETKEYBYTES + 1000,
		.number_measurements = 1e6,
	};
	dudect_ctx_t ctx;
	dudect_init(&ctx, &config);

	dudect_state_t state = DUDECT_NO_LEAKAGE_EVIDENCE_YET;
	while (state == DUDECT_NO_LEAKAGE_EVIDENCE_YET) {
		state = dudect_main(&ctx);
	}
	dudect_free(&ctx);
	return (int)state;
}
