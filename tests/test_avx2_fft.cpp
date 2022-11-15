#include <random>
#include <iostream>

extern "C"
{
#include "macros.h"
#include <params.h>
#include <ntt.h>
#include <string.h>
#include <immintrin.h>
}

extern "C"
{
    void fft_jazz(__m256i *buf);
    void ifft_jazz(__m256i *buf);

    void NTT_REF(int32_t poly32[N]);
    void INVNTT_REF(int32_t poly32[N]);
}

static int32_t freeze(int32_t x)
{
    x %= Q;
    x += Q;
    x %= Q;
    return x;
}

// print_nttdomain_order dumps the order in which coefficients exist in the NTT
// domain.
static int print_nttdomain_order(void)
{
    int32_t poly32[N];
    __m256i poly256[32];

    for (size_t i = 0; i < N; i++)
    {
        // poly32[i] = i;
        poly32[i] = ((int64_t)i * -114592) % Q;
    }

    memcpy(poly256, poly32, 1024);
    fft_jazz(poly256);
    memcpy(poly32, poly256, 1024);
    INVNTT_REF((int32_t *)poly32);

    // invntt_tomont_avx2_jazz(poly256);

    std::cout << "poly256: ";
    for (size_t i = 0; i < 256; i++)
    {
        poly32[i] = freeze(poly32[i]);
        std::cout << (poly32[i] % Q) << ", ";
    }
    std::cout << "\n";
    std::cout << std::flush;
    return 0;
}

static int test_ntt(void)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int64_t> distrib(INT32_MIN, INT32_MAX);

    __m256i buf_new[32] = {};
    __m256i buf_ref[32] = {};

    for (size_t test_idx = 0; test_idx < 1000; test_idx++)
    {
        for (size_t i = 0; i < 32; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                int64_t rand = distrib(gen);
                rand |= distrib(gen) << 32;
                buf_ref[i][j] = rand;
                buf_new[i][j] = rand;
            }
        }

        NTT_REF((int32_t *)buf_ref);
        fft_jazz(buf_new);

        for (size_t i = 0; i < 32; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                if (freeze(buf_new[i][j]) != freeze(buf_ref[i][j]))
                {
                    std::cerr << "test_ntt: mismatch at [" << i << "][" << j << "]:\n";
                    std::cerr << std::hex << buf_new[i][j] << " != " << buf_ref[i][j] << "\n";
                    return 1;
                }
            }
        }
    }
    return 0;
}

static int test_invntt(void)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int64_t> distrib(INT32_MIN, INT32_MAX);

    __m256i buf_new[32] = {};
    __m256i buf_ref[32] = {};

    for (size_t test_idx = 0; test_idx < 1000; test_idx++)
    {
        for (size_t i = 0; i < 32; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                int64_t rand = distrib(gen);
                rand |= distrib(gen) << 32;
                buf_ref[i][j] = rand;
                buf_new[i][j] = rand;
            }
        }

        INVNTT_REF((int32_t *)buf_ref);
        ifft_jazz(buf_new);

        for (size_t i = 0; i < 32; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                if (freeze(buf_new[i][j]) != freeze(buf_ref[i][j]))
                {
                    std::cerr << "test_invntt: mismatch at [" << i << "][" << j << "]:\n";
                    std::cerr << std::hex << buf_new[i][j] << " != " << buf_ref[i][j] << "\n";
                    return 1;
                }
            }
        }
    }
    return 0;
}

int main(void)
{
    int status;

    // NOTE: Set to true if you want to know the order in which the NTT
    // output is ordered.
    if (false)
    {
        status = print_nttdomain_order();
        if (status != 0)
        {
            return status;
        }
    }

    status = test_ntt();
    if (status != 0)
    {
        return status;
    }
    status = test_invntt();
    if (status != 0)
    {
        return status;
    }

    std::cerr << "OK\n";
    return 0;

    // TODO: [electricdusk] LEFT HERE
    // Get all tests working again
}
