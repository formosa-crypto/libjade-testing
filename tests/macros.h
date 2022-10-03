#define PRINT(X) cout << (#X) << " = " << (X) << endl

// TODO: We can remove all the ref macros, because they are already defined
// in the respective Dilithium headers

#if DILITHIUM_MODE == 2

#define SEEDED_KEYGEN_REF pqcrystals_dilithium2_ref_seeded_keypair
#define SEEDED_KEYGEN_JAZZ dilithium2_keygen_jazz
#define KEYGEN_REF pqcrystals_dilithium2_ref_keypair
#define SIGN_REF pqcrystals_dilithium2_ref_signature
#define SIGN_JAZZ dilithium2_sign_jazz
#define PUBLICKEYBYTES pqcrystals_dilithium2_PUBLICKEYBYTES
#define SECRETKEYBYTES pqcrystals_dilithium2_SECRETKEYBYTES
#define DECOMPOSE_REF pqcrystals_dilithium2_ref_decompose
#define DECOMPOSE_JAZZ decompose_gamma2_88_jazz
#define POLY_UNIFORM_GAMMA1_REF pqcrystals_dilithium2_ref_poly_uniform_gamma1
#define POLY_UNIFORM_GAMMA1_JAZZ expandMask_poly_gamma1_217_jazz
#define POLY_ETA_PACK_JAZZ polyeta_2_pack_jazz
#define POLY_ETA_UNPACK_JAZZ polyeta_2_unpack_jazz
#define POLY_Z_PACK_JAZZ polyz_pack_gamma1_217_jazz

#elif DILITHIUM_MODE == 3

#define SEEDED_KEYGEN_REF pqcrystals_dilithium3_ref_seeded_keypair
#define SEEDED_KEYGEN_JAZZ dilithium3_keygen_jazz
#define KEYGEN_REF pqcrystals_dilithium3_ref_keypair
#define SIGN_REF pqcrystals_dilithium3_ref_signature
#define SIGN_JAZZ dilithium3_sign_jazz
#define PUBLICKEYBYTES pqcrystals_dilithium3_PUBLICKEYBYTES
#define SECRETKEYBYTES pqcrystals_dilithium3_SECRETKEYBYTES
#define DECOMPOSE_REF pqcrystals_dilithium3_ref_decompose
#define DECOMPOSE_JAZZ decompose_gamma2_32_jazz
#define POLY_UNIFORM_GAMMA1_REF pqcrystals_dilithium3_ref_poly_uniform_gamma1
#define POLY_UNIFORM_GAMMA1_JAZZ expandMask_poly_gamma1_219_jazz
#define POLY_ETA_PACK_JAZZ polyeta_4_pack_jazz
#define POLY_ETA_UNPACK_JAZZ polyeta_4_unpack_jazz
#define POLY_Z_PACK_JAZZ polyz_pack_gamma1_219_jazz


#elif DILITHIUM_MODE == 5

#define SEEDED_KEYGEN_REF pqcrystals_dilithium5_ref_seeded_keypair
#define SEEDED_KEYGEN_JAZZ dilithium5_keygen_jazz
#define KEYGEN_REF pqcrystals_dilithium5_ref_keypair
#define SIGN_REF pqcrystals_dilithium5_ref_signature
#define SIGN_JAZZ dilithium5_sign_jazz
#define PUBLICKEYBYTES pqcrystals_dilithium5_PUBLICKEYBYTES
#define SECRETKEYBYTES pqcrystals_dilithium5_SECRETKEYBYTES
#define DECOMPOSE_REF pqcrystals_dilithium5_ref_decompose
#define DECOMPOSE_JAZZ decompose_gamma2_32_jazz
#define POLY_UNIFORM_GAMMA1_REF pqcrystals_dilithium5_ref_poly_uniform_gamma1
#define POLY_UNIFORM_GAMMA1_JAZZ expandMask_poly_gamma1_219_jazz
#define POLY_ETA_PACK_JAZZ polyeta_2_pack_jazz
#define POLY_ETA_UNPACK_JAZZ polyeta_2_unpack_jazz
#define POLY_Z_PACK_JAZZ polyz_pack_gamma1_219_jazz


#else

#error "DILITHIUM_MODE undefined"

#endif