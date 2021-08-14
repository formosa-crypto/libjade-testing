require import AllCore IntDiv CoreMap List.
require import Montgomery.
from Jasmin require import JModel.

op dilithium_modulo = 8380417.

module Correctness = {
	proc brute_force_mod_check(v: int) : bool = {
		var correct_answer : W64.t;
		var computed_answer : W64.t;

		computed_answer <@ M.brute_force_mod(W64.of_int v);
		correct_answer <- W64.of_int (v %% dilithium_modulo);
		return (computed_answer = correct_answer);
	}
}.

(* Check somewhere that v is int64? *)

(* local lemma brute_force_mod_correctness *)
