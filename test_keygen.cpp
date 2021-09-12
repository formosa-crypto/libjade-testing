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
	void keygen_jazz(char pk[pk_len], char sk[sk_len], char randomness[32]);
}

uint8_t sampleByte() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> distrib(0,  255);
	return distrib(gen);
}

int main() {
	char pk[pk_len];
	char sk[sk_len];
	char randomness[32];
	for(int i = 0; i < 32; ++i) {
		randomness[i] = sampleByte();
	}

	keygen_jazz(pk, sk, randomness);

	PRINT(int(pk[1]));
	PRINT(int(sk[1]));

	return 0;
}
