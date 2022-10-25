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
         bench_sign \
		 bench_verify

JAZZ_TEMPLATES := $(wildcard *.jazz.in)
JAZZ_GENERATED := $(JAZZ_TEMPLATES:.jazz.in=.jazz)

RANDOMBYTES := ../dilithium/ref/randombytes.o
SRC_DIR ?= ../libjade/src
LIB ?= $(SRC_DIR)/crypto_sign/dilithium/$(VARIANT)/amd64/$(ARCH)/lib.s

# Jasmin settings, based on libjade/src/Makefile.common
# TODO: Consider adding '-I Dilithium:$(dir $(LIB))' to fix the `sed` hack
JASMIN ?= jasminc
JFLAGS ?= -noinsertarraycopy
JINCLUDE := -I Jade:$(SRC_DIR)
JASMINC := $(JASMIN) $(JFLAGS) $(JINCLUDE)

tests: $(TESTS)

test_keygen: test_keygen.cpp $(LIB) $(RANDOMBYTES)
	$(CXX) $(CXXFLAGS) $(RANDOMBYTES) $(LIB) test_keygen.cpp -o $@ $(LDFLAGS)

test_sign: test_sign.cpp $(LIB) $(RANDOMBYTES)
	$(CXX) $(CXXFLAGS) $(RANDOMBYTES) $(LIB) test_sign.cpp -o $@ $(LDFLAGS)

test_verify: test_verify.cpp $(LIB) $(RANDOMBYTES)
	$(CXX) $(CXXFLAGS) $(RANDOMBYTES) $(LIB) test_verify.cpp -o $@ $(LDFLAGS)

test_matrix_mult: test_matrix_mult.cpp matrix_mult_export.s
	$(CXX) $(CXXFLAGS) $(RANDOMBYTES) $^ -o $@ $(LDFLAGS)

test_hashing: test_hashing.cpp hashing_export.s
	$(CXX) $(CXXFLAGS) $(RANDOMBYTES) $^ -o $@ $(LDFLAGS)

bench_keygen: bench_keygen.cpp $(LIB) $(RANDOMBYTES)
	$(CXX) $(CXXFLAGS) -O3 $(RANDOMBYTES) $(LIB) bench_keygen.cpp -o $@ $(LDFLAGS)

bench_sign: bench_sign.cpp $(LIB) $(RANDOMBYTES)
	$(CXX) $(CXXFLAGS) -O3 $(RANDOMBYTES) $(LIB) bench_sign.cpp -o $@ $(LDFLAGS)

bench_verify: bench_verify.cpp $(LIB) $(RANDOMBYTES)
	$(CXX) $(CXXFLAGS) -O3 $(RANDOMBYTES) $(LIB) bench_verify.cpp -o $@ $(LDFLAGS)

bench_fft: bench_fft.cpp fft_export.s
	$(CXX) $(CXXFLAGS) -O3 $(RANDOMBYTES) fft_export.s bench_fft.cpp -o $@ $(LDFLAGS)

bench_hashing: bench_hashing.cpp hashing_export.s
	$(CXX) $(CXXFLAGS) -O3 $(RANDOMBYTES) hashing_export.s bench_hashing.cpp -o $@ $(LDFLAGS)

test_%: test_%.cpp %_export.s
	$(CXX) $(CXXFLAGS) $(RANDOMBYTES) $*_export.s test_$*.cpp -o $@ $(LDFLAGS)

.PHONY: common_phony
common_phony:
	$(MAKE) --directory=../src/$(IMPL)/common

# HACK: To differentiate between different Dilithium implementations, we
# use templates and rewrite some variables (e.g., {DILITHIUM_VARIANT}) using
# sed.
# TODO: Differentiate between generated and normal files in the filename.
# For example, add '.genererated' to the name of the generated file.
%.jazz: %.jazz.in
	sed -e "1 i// AUTO-GENERATED FILE.  DO NOT EDIT." \
	    -e "s/{DILITHIUM_VARIANT}/$(VARIANT)/g" \
	    -e "s/{DILITHIUM_ARCH}/$(ARCH)/g" \
	    $< >$@

%.s: %.jazz $(SRC_DIR)/**
	$(JASMINC) -pasm $< >$@

clean:
	$(MAKE) --directory=../dilithium/ref clean
	$(MAKE) --directory=../dilithium/avx2 clean
	# $(MAKE) --directory=../src/common clean
	$(RM) $(TESTS) $(JAZZ_GENERATED) $(wildcard *.s)
