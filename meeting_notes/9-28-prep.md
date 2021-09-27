# Meeting prep for September 28, 2021

## Technical items

### Progress Report

* Apologize for haven't done enough.
* Quick demo for [keygen](../tests/test_keygen.cpp).
* Duct tape engineering at its finest.

### Current Workflow

For all function F, implement `F.jazz` + `F_export.jazz` + `test_F.cpp`.
* Example: fft
	* [fft.jazz](../src/fft.jazz)
	* [fft\_export.jazz](../tests/fft_export.jazz)
	* [test\_fft.cpp](../tests/test_fft.cpp)
* Doesn't work very well for [keygen](../src/keygen.jazz).

### Summarize of the Challenges

* Originally went mainly by [spec](https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf).
	* was slower and more bug-prone
	* Harder to write tests
	* Follow ref impl now
* Long build time?
	* Long iteration time - `time make keygen.s` gives 1m24s.
	* Shapes the current workflow above
* Register allocation
	* calling `fips202.jazz` still runs out of register
	* pushing things onto the stack and making things inline kinda helps
* Deviation from reference impl.
	* FFT is the worst offender - now fixed
	* Bad handling of negative quantities (or lack thereof)
	* Still not constant time

### Follow up on wish list

* Now using subarrays!
* Now using `require`!
* Still spaghetti code though.

## Administrative items

### Licensing
* Preferred licenses?
	* CC0: We lose copyright completely and irrevocably.
	* LGPL: Seems more friendly than GPL after having worked in startups.
* What about `fips202.jazz` from the Kyber repo?
