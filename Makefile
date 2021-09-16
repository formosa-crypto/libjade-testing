CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++17

test_keygen: test_keygen.cpp keygen.s

main: main.cpp montgomery.s shake256.s keygen.s

%.s: %.jazz *.jazz roots_of_unity.jazz
	jasminc -pasm $< > $@

write_roots: write_roots.cpp

roots_of_unity.jazz: write_roots
	./write_roots

clean:
	rm -f *.s montgomery.ec main *.eco write_roots test_keygen roots_of_unity.jazz
