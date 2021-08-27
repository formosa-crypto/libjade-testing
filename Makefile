main: main.cpp montgomery.s

%.japp: %.jazz
	gpp -o $@ $<

%.s: %.japp
	jasminc -pasm $< > $@

clean:
	rm -f *.japp *.s montgomery.ec
