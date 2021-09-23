CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++17

main: keygen.s

%.s: %.jazz *.jazz twiddle_factors.jazz montgomery_of_noise.jazz
	jasminc -pasm $< > $@

%.o: %.h %.cpp

write_twiddle_factors: write_twiddle_factors.cpp

twiddle_factors.jazz: write_twiddle_factors
	./write_twiddle_factors

montgomery_of_noise.jazz: write_montgomery_of_noise.py
	python3 write_montgomery_of_noise.py > montgomery_of_noise.jazz

clean:
	rm -f *.s montgomery.ec main *.eco write_twiddle_factors test_keygen twiddle_factors.jazz
