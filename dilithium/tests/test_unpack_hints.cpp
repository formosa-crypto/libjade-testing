#include <iostream>
#include <random>
#include <cstring>

extern "C" {
    #include "../dilithium/ref/api.h"
    #include "../dilithium/ref/params.h"
}

extern "C" {
    uint64_t unpack_hints_jazz(const uint8_t hints_buf[OMEGA + K], int32_t hints[K][N]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

static void pack_hints_ref(uint8_t h_buf[OMEGA + K], const int32_t h[K][N]) {
    // Encode h
    for(size_t i = 0; i < OMEGA + K; ++i) {
        h_buf[i] = 0;
    }

    size_t k = 0;
    for(size_t i = 0; i < K; ++i) {
        for(size_t j = 0; j < N; ++j) {
            if(h[i][j] != 0) {
                h_buf[k++] = j;
            }
        }
        h_buf[OMEGA + i] = k;
    }
}

static int8_t unpack_hints_ref(int32_t h[K][N], const uint8_t h_buf[OMEGA + K]) {
    // Decode h
    size_t k = 0;
    for (size_t i = 0; i < K; ++i) {
        for (size_t j = 0; j < N; ++j) {
            h[i][j] = 0;
        }

        if (h_buf[OMEGA + i] < k || h_buf[OMEGA + i] > OMEGA) {
            return 1;
        }

        for (size_t j = k; j < h_buf[OMEGA + i]; ++j) {
            // Coefficients are ordered for strong unforgeability
            if (j > k && h_buf[j] <= h_buf[j-1]) {
                return 1;
            }
            h[i][h_buf[j]] = 1;
        }

        k = h_buf[OMEGA + i];
    }
    return 0;
}

static int test_functional() {
    int32_t h_orig[K][N] = {};
    int32_t h0[K][N] = {};
    int32_t h1[K][N] = {};
    uint8_t h_buf[OMEGA + K] = {};

    unsigned int popcount = std::numeric_limits<unsigned int>::max();
    do {
        popcount = sampleByte();
    } while (popcount > OMEGA);

    unsigned int k = 0;
    while (k < popcount) {
        size_t vec_idx = std::numeric_limits<size_t>::max();
        do {
            vec_idx = sampleByte() % 8;
        } while (vec_idx >= K);
        size_t poly_idx = sampleByte();
        if (h_orig[vec_idx][poly_idx] != 0) {
            continue;
        }
        h_orig[vec_idx][poly_idx] = 1;
        k++;
    }

    pack_hints_ref(h_buf, h_orig);
    int8_t status_ref = unpack_hints_ref(h0, h_buf);
    int8_t status_jazz = unpack_hints_jazz(h_buf, h1);

	int ref_eq = memcmp(h0, h_orig, K * N * sizeof(int32_t));
	int jazz_eq = memcmp(h1, h0, K * N * sizeof(int32_t));

    // std::cout << "popcount: " << popcount << '\n';
	// std::cout << std::hex << "status_ref: 0x" << (int)status_ref << '\n';
	// std::cout << std::hex << "ref_eq: 0x" << (int)ref_eq << '\n';
	// std::cout << std::hex << "status_jazz: 0x" << (int)status_jazz << '\n';
	// std::cout << std::hex << "jazz_eq: 0x" << (int)jazz_eq << '\n';

    if (status_ref != 0 || status_jazz != 0 || ref_eq != 0 || jazz_eq != 0) {
        return -1;
    }

	return 0;
}

int main() {
    for (int i = 0; i < 10000; i++) {
        int status = test_functional();
        if (status != 0) {
            return status;
        }
    }
    return 0;
}