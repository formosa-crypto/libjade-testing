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

CXXFLAGS = -g -Og -Wall -Wextra -Wpedantic -std=c++17 "-DDILITHIUM_MODE=${MODE}"
LDFLAGS = -I ../dilithium/ref -L../dilithium/ref -lpqcrystals_dilithium${MODE}_ref -lpqcrystals_fips202_ref

TESTS ?= test_fft \
         test_gen_matrix \
         test_packing \
         test_keygen_firsthalf \
         test_hashing \
         test_matrix_mult \
         test_sign_fragments \
         test_expandmask \
         test_sign \
         test_rounding
# TODO: test_keygen

LIB ?= ../src/${VARIANT}/lib.s

tests: $(TESTS)

test_keygen: test_keygen.cpp ${LIB} ../dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o ${LIB} test_keygen.cpp -o $@ $(LDFLAGS)

test_sign: test_sign.cpp ${LIB} ../dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o ${LIB} test_sign.cpp -o $@ $(LDFLAGS)

test_verify: test_verify.cpp ${LIB} ../dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o ${LIB} test_verify.cpp -o $@ $(LDFLAGS)

bench_keygen: bench_keygen.cpp ${LIB} ../dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o ${LIB} bench_keygen.cpp -o $@ $(LDFLAGS)

bench_sign: bench_sign.cpp ${LIB} ../dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o ${LIB} bench_sign.cpp -o $@ $(LDFLAGS)

bench_fft: bench_fft.cpp fft_export.s
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o fft_export.s bench_fft.cpp -o $@ $(LDFLAGS)

bench_hashing: bench_hashing.cpp hashing_export.s
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o hashing_export.s bench_hashing.cpp -o $@ $(LDFLAGS)

test_%: test_%.cpp %_export.s
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o $*_export.s test_$*.cpp -o $@ $(LDFLAGS)

%.s: %.jazz ../src/**/*.jazz
	jasminc -pasm $< > $@

clean:
	$(MAKE) --directory=../dilithium/ref clean
	$(MAKE) --directory=../src/common clean
	$(MAKE) --directory=../src/dilithium3 clean
	rm -f $(TESTS)
