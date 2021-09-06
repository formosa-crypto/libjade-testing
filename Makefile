CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++17

main: main.cpp montgomery.s shake256.s

%.japp: %.jazz
	gpp -o $@ $<

%.s: %.japp
	jasminc -pasm $< > $@

write_roots: write_roots.cpp

clean:
	rm -f *.japp *.s montgomery.ec main *.eco write_roots
