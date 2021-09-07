#include <iostream>
#include <random>

using std::cout;
using std::endl;
using std::vector;

#define PRINT(X) cout << (#X) << " = " << (X) << endl

//constexpr const int poly_deg = 256;

constexpr const int pk_len = 1952;
constexpr const int sk_len = 4016;

extern "C" {
	//uint64_t montgomery_REDC(uint64_t mx);
	//uint64_t montgomery_mult(uint64_t mx, uint64_t my);
	//64 bits int only for pointer params...? Why?
	//untested fft; this probably ain't gonna work...
	//void fft(uint64_t f[poly_deg], uint64_t precomputed_mroots[256]);
	void keygen_jazz(char pk[pk_len], char sk[sk_len]);
}

int main() {
	char pk[pk_len];
	char sk[sk_len];
	keygen_jazz(pk, sk);
	PRINT(int(pk[0]));
	PRINT(int(pk[1]));
	PRINT(int(pk[2]));
	//PRINT(int(pk[3]));
	PRINT(int(pk[pk_len - 1]));
	return 0;
}
