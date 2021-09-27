# Meeting Preparation for September 28, 2021

## Technical items

### Progress Report

* Done implementing keygen in Jasmin!
* Apologize for haven't done enough.
* Quick [demo](../tests/test_keygen.cpp).
* Duct tape engineering at its finest.

### Current Workflow

For all functions F, implement `F.jazz` + `F_export.jazz` + `test_F.cpp`.
* Example: fast Fourier transform
	* [fft.jazz](../src/fft.jazz)
	* [fft\_export.jazz](../tests/fft_export.jazz)
	* [test\_fft.cpp](../tests/test_fft.cpp)
* Doesn't work very well for [keygen](../src/keygen.jazz).

### Summary of Challenges

Here is a short list of software engineering challenges that I've mostly overcome,
albeit with some minor lingering issues.

* Unfamiliarity with assembly
	* `asmgen: not able to assemble address`
	* `asmgen: invalid instruction, check do not pass : MOV_32`
	*
          internal compilation error in function test:
	        asmgen: (compile_arg) not compatible asm_arg
          Please report at https://github.com/jasmin-lang/jasmin/issues
	    
	* Unsure what I can and cannot do.
* Register allocations
	* `error in “one-varmap” checker: modified expression`
	* Automatic variables live on the stack.
	* Adding `inline` on functions seems to help.
	* Using `fips202.jazz` still easily runs out of register.
* Long build time
	* Long iteration time - `time make keygen.s` says 1m24s.
	* Shapes workflow above.
* Originally went by the [specification](https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf)
	* Took more effort.
	* More bug-prone.
	* Writing tests is hard!
* Deviations from the [reference C implementation](https://github.com/ethanlee515/dilithium)
	* FFT was the worst offender - now fixed.
	* Bad handling of negative quantities (or lack thereof).
	* Still not constant time.

### Following up Previous Discussions

* From [last meeting](./9-14-prep.md):
	* Now using subarrays!
	* Now using `require`!
	* Still spaghetti code though.
	* Still hard-coded at level 3.
* How's the Jasmin memory safety checker?

## Administrative Items

### Licensing

* Preferred licenses?
	* CC0: We lose copyright completely and irrevocably. What if we regret it one day?
	* LGPL: Seems more friendly than GPL, after having worked at startups.
* What about `fips202.jazz` from the Kyber repo?
