# Shared subroutines

Here are techniques that both Kyber and Dilithium use.
Some of these can potentially be brought in from the Kyber repo with minimal changes, while others may take a bit more work.

## Keccak-based pseudorandomness

* Both Kyber and Dilithium use SHAKE-128, SHAKE-256, and SHA3-256.
* In Kyber: Found in `fips202.jahh`. Currently uses many registers, so its caller easily runs out of registers.

## Sampling a matrix from a seed

* As a rough sketch, the subroutine applies SHAKE-128 to the seed to create a byte stream. It then parses the byte stream into a matrix, where rejection sampling is used to ensure that entries are uniformly random.
* In Kyber: Found in `gen_matrix.jahh`. Currently hard-coded for 3-row matrices, including manually unrolled loops.

## Matrix and vector operations

* Vector additions. Matrix-vector multiplications.
* In Kyber: Found in `polyvec_add2.jahh`, `polyvec_pointwise_acc.jahh`, and others files referenced by these. Matrices are implemented as 3-tuples of vectors.

## Fast Fourier Transform

* Used to speed up polynomial multiplications. Kyber's finite field does not have a n-th root of unity, so it is more complicated.
* In Kyber: Found in `poly_ntt.jahh` and `poly_invntt.jahh`.

# Dilithium specific techniques

Here are functions we won't be able to take from the Kyber repository.

## High and low order bits decomposition

* A space-optimization. Reduce key size by dropping lower-order bits.
* In [specification](https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf): Six subroutines on page 12 in Figure 3.

## Sample in ball

* Selects locations to apply random noises
* In [specification](https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf): Introduced on page 10; more details given on page 19.

# Wishlist

Here are some potential improvements to the Kyber repo.

 * Use of subarrays as mentioned last meeting. Potentially to represent rows of matrices? After all, Dilithium matrices have more than 3 rows.
 * Use of Jasmin's `require` feature, instead of using `gpp` with `#include`.
 * Currently we have 27 files with names `poly*.jahh`, each containing a single function.
	The median length of these is 33 lines.
	I'm not quite sure if this is a good way to structure code.
