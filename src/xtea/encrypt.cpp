#include "xtea.h"

#include <benchmark/benchmark.h>
#include <chrono>
#include <cstring>

namespace {

void encrypt(uint8_t* data, size_t length, const key& k)
{
	for (auto it = data, last = data + length; it < last; it += 8) {
		uint32_t v0, v1;
		std::memcpy(&v0, it, 4);
		std::memcpy(&v1, it + 4, 4);

		for (uint32_t i = 0u, sum = 0u; i < 32u; ++i) {
			v0 += ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
			sum += delta;
			v1 += ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[(sum >> 11) & 3]);
		}

		std::memcpy(it, &v0, 4);
		std::memcpy(it + 4, &v1, 4);
	}
}

void encrypt_interleaved(uint8_t* data, size_t length, const key& k)
{
	for (uint32_t i = 0u, sum = 0u; i < 32u; ++i) {
		for (auto it = data, last = data + length; it < last; it += 8) {
			uint32_t v0, v1;
			std::memcpy(&v0, it, 4);
			std::memcpy(&v1, it + 4, 4);

			v0 += ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
			sum += delta;
			v1 += ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[(sum >> 11) & 3]);

			std::memcpy(it, &v0, 4);
			std::memcpy(it + 4, &v1, 4);
		}
	}
}

void encrypt_precomputed(uint8_t* data, size_t length, const round_keys& k)
{
	for (auto i = 0u; i < k.size(); i += 2u) {
		for (auto it = data, last = data + length; it < last; it += 8) {
			uint32_t v0, v1;
			std::memcpy(&v0, it, 4);
			std::memcpy(&v1, it + 4, 4);

			v0 += ((v1 << 4 ^ v1 >> 5) + v1) ^ k[i];
			v1 += ((v0 << 4 ^ v0 >> 5) + v0) ^ k[i + 1];

			std::memcpy(it, &v0, 4);
			std::memcpy(it + 4, &v1, 4);
		}
	}
}

void encrypt_keypair(uint8_t* data, size_t length, const round_keys_v2& k)
{
	for (auto&& [k0, k1] : k) {
		for (auto it = data, last = data + length; it < last; it += 8) {
			uint32_t v0, v1;
			std::memcpy(&v0, it, 4);
			std::memcpy(&v1, it + 4, 4);

			v0 += ((v1 << 4 ^ v1 >> 5) + v1) ^ k0;
			v1 += ((v0 << 4 ^ v0 >> 5) + v0) ^ k1;

			std::memcpy(it, &v0, 4);
			std::memcpy(it + 4, &v1, 4);
		}
	}
}

template <class Fn, class Key>
static void bench(Fn fn, Key key, benchmark::State& state)
{
	std::array<uint8_t, 8> m = {0xEF, 0xBE, 0xAD, 0xDE, 0xEF, 0xBE, 0xAD, 0xDE},
	                       c = {0xB5, 0x8C, 0xF2, 0xFA, 0xE0, 0xC0, 0x40, 0x09};
	fn(m.data(), m.size(), key);

	if (c != m) {
		state.SkipWithError("Failed to encrypt");
	}

	auto length = state.range(0) & ~7u;

	for (auto _ : state) {
		auto p1 = std::make_unique<uint8_t[]>(length);
		std::generate_n(p1.get(), length, std::rand);

		auto start = std::chrono::high_resolution_clock::now();
		fn(p1.get(), length, key);
		auto end = std::chrono::high_resolution_clock::now();

		benchmark::DoNotOptimize(p1);

		auto elapsed = duration_cast<std::chrono::duration<double>>(end - start);
		state.SetIterationTime(elapsed.count());
	}

	state.SetBytesProcessed(length * state.iterations());
}

void XTEA_Encrypt(benchmark::State& state) { bench(encrypt, deadbeef, state); }
BM(XTEA_Encrypt);

void XTEA_Encrypt_Interleaved(benchmark::State& state) { bench(encrypt_interleaved, deadbeef, state); }
BM(XTEA_Encrypt_Interleaved);

void XTEA_Encrypt_Precomputed_Key(benchmark::State& state)
{
	auto ek0 = expand_key(deadbeef);
	bench(encrypt_precomputed, ek0, state);
}
BM(XTEA_Encrypt_Precomputed_Key);

void XTEA_Encrypt_Keypair(benchmark::State& state)
{
	auto ek1 = expand_key_v2(deadbeef);
	bench(encrypt_keypair, ek1, state);
}
BM(XTEA_Encrypt_Keypair);

} // namespace
