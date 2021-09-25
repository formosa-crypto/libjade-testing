CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++17
LDFLAGS = -I ../dilithium/ref -L../dilithium/ref -lpqcrystals_dilithium3_ref -lpqcrystals_fips202_ref

tests: test_keygen test_noises test_fft test_gen_matrix test_packing

test_keygen: test_keygen.cpp ../src/keygen.s ../dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o ../src/keygen.s test_keygen.cpp -o $@ $(LDFLAGS)

test_noises: test_noises.cpp test_gen_noises.s ../src/keygen.s

test_fft: test_fft.cpp fft_export.s

test_gen_matrix: test_gen_matrix.cpp gen_matrix_export.s ../src/gen_matrix.jazz
	$(CXX) $(CXXFLAGS) -DDILITHIUM_MODE=3 ../dilithium/ref/randombytes.o gen_matrix_export.s test_gen_matrix.cpp -o $@ $(LDFLAGS)

test_packing: test_packing.cpp packing_export.s ../dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) -DDILITHIUM_MODE=3 ../dilithium/ref/randombytes.o packing_export.s test_packing.cpp -o $@ $(LDFLAGS)

%.s: %.jazz ../src/*.jazz
	jasminc -pasm $< > $@

clean:
	rm -f *.s test_keygen test_fft test_noises test_gen_matrix
