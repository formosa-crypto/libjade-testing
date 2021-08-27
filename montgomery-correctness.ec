require import AllCore IntDiv CoreMap List.
require import Montgomery.
from Jasmin require import JModel.

op dilithium_modulo = 8380417.
op dilithium_modulo_left_shift41 = 18428731874223325184.
op dilithium_modulo_inv = 58728449.

(*
lemma dilithium_mod_right_inv_correct:
    dilithium_modulo * dilithium_modulo_inv %% (2 ^ 32) = 1.
proof.
    trivial.
qed.

lemma dilithium_mod_left_inv_correct:
    dilithium_modulo_inv * dilithium_modulo %% (2 ^ 32) = 1.
proof.
    trivial.
qed.
  *)

lemma dilithium_mod_inv_correct:
    dilithium_modulo_inv * dilithium_modulo %% (2 ^ 32) = 1.
proof.
    trivial.
qed.
  
  
op montgomery = fun (x : int) => (2 ^ 32 * x) %% dilithium_modulo.

op montgomery_redc_m = fun (a' : int) => (-a' * dilithium_modulo_inv) %% (2 ^ 32).

op montgomery_redc_t = fun (a' : int) =>
    let m = montgomery_redc_m(a') in
    (a' + m * dilithium_modulo) %/ (2 ^ 32).

op montgomery_redc = fun (a' : int) => 
    let m = montgomery_redc_m(a') in
    let t = montgomery_redc_t(a') in
    if t < dilithium_modulo then t else t - dilithium_modulo.

lemma montgomery_correct_div_t:
    forall a', 0 <= a' < dilithium_modulo =>
    montgomery_redc_t(a') * (2 ^ 32) = a' + montgomery_redc_m(a') * dilithium_modulo.
proof.
    move => a' a'_range.
    have r_divides_mN: (a' + montgomery_redc_m(a') * dilithium_modulo) %% 2 ^ 32 = 0.
        rewrite /montgomery_redc_m.
        rewrite - modzDmr.
        rewrite modzMml.

        have move_a':
            (- a' * dilithium_modulo_inv) * dilithium_modulo =
            dilithium_modulo_inv * dilithium_modulo * (-a').
            (* I really didn't want to hit associativity with smt... *)
            smt.
        rewrite move_a'.
        rewrite - modzMml .
        rewrite dilithium_mod_inv_correct.
        apply modzDmr.
    rewrite /montgomery_redc_t.
    simplify.
    rewrite divzE.
    rewrite r_divides_mN.
    simplify.
done.

(*
251 (* -------------------------------------------------------------------- *)
252 lemma nosmt modzE m d : m %% d = m - (m %/ d) * d.
253 proof. by have [+ _] - {2}-> := edivzP m d; rewrite addrAC addrN. qed.
254
255 (* -------------------------------------------------------------------- *)
256 lemma nosmt divzE m d : m %/ d * d = m - m %% d.
257 proof. by rewrite modzE; ring. qed.
*)
    

    
            
    



        





(* Add-linear?
Want: (a%d + b%d) %d = (a+b)%d
*)

(*
337 lemma nosmt modzDml m n d : (m %% d + n) %% d = (m + n) %% d.
338 proof. by rewrite {2}(divz_eq m d) -addrA modzMDl. qed.
339
340 lemma nosmt modzDmr m n d : (m + n %% d) %% d = (m + n) %% d.
341 proof. by rewrite !(addrC m) modzDml. qed.
*)

(* mult-linear: modzMml *)
(* modzMml m n d : ((m %% d) * n) %% d = (m * n) %% d *)
    

    
    



    


lemma montgomery_redc_is_left_inv:
    forall x, 0 <= x < 2 ^ 32 => montgomery_redc(montgomery(x)) = x.
proof.
    move => x x_range.
    





