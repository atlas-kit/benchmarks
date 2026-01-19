#include "xtea.h"

#include <benchmark/benchmark.h>
#include <chrono>
#include <cstring>
#include <ranges>

namespace {

void xtea_decrypt(uint8_t* data, std::size_t length, const xtea::key& k)
{
	for (auto it = data, last = data + length; it < last; it += 8) {
		uint32_t v0, v1;
		std::memcpy(&v0, it, 4);
		std::memcpy(&v1, it + 4, 4);

		for (uint32_t i = 0u, sum = xtea::delta * 32u; i < 32u; ++i) {
			v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[(sum >> 11) & 3]);
			sum -= xtea::delta;
			v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
		}

		std::memcpy(it, &v0, 4);
		std::memcpy(it + 4, &v1, 4);
	}
}

void xtea_decrypt_interleaved(uint8_t* data, std::size_t length, const xtea::key& k)
{
	for (uint32_t i = 0u, sum = xtea::delta * 32u; i < 32u; ++i) {
		for (auto it = data, last = data + length; it < last; it += 8) {
			uint32_t v0, v1;
			std::memcpy(&v0, it, 4);
			std::memcpy(&v1, it + 4, 4);

			v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[(sum >> 11) & 3]);
			sum -= xtea::delta;
			v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);

			std::memcpy(it, &v0, 4);
			std::memcpy(it + 4, &v1, 4);
		}
	}
}

// always slower than interleaved
// void xtea_decrypt_precomputed(uint8_t* data, std::size_t length, const xtea::round_keys& k)
// {
// 	for (auto it = data, last = data + length; it < last; it += 8) {
// 		uint32_t v0, v1;
// 		std::memcpy(&v0, it, 4);
// 		std::memcpy(&v1, it + 4, 4);

// 		for (auto i = k.size(); i > 0u; i -= 2u) {
// 			v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ k[i - 1];
// 			v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ k[i - 2];
// 		}

// 		std::memcpy(it, &v0, 4);
// 		std::memcpy(it + 4, &v1, 4);
// 	}
// }

void xtea_decrypt_interleaved_precomputed(uint8_t* data, std::size_t length, const xtea::round_keys& k)
{
	for (auto i = k.size(); i > 0u; i -= 2u) {
		for (auto it = data, last = data + length; it < last; it += 8) {
			uint32_t v0, v1;
			std::memcpy(&v0, it, 4);
			std::memcpy(&v1, it + 4, 4);

			v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ k[i - 1];
			v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ k[i - 2];

			std::memcpy(it, &v0, 4);
			std::memcpy(it + 4, &v1, 4);
		}
	}
}

// always slower than interleaved
// void xtea_decrypt_keypair(uint8_t* data, size_t length, const xtea::round_keys_v2& k)
// {
// 	for (auto it = data, last = data + length; it < last; it += 8) {
// 		uint32_t v0, v1;
// 		std::memcpy(&v0, it, 4);
// 		std::memcpy(&v1, it + 4, 4);

// 		for (auto&& [k0, k1] : k | std::views::reverse) {
// 			v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ k1;
// 			v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ k0;
// 		}

// 		std::memcpy(it, &v0, 4);
// 		std::memcpy(it + 4, &v1, 4);
// 	}
// }

void xtea_decrypt_interleaved_keypair(uint8_t* data, size_t length, const xtea::round_keys_v2& k)
{
	for (auto&& [k0, k1] : k | std::views::reverse) {
		for (auto it = data, last = data + length; it < last; it += 8) {
			uint32_t v0, v1;
			std::memcpy(&v0, it, 4);
			std::memcpy(&v1, it + 4, 4);

			v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ k1;
			v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ k0;

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
	fn(c.data(), c.size(), key);

	if (c != m) {
		state.SkipWithError("Failed to decrypt");
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

void decrypt(benchmark::State& state) { bench(xtea_decrypt, xtea::deadbeef, state); }
BM(decrypt);

void decrypt_interleaved(benchmark::State& state) { bench(xtea_decrypt_interleaved, xtea::deadbeef, state); }
BM(decrypt_interleaved);

// void decrypt_precomputed(benchmark::State& state)
// {
// 	auto ek0 = xtea::expand_key(xtea::deadbeef);
// 	bench(xtea_decrypt_precomputed, ek0, state);
// }
// BM(decrypt_precomputed);

void decrypt_interleaved_precomputed(benchmark::State& state)
{
	auto ek0 = xtea::expand_key(xtea::deadbeef);
	bench(xtea_decrypt_interleaved_precomputed, ek0, state);
}
BM(decrypt_interleaved_precomputed);

// void decrypt_keypair(benchmark::State& state)
// {
// 	auto ek0 = xtea::expand_key_v2(xtea::deadbeef);
// 	bench(xtea_decrypt_keypair, ek0, state);
// }
// BM(decrypt_keypair);

void decrypt_interleaved_keypair(benchmark::State& state)
{
	auto ek0 = xtea::expand_key_v2(xtea::deadbeef);
	bench(xtea_decrypt_interleaved_keypair, ek0, state);
}
BM(decrypt_interleaved_keypair);

} // namespace
