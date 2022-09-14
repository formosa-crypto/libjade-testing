CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++17

twiddle_factors.jazz: write_twiddle_factors
	./write_twiddle_factors

write_twiddle_factors: write_twiddle_factors.cpp

%.o: %.h %.cpp

.PHONY: clean
clean:
	$(RM) -f write_twiddle_factors twiddle_factors.jazz

