CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++17

main: all.s keygen.s sign.s verify.s

%.s: %.jazz *.jazz twiddle_factors.jazz
	jasminc -pasm $< > $@

%.o: %.h %.cpp

write_twiddle_factors: write_twiddle_factors.cpp

twiddle_factors.jazz: write_twiddle_factors
	./write_twiddle_factors

clean:
	rm -f *.s montgomery.ec main *.eco write_twiddle_factors test_keygen twiddle_factors.jazz
