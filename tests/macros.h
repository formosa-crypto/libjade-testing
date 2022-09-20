#define PRINT(X) cout << (#X) << " = " << (X) << endl

#if DILITHIUM_MODE == 2

#define SEEDED_KEYGEN_REF pqcrystals_dilithium2_ref_seeded_keypair
#define SEEDED_KEYGEN_JAZZ dilithium2_keygen_jazz
#define KEYGEN_REF pqcrystals_dilithium2_ref_keypair
#define SIGN_REF pqcrystals_dilithium2_ref_signature
#define SIGN_JAZZ dilithium2_sign_jazz
#define PUBLICKEYBYTES pqcrystals_dilithium2_PUBLICKEYBYTES
#define SECRETKEYBYTES pqcrystals_dilithium2_SECRETKEYBYTES

#define POLYT1_PACK_REF pqcrystals_dilithium2_ref_polyt1_pack
#define POLYT1_UNPACK_REF pqcrystals_dilithium2_ref_polyt1_unpack
#define POLYZ_PACK pqcrystals_dilithium2_ref_polyz_pack

#elif DILITHIUM_MODE == 3

#define SEEDED_KEYGEN_REF pqcrystals_dilithium3_ref_seeded_keypair
#define SEEDED_KEYGEN_JAZZ dilithium3_keygen_jazz
#define KEYGEN_REF pqcrystals_dilithium3_ref_keypair
#define SIGN_REF pqcrystals_dilithium3_ref_signature
#define SIGN_JAZZ dilithium3_sign_jazz
#define PUBLICKEYBYTES pqcrystals_dilithium3_PUBLICKEYBYTES
#define SECRETKEYBYTES pqcrystals_dilithium3_SECRETKEYBYTES

#define POLYT1_PACK_REF pqcrystals_dilithium3_ref_polyt1_pack
#define POLYT1_UNPACK_REF pqcrystals_dilithium3_ref_polyt1_unpack
#define POLYZ_PACK pqcrystals_dilithium3_ref_polyz_pack

#elif DILITHIUM_MODE == 5

#define SEEDED_KEYGEN_REF pqcrystals_dilithium5_ref_seeded_keypair
#define SEEDED_KEYGEN_JAZZ dilithium5_keygen_jazz
#define KEYGEN_REF pqcrystals_dilithium5_ref_keypair
#define SIGN_REF pqcrystals_dilithium5_ref_signature
#define SIGN_JAZZ dilithium5_sign_jazz
#define PUBLICKEYBYTES pqcrystals_dilithium5_PUBLICKEYBYTES
#define SECRETKEYBYTES pqcrystals_dilithium5_SECRETKEYBYTES

#define POLYT1_PACK_REF pqcrystals_dilithium5_ref_polyt1_pack
#define POLYT1_UNPACK_REF pqcrystals_dilithium5_ref_polyt1_unpack
#define POLYZ_PACK pqcrystals_dilithium5_ref_polyz_pack

#else

#error "DILITHIUM_MODE undefined"

#endif