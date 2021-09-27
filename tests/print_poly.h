#include <iostream>

using std::cout;
using std::endl;

template<typename T> void print_poly(T* f) {
	for(int i = 0; i < 16; ++i) {
		cout << f[16 * i];
		for(int j = 1; j < 16; ++j) {
			cout << ' ' << f[16 * i + j];
		}
		cout << endl;
	}
}
