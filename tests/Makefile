VARIANT ?= dilithium3
ifeq "${VARIANT}" "dilithium2"
	MODE := 2
endif
ifeq "${VARIANT}" "dilithium3"
	MODE := 3
endif
ifeq "${VARIANT}" "dilithium5"
	MODE := 5
endif

ARCH ?= ref

ifeq "${ARCH}" "ref"
	CPPFLAGS += '-DDILITHIUM_ARCH_REF'
endif
ifeq "${ARCH}" "avx2"
	CPPFLAGS += '-DDILITHIUM_ARCH_AVX2'
endif

CXXFLAGS := $(CPPFLAGS) -g -Og -ftrapv -Wall -Wextra -Wpedantic -std=c++17 "-DDILITHIUM_MODE=${MODE}"
LDFLAGS := -I ../dilithium/ref \
          -I ../dilithium/avx2 \
          -L../dilithium/ref \
          -L../dilithium/avx2 \
          -lpqcrystals_dilithium$(MODE)_ref \
          -lpqcrystals_dilithium$(MODE)_avx2 \
          -lpqcrystals_fips202_ref \
          -lpqcrystals_fips202_avx2 \
          -lpqcrystals_fips202x4_avx2

TESTS ?= test_fft \
         test_gen_matrix \
         test_packing \
         test_keygen_firsthalf \
         test_hashing \
         test_matrix_mult \
         test_sign_fragments \
         test_expandmask \
         test_sign \
         test_rounding \
         test_keygen \
         test_verify \
         bench_fft \
         bench_hashing \
         bench_keygen \
         bench_sign

JAZZ_TEMPLATES := $(wildcard *.jazz.in)
JAZZ_GENERATED := $(JAZZ_TEMPLATES:.jazz.in=.jazz)

LIB ?= ../src/${VARIANT}/${ARCH}/lib.s

tests: $(TESTS)

test_keygen: test_keygen.cpp $(LIB) ../dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o $(LIB) test_keygen.cpp -o $@ $(LDFLAGS)

test_sign: test_sign.cpp $(LIB) ../dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o $(LIB) test_sign.cpp -o $@ $(LDFLAGS)

test_verify: test_verify.cpp $(LIB) ../dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o $(LIB) test_verify.cpp -o $@ $(LDFLAGS)

test_matrix_mult: test_matrix_mult.cpp matrix_mult_export.s
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o $^ -o $@ $(LDFLAGS)

test_hashing: test_hashing.cpp hashing_export.s
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o $^ -o $@ $(LDFLAGS)

bench_keygen: bench_keygen.cpp $(LIB) ../dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) -O3 ../dilithium/ref/randombytes.o $(LIB) bench_keygen.cpp -o $@ $(LDFLAGS)

bench_sign: bench_sign.cpp $(LIB) ../dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) -O3 ../dilithium/ref/randombytes.o $(LIB) bench_sign.cpp -o $@ $(LDFLAGS)

bench_fft: bench_fft.cpp fft_export.s
	$(CXX) $(CXXFLAGS) -O3 ../dilithium/ref/randombytes.o fft_export.s bench_fft.cpp -o $@ $(LDFLAGS)

bench_hashing: bench_hashing.cpp hashing_export.s
	$(CXX) $(CXXFLAGS) -O3 ../dilithium/ref/randombytes.o hashing_export.s bench_hashing.cpp -o $@ $(LDFLAGS)

test_%: test_%.cpp %_export.s
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o $*_export.s test_$*.cpp -o $@ $(LDFLAGS)

.PHONY: common_phony
common_phony:
	$(MAKE) --directory=../src/$(IMPL)/common

%.jazz: %.jazz.in
	sed -e "1 i// AUTO-GENERATED FILE.  DO NOT EDIT." \
	    -e "s/{DILITHIUM_VARIANT}/$(VARIANT)/g" \
	    -e "s/{DILITHIUM_ARCH}/$(ARCH)/g" \
	    $< >$@

%.s: %.jazz ../src/*/*/*.jazz common_phony
	jasminc -pasm $< >$@

clean:
	$(MAKE) --directory=../dilithium/ref clean
	$(MAKE) --directory=../dilithium/avx2 clean
	$(MAKE) --directory=../src/common clean
	$(RM) $(TESTS) $(JAZZ_GENERATED)
