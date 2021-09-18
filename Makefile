CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++17
LDFLAGS = -I ./dilithium/ref -L./dilithium/ref -lpqcrystals_dilithium5_ref -lpqcrystals_fips202_ref

test_keygen: test_keygen.cpp keygen.s dilithium/ref/randombytes.o
	$(CXX) $(CXXFLAGS) dilithium/ref/randombytes.o keygen.s test_keygen.cpp -o $@ $(LDFLAGS)

main: main.cpp montgomery.s shake256.s keygen.s dilithium/ref/randombytes.o

%.s: %.jazz *.jazz roots_of_unity.jazz
	jasminc -pasm $< > $@

%.o: %.h %.cpp

write_roots: write_roots.cpp

roots_of_unity.jazz: write_roots
	./write_roots

clean:
	rm -f *.s montgomery.ec main *.eco write_roots test_keygen roots_of_unity.jazz
