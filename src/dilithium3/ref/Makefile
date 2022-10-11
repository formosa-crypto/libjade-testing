all: lib.s

%.s: %.jazz *.jazz ../common/*.jazz ../common/twiddle_factors.jazz
	jasminc -pasm $< >$@

.PHONY: ../common/twiddle_factors.jazz
../common/twiddle_factors.jazz:
	$(MAKE) --directory=../common $(notdir $@)

.PHONY: clean
clean:
	$(RM) -f *.s