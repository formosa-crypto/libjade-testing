# EasyCrypt model

High level description found in MLWE\_PKE.ec
* Number of lines even less than the paper
* No NTT
* Matrix operations taken from `import Matrix_`. Seems to be from KyberAlgebra.ec.

TODO check Kyber.ec

TODO Find the functional correctness theorem

# General question

What's all these python doing here???

# Montgomery

Proves everything in ambient logic. Line 513:
```
op REDC T = let t = REDC' T in if N <= t then t-N else t.

lemma REDC_corr T:
 0 <= T < N + N*R =>
  0 <= REDC T < N  /\  REDC T %% N = T * Rinv %% N.
```

Not sure where does it get related to modules.

# NTT

Line 15 models imperative NTT: `module NTT = {`, and there are multiple versions.

The different versions are proven to be _equiv_.
Line 1722: `equiv eq_NTT4_NTT5 p : NTT4.ntt ~ NTT5.ntt: arg{1} = p /\ arg{2} = p ==> ={res}.`

Line 1036 switches over to a functional definition:
```
op partial_ntt (p : zmod Array256.t, len start bsj : int) =
BAdd.bigi
	predT
	(fun s =>
		(exp zeta1 (exponent len start s)) *
		p.[index len s bsj])
	0 len.
```

Line 1385 relates imperative and functional definitions using Hoare logic:
```
lemma naiventt (p : zmod Array256.t) :
hoare
	[NTT5.ntt :
		arg = (p) ==>
		all_range_2 (partial_ntt_spec res p 128) 0 128 0 2].
```

Wants to "prove equivalence  between functional  and imperative realizations" but hasn't gotten there yet.
Also, the proof isn't correct or complete.

Line 273 models functional NTT:
```
op dft (v : vector) =
offunv (fun k => BAdd.bigi predT (fun j => v.[j] * exp a (j * k)) 0 n).
```
but it's unfortunately unused.

# KyberPoly.ec

Similar use of `hoare` in line 128:

```
lemma poly_csubq_corr_h ap :
      hoare[ Mderand.poly_csubq :
           ap = lift_array256 rp /\
           pos_bound256_cxq rp 0 256 2 
           ==>
           ap = lift_array256 res /\
           pos_bound256_cxq res 0 256 1 ].
```

Line 1002: What...?

