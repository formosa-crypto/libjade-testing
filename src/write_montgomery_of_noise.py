dilithium_N = (1 << 23) - (1 << 13) + 1

def montgomery(x):
	return (x << 32) % dilithium_N

print("u32[9] montgomery_from_eta_minus_x = {")
for i in range(4, -4, -1):
	print(f"\t{montgomery(i)},")
print(f"\t{montgomery(4)}")
print("};")
