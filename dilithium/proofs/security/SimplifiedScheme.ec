require import AllCore Distr List DistrExtras.
require DigitalSignaturesRO.
require MatPlus.
(* require import PolyReduce. *)
require import IntDiv.
(* require ZqRounding. *)
(* require ZModFieldExtras. *)
require import Nat.

require import IDSabort.
require FSabort.
require FSa_CMAtoKOA.
require FSa_CRtoGen.

require DRing DVect MLWE SelfTargetMSIS. 


(* Dilithium-specific parameters *)

(* secret key range.
 * Typically "eta" but that's a reserved keyword in EC. *)
op e : int.

(* upper bound on number of itertions. *)
op kappa : int.

(* matrix dimensions *)
op k : {int | 0 < k} as gt0_k.
op l : {int | 0 < l} as gt0_l.

(* Abstract theory representing Rq = Zq[X]/(X^n + 1) and the high/lowBits operations *)
(* The constants [n] and [q] are defined by this theory *)
clone import DRing as DR. 
import RqRing.

(* Parameters for "Rounding" (e.g., highBits, lowBits, and shift)  *)
op gamma1 : int.
op gamma2 : { int | 2 <= gamma2 <= q %/ 4 } as gamma2_bound.
axiom gamma2_div : 2 * gamma2 %| (q - 1).

(* beta in spec.
 * beta is again a reserved keyword in EC. *)
op b : int.

clone import DVect as DV with 
  theory DR <- DR,
  op HL.alpha <- 2*gamma2 
proof HL.ge2_alpha, HL.alpha_halfq_le, HL.even_alpha, HL.alpha_almost_divides_q.
realize HL.ge2_alpha by smt(gamma2_bound).
realize HL.even_alpha by smt().
realize HL.alpha_halfq_le by smt(gamma2_bound).
realize HL.alpha_almost_divides_q by apply gamma2_div.

import DV.MatRq. (* Matrices and Vectors over Rq *)
import DV.HL.    (* highBitsV and lowBitsV with alpha = 2 * gamma2 *)


(* challenge weight *)
op tau : { int | 1 <= tau <= n } as tau_bound.

lemma cnorm_dC c tau : c \in dC tau => cnorm c <= 1 by smt(supp_dC).

type M.

type challenge_t = Rq.
type SK = matrix * vector * vector.
type PK = matrix * vector.
type commit_t = high list.
type response_t = vector. 

(* op dC : challenge_t distr = dC tau.  *)

(* Just storing `y` should be good here. *)
type pstate_t = vector. 

clone IDS as DID with 
  type PK <= PK,
  type SK <= SK,
  type W <= commit_t,
  type C <= challenge_t,
  type Z <= response_t,
  type Pstate <= pstate_t proof*.

(* Raises "`ID.Imp_Game` is incompatible" error *)
(*
clone import FSabort as OpFSA with
  theory ID <= DID,
  type M <- M,
  op dC <- dC.
*)

(* Unsure if this is intended usage;
 * can't recall from the meeting the other day *)
clone import FSabort as OpFSA with
  type ID.PK <= PK,
  type ID.SK <= SK,
  type ID.W <= commit_t,
  type ID.C <= challenge_t,
  type ID.Z <= response_t,
  type ID.Pstate <= pstate_t,
  type M <= M,
  op dC <= dC tau
proof* by smt(dC_ll dC_uni tau_bound).

(* -- Procedure-based -- *)

op recover (pk : PK) (c : challenge_t) (z : response_t) : commit_t =
  let (mA, t) = pk in
  highBitsV (mA *^ z - c ** t).

clone import CommRecov as FSaCR with
  op recover <= recover,
  op kappa <- kappa.
(* TODO instantiate a couple more things and prove axioms
 * TODO at least `recover` has to be defined for things to be provable *)
import DSS.

module (SimplifiedDilithium : SchemeRO)(H: Hash) = {
  proc keygen() : PK * SK = {
    var pk, sk;
    var mA, s1, s2;
    mA <$ dmatrix dRq k l;
    s1 <$ dvector (dRq_ e) l;
    s2 <$ dvector (dRq_ e) k;
    pk <- (mA, mA *^ s1 + s2);
    sk <- (mA, s1, s2);
    return (pk, sk);
  }

  proc sign(sk: SK, m: M) : Sig = {
    var z : vector option;
    var c : R;
    var ctr : int;
    var y, w, w1;
    var mA, s1, s2;
    (* silences unused variable warning *)
    c <- witness;

    (mA, s1, s2) <- sk;
    ctr <- 0;
    z <- None;
    while(ctr < kappa /\ z = None) {
      y <$ dvector (dRq_ (gamma1 - 1)) l;
      w <- mA *^ y;
      w1 <- highBitsV w;
      c <@ H.get((w1, m));
      z <- Some (y + c ** s1);
      if(gamma1 - b <= inf_normv (oget z) \/
         gamma2 - b <= inf_normv (lowBitsV (mA *^ y - c ** s2))) {
        z <- None;
      }
      ctr <- ctr + 1;
    }
    return if z = None then None else Some (c, oget z);
  }

  proc verify(pk: PK, m : M, sig : Sig) = {
    var w, c, z, c';
    var mA, t1;
    var result;
    (mA, t1) <- pk;
    result <- false;
    if(sig <> None) {
      (c, z) <- oget sig;
      w <- highBitsV (mA *^ z - c ** t1);
      c' <@ H.get((w, m));
      result <- size z = l /\ inf_normv z < gamma1 - b /\ c = c';
    }
    return result;
  }
}.

(** KOA to MLWE + SelfTargetMSIS *)

clone import MLWE as RqMLWE with 
  theory M <- MatRq,
  op dR <- dRq,
  op dS <- dRq_ e,
  op k <- k,
  op l <- l
proof* by smt(gt0_k gt0_l).

clone import SelfTargetMSIS as RqStMSIS with
  theory M <- MatRq,
  type M <- M,
  op m <- k,
  op n <- l+1,
  op dR <- dRq,
  op dC <- dC tau,
  op inf_norm <- inf_normv,
  op gamma <- max (gamma1 - b) (gamma2+1).

module H = DSS.PRO.RO.
module G = RqStMSIS.PRO.RO.

module RedMLWE (A : Adv_EFKOA_RO) (H : Hash_i) : RqMLWE.Adversary = { 
  proc distinguish (mA : matrix, t : vector) = { 
    var pk,m,sig,r;
    H.init();
    pk <- (mA,t);
    (m,sig) <@ A(H).forge(pk);
    r <@ SimplifiedDilithium(H).verify(pk,m,sig);
    return r;
  }
}.

module H' : Hash_i = { 
  proc init = G.init

  proc get(w1,mu) = {
    var r;
    r <@ G.get(shiftV w1,mu);
    return r;
  }
}.

module RedMSIS (A : Adv_EFKOA_RO) (H : RqStMSIS.PRO.RO) = { 
  proc guess(mB : matrix) : vector * M = { 
    var mA,tbar,t,mu,sig,c,z,w,e,y;
    mA <- subm mB 0 k 0 l;
    tbar <- col mB l;
    t <- -tbar;
    (mu,sig) <@ A(H').forge(mA,t); (* discard H, use H' *)
    y <- witness;
    if (sig <> None) {
      (c,z) <- oget sig;
      w <- (mA *^ z - c ** t);
      e <- -lowBitsV w;
      y <- e || z || vectc 1 c;
    }
    return (y,mu);
  }
}.

op locked (x : 'a) = x axiomatized by unlock.
lemma lock (x : 'a) : x = locked x by rewrite unlock.

section PROOF.

declare module A <: Adv_EFKOA_RO{-H,-G}.

local module S1 (H : Hash) = { 
  proc keygen() : PK * SK = {
    var pk, mA, t;
    mA <$ dmatrix dRq k l;
    t  <$ dvector dRq k;
    pk <- (mA, t);
    return (pk,witness);
  }
  
  proc sign(sk: SK, m: M) : Sig = { return None; }

  proc verify = SimplifiedDilithium(H).verify
}.

(* Hop 1 : replace keygen with (completely) random public key *)
local lemma hop1 &m : 
  Pr [EF_KOA_RO(SimplifiedDilithium,A,H).main() @ &m : res ] <=
  Pr [EF_KOA_RO(S1,A,H).main() @ &m : res] + 
  `| Pr[ GameL(RedMLWE(A,H)).main() @ &m : res ] - Pr [ GameR(RedMLWE(A,H)).main() @ &m : res ] |.
proof.
have -> : Pr [EF_KOA_RO(SimplifiedDilithium,A,H).main() @ &m : res ] = 
          Pr[ GameL(RedMLWE(A,H)).main() @ &m : res ].
- byequiv (_: ={glob A,glob H} ==> ={res}) => //; proc. 
  inline{1} 2; inline{1} 2; inline{2} 4. swap{2} 6 -5.
  by sim; wp; sim. (* why is the "wp" necessary - is sim this stupid? *)   
suff -> : Pr [EF_KOA_RO(S1,A,H).main() @ &m : res ] = 
          Pr[ GameR(RedMLWE(A,H)).main() @ &m : res ] by smt().
- byequiv (_: ={glob A,glob H} ==> ={res}) => //; proc. 
  inline{1} 2; inline{1} 2; inline{2} 3. swap{2} 5 -4. 
  by sim; wp; sim. 
qed.

import SmtMap.

(* Hop2 : replace H with H' (wrapped RO from SelfTargetMSIS *)
local lemma hop2 &m : 
  Pr [EF_KOA_RO(S1,A,H).main() @ &m : res] = Pr [EF_KOA_RO(S1,A,H').main() @ &m : res].
proof.
byequiv (_ : ={glob A} ==> ={res}) => //; proc. 
inline{1} 2; inline{2} 2. 
seq 3 3 : (={m,sig,pk} /\ 
           forall (w : commit_t) (mu : M), 
             H.m.[(w,mu)]{1} = G.m.[(shiftV w,mu)]{2}). 
- call (:forall (w : commit_t) (mu : M), 
         H.m.[(w,mu)]{1} = G.m.[(shiftV w,mu)]{2}).
  + proc; inline*; sp. 
    seq 1 1 : (#pre /\ r{1} = r0{2}); first by auto.
    if; 1,3: by auto => /> /#.
    auto => />. smt(get_setE set_set_sameE shiftV_inj).
  by inline*; auto => />; smt(emptyE).
inline*; wp. sp. if; 1,3: by auto. 
sp 3 4; seq 1 1 : (#pre /\ r0{1} = r1{2}); first by auto.
if; 2,3: by auto => />; smt(get_set_sameE). (* uff ... *)
move => /> &1 &2. case: (sig{2}) => // -[c z] /> Hshift.
pose w1 := highBitsV _. smt().
qed.

import StdOrder.RealOrder.

(* move to EC lib? *)
lemma max_ltrP (i j k : int) : i < max j k <=> i < j \/ i < k by smt().

local lemma hop3 &m : 
  Pr[EF_KOA_RO(S1, A, H').main() @ &m : res] <= Pr[Game(RedMSIS(A), G).main() @ &m : res].
proof.
byequiv (_ : ={glob A} ==> res{1} => res{2}) => //; proc. 
inline{1} 2; inline{1} 1. inline{1} 2; inline{2} 3. 
seq 6 7 : (={sig,PRO.RO.m} /\ (forall x, x \in PRO.RO.m => oget PRO.RO.m.[x] \in dC tau){1}
           /\ m{1} = mu0{2} /\ pk{1} = (mA0,t){2} /\ 
           (mA = (mA0 || - colmx t) /\ size mA0 = (k,l) /\ size t = k){2}).
- (* merge [dmatrix/dvector] sampling on LHS *)
  seq 3 2 : (={glob A,PRO.RO.m} /\ (forall x, x \in PRO.RO.m => oget PRO.RO.m.[x] \in dC tau){1}
            /\ size mA{1} = (k,l) /\ size t{1} = k /\
            (mA || - colmx t){1} = mA{2}). 
  + inline*; sp 1 1. 
    conseq (:_ ==> size mA{1} = (k,l) /\ size t{1} = k /\ (mA || - colmx t){1} = mA{2}).
    * smt(emptyE).
    rnd (fun mAt : matrix * vector => (mAt.`1 || -colmx mAt.`2))  
        (fun mAt : matrix => (subm mAt 0 k 0 l,- col mAt l)) : *0 *0.
    skip => /= &1 &2 _. split => [A|?]; last split => [A|?].
    * rewrite dmap_id => /size_dmatrix /(_ _ _) /=; 1,2: smt(Top.gt0_k Top.gt0_l).
      rewrite colmxN oppvK => -[<-] ?. rewrite subm_colmx; smt(Top.gt0_k Top.gt0_l).
    * rewrite -(dmap_dprodE _ _ (fun x => x)) !dmap_id.
      rewrite dprod1E (@dvector_rnd_funi _ _ (col A l)) ?dRq_funi // -dprod1E.
      move/size_dmatrix => /(_ _ _); 1,2: smt(Top.gt0_k Top.gt0_l).
      apply dmatrixRSr1E; 1,2: smt(Top.gt0_k Top.gt0_l).
    case => A t /=; rewrite -(dmap_dprodE _ _ (fun x => x)) !dmap_id supp_dprod /=.
    case => supp_A supp_t. 
    move: (supp_A) => /size_dmatrix /(_ _ _) /=; 1,2: smt(Top.gt0_k Top.gt0_l).
    move: (supp_t) => /size_dvector. rewrite lez_maxr; 1:smt(Top.gt0_k). move => s_t [r_A c_A].
    (* case => /supp_dmatrix_Rqkl /= [[r_A c_A] Rq_A] /supp_dvector_Rqk [s_t Rq_t]. *)
    rewrite r_A c_A s_t /= -{2}r_A -{2}c_A subm_catmcCl /= 1:/#.
    rewrite col_catmcR /= ?r_A ?c_A ?s_t // subrr. 
    rewrite colN oppmK colK /=; apply supp_dmatrix_catmc => //;1,2: smt(Top.gt0_k Top.gt0_l).
    rewrite supp_dmatrix_full ?dRq_fu //; smt(Top.gt0_k Top.gt0_l). 
  call (: ={PRO.RO.m} /\ (forall x, x \in PRO.RO.m => oget PRO.RO.m.[x] \in dC tau){1}).
    proc; inline*; auto => />; smt(get_setE get_set_sameE).
  auto => /> &1 &2 RO_dC r_mA c_mA s_t. split => [|E1 E2]. 
  + rewrite -r_mA -c_mA subm_catmcCl /= 1:/#. 
    rewrite col_catmcR //= 1:/#. 
    by rewrite colN oppmK colK. 
  move => _ _.     
  by rewrite -E1 -E2 /= rows_catmc //=; smt(Top.gt0_k Top.gt0_l).
inline S1(H').verify. sp 5 1. 
if; 1,3: by (try inline*); auto.
inline H'.get. wp. sp 1 1. 
(* need [size z{1} = l] to prove equality of the RO argument *)
case (size z{1} = l /\ inf_normv z{1} < gamma1 - b);
  last by conseq (:_ ==> true); [ smt() | inline*; auto].
call(: ={arg,glob G} /\ (forall x, x \in PRO.RO.m => oget PRO.RO.m.[x] \in dC tau){1} 
       ==> ={res} /\ res{1} \in dC tau).
  proc; inline*; auto => />.  smt(get_set_sameE).
wp; skip => &1 &2. case: (sig0{1}) => // -[? ?]. 
move => />. case. move => /> _. (* why is case needed? *)
(* recover names / definitions *)
move: (mA0{2}) (t{2}) (z{1}) (c{1}) => A t z c.
pose w := (_ - MatRq.(**) _ _). (* FIXME: why is XInt.(**) in scope? *)
pose w1 := highBitsV _. 
pose e := - lowBitsV _.
move => r_mA c_mA size_t size_z normv_z. 
have size_w : size w = k. rewrite size_addv /= size_scalarv /= rows_mulmx /= /#.
have size_e : size e = k by rewrite size_oppv size_lowBitsV.
split => [|? c_dC]; last split.
- rewrite mulmxv_cat.
  + smt(gt0_k). 
  + rewrite cols_catmc /= 1:/# size_catv /=. smt().
  + rewrite rows_catmc /=; smt(). 
  rewrite -size_e mulmx1v mulmxv_cat;  1..3: smt().
  rewrite colmxN colmxc scalarvN.  
  rewrite addvC -sub_eqv 2://; 1: by rewrite size_shiftV size_highBitsV /#.
  by rewrite /w1 /e oppvK high_lowPv. 
- rewrite 2!inf_normv_cat !StdOrder.IntOrder.ltr_maxrP !max_ltrP.
  rewrite normv_z /= 1!inf_normv_vectc //.
  have -> /= : cnorm c < gamma2+1 by smt(cnorm_dC gamma2_bound).
  right. rewrite /e inf_normvN. smt(inf_normv_low gamma2_bound).
- rewrite catvA get_catv_r ?size_catv 1:/#. 
  have -> : k + (l + 1) - 1 - (size e + size z) = 0 by smt().
  by rewrite get_vectc.
qed.

lemma KOA_bound &m : 
     Pr [EF_KOA_RO(SimplifiedDilithium,A,H).main() @ &m : res ] 
  <= `| Pr[ GameL(RedMLWE(A,H)).main() @ &m : res ] - Pr [ GameR(RedMLWE(A,H)).main() @ &m : res ] |
   + Pr [ Game(RedMSIS(A),G).main() @ &m : res]. 
proof.
apply (ler_trans _ _ _ (hop1 &m)); rewrite RField.addrC ler_add2l.
by rewrite hop2 hop3.
qed.

end section PROOF.


(* -- Operator-based -- *)

op keygen : (PK * SK) distr =
  dlet (dmatrix dRq k l) (fun mA =>
  dlet (dvector (dRq_ e) l) (fun s1 =>
  dmap (dvector (dRq_ e) k) (fun s2 =>
  let pk = (mA, mA *^ s1 + s2) in
  let sk = (mA, s1, s2) in
  (pk, sk)))).

op commit (sk : SK) : (commit_t * pstate_t) distr =
  let (mA, s1, s2) = sk in
  dmap (dvector (dRq_ (gamma1 - 1)) l) (fun y =>
  let w1 = highBitsV (mA *^ y) in
  (w1, y)).

op respond (sk : SK) (c : challenge_t) (y: pstate_t) : response_t option =
  let (mA, s1, s2) = sk in
  let z = y + c ** s1 in
  if gamma1 - b <= inf_normv z \/
     gamma2 - b <= inf_normv (lowBitsV (mA *^ y - c ** s2) ) then
    None else
    Some z.

op verify (pk : PK) (w1 : commit_t) (c : challenge_t) (z : response_t) : bool =
  let (mA, t) = pk in
  size z = l /\ 
  inf_normv z < gamma1 - b /\
  w1 = highBitsV (mA *^ z - c ** t).

clone import OpBased with
  op keygen <= keygen,
  op commit <= commit,
  op response <= respond,
  op verify <= verify.
(* TODO proof *. *)

(* Sanity check for matrix/vector dimensions *)
lemma size_t pk sk : (pk,sk) \in keygen => size pk.`2 = k.
proof. 
case/supp_dlet => mA /= [s_mA]. 
case/supp_dlet => s1 /= [s_s1]. 
case/supp_dlet => s2 /= [s_s2].
rewrite /(\o) supp_dunit => -[-> _]. 
rewrite [Vectors.size]lock /= -lock.
rewrite size_addv size_mulmxv;
smt(size_dmatrix size_dvector Top.gt0_k Top.gt0_l).
qed.

module OpBasedSig = IDS_Sig(OpBased.P, OpBased.V).

section OpBasedCorrectness.

declare module H <: Hash {-OpBased.P}.

equiv keygen_opbased_correct :
  OpBasedSig(H).keygen ~ SimplifiedDilithium(H).keygen :
  true ==> ={res}.
proof. 
proc; inline *.
rnd: *0 *0; skip => />; split => [kp ?|_ [pk sk]].
- by rewrite dmap_id /keygen &(mu_eq). 
by rewrite /keygen dmap_id.
qed.

equiv sign_opbased_correct :
  OpBasedSig(H).sign ~ SimplifiedDilithium(H).sign :
  ={arg,glob H} ==> ={res,glob H}.
proof.
proc; inline *. 
while (oz{1} = z{2} /\ ={c,sk,glob H,m} /\ k{1} = ctr{2} /\ (sk = (mA,s1,s2)){2}); 
  last by auto => /> /#.
conseq (: _ ==> ={c, sk, glob H, m} /\ (sk = (mA,s1,s2)){2} 
                 /\ oz{1} = z{2} /\ k{1} = ctr{2}); 1: smt().
seq 4 4 : (#pre /\ w{1} = w1{2} /\ P.pstate{1} = y{2}).
- call(: true). conseq />. 
  rnd: *0 *0. skip => /> &m ?. split => [[y w1] ?|_]. 
  + apply/eq_sym. congr. (* symmetry as hack for RHS pattern selection *)
    by rewrite /commit /= dmap_comp /(\o) /=. 
  move => ?. by rewrite /commit /= dmap_comp /(\o).
conseq />. auto => /> &m ?. split => [|pass_chk]. 
+ by rewrite /respond /= => ->.
+ by rewrite /respond /= ifF.
qed.

equiv verify_opbased_correct :
  OpBasedSig(H).verify ~ SimplifiedDilithium(H).verify :
  ={arg,glob H} ==> ={res,glob H}.
proof.
proc; inline *. 
sp; if; 1,3: by auto => />.
seq 3 3: (#pre /\ ={c, z, w, c'} /\
          w{1} = recover pk{1} c{1} z{1}).
- sp; call (: true); skip => /> ?? H H' ?.
  by rewrite -H in H'; case H' => ?? /#.
if{1}; by auto => />.
qed.

end section OpBasedCorrectness.



(* Main Theorem *)

import FSaCR.DSS.
import FSaCR.DSS.PRO.
import FSaCR.DSS.DS.Stateless.

(* Distinguisher for (RO) signature schemes *)
module type SigDist (S : Scheme) (H : Hash_i) = { 
  proc distinguish() : bool
}.

equiv eqv_code_op (D <: SigDist{-OpBasedSig}) (H <: Hash_i{-P,-D}) : 
  D(SimplifiedDilithium(H),H).distinguish ~ D(OpBasedSig(H),H).distinguish : 
  ={glob D,glob H} ==> ={glob D,glob H,res}.
proof.
proc*; call (: ={glob H}); last done.
- proc true; auto.
- proc true; auto.
- by symmetry; conseq (keygen_opbased_correct H) => //.
- by symmetry; conseq (sign_opbased_correct H) => /#.
- by symmetry; conseq (verify_opbased_correct H) => /#.
qed.

section PROOF.

declare module A <: Adv_EFCMA_RO{-O_CMA_Default,-RO,-OpBasedSig}.

op bound : real. (* TODO *)

(*** Step 1 : Replace the code-based scheme with an operator-based one ***)

(* EF_CMA game seen as a distinshuisher for signature schemes *) 
local module (SD : SigDist) (S : Scheme) (H : Hash_i) = { 
  proc distinguish() = {
    var r;
    H.init();
    r <@ EF_CMA(S,A(H),O_CMA_Default).main();
    return r;
  }
}.

local lemma pr_code_op &m : 
  Pr [ EF_CMA_RO(SimplifiedDilithium, A, RO,O_CMA_Default).main() @ &m : res ] = 
  Pr [ EF_CMA_RO(OpBasedSig, A, RO,O_CMA_Default).main() @ &m : res ].
proof.
byequiv (_: ={glob A,glob RO,glob O_CMA_Default} ==> ={res}) => //.
transitivity SD(SimplifiedDilithium(RO),RO).distinguish 
   (={glob A,glob RO,glob O_CMA_Default} ==> ={res}) 
   (={glob A,glob RO,glob O_CMA_Default} ==> ={res}); 
   [smt()| smt() | by sim |].
transitivity SD(OpBasedSig(RO),RO).distinguish 
   (={glob A,glob RO,glob O_CMA_Default} ==> ={res}) 
   (={glob A,glob RO,glob O_CMA_Default} ==> ={res}); 
   [smt()| smt() | | by sim].
by conseq (eqv_code_op SD RO).
qed.

(*** Step 2 : Reduce to the case for general (i.e., not commitment recoverable schemes) ***)

(* TODO: this should be local, but for this all axioms need to be proved *)
clone Generic as FSaG with
  op kappa <- kappa,
  op qS <- qS,
  op qH <- qH + qS.

(* clone import FSa_CRtoGen as CG with  *)
(*   theory FSa <- OpFSA. *)
(*   theory FSaG <- FSaG. *)

lemma SimplifiedDilithium_secure &m : 
  Pr [ EF_CMA_RO(SimplifiedDilithium, A, RO,O_CMA_Default).main() @ &m : res ] <= bound.
proof.
(* Step 1 *)
rewrite pr_code_op.
admitted.

end section PROOF.
