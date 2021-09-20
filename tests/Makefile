CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++17
LDFLAGS = -I ../dilithium/ref -L../dilithium/ref -lpqcrystals_dilithium3_ref -lpqcrystals_fips202_ref

tests: test_keygen test_noises

test_keygen: test_keygen.cpp ../src/keygen.s ../dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) ../dilithium/ref/randombytes.o ../src/keygen.s test_keygen.cpp -o $@ $(LDFLAGS)

test_noises: test_noises.cpp test_gen_noises.s ../src/keygen.s

%.s: %.jazz
	jasminc -pasm $< > $@

clean:
	rm -f *.s montgomery.ec main *.eco write_twiddle_factors test_keygen twiddle_factors.jazz
