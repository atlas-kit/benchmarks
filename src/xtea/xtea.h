#pragma once

#include <array>
#include <cstdint>

namespace xtea {

inline constexpr uint32_t delta = 0x9e3779b9;

using key = std::array<uint32_t, 4>;
using round_keys = std::array<uint32_t, 64>;
using round_keys_v2 = std::array<std::pair<uint32_t, uint32_t>, 32>;

constexpr round_keys expand_key(const key& k)
{
	round_keys expanded = {};

	for (uint32_t i = 0, sum = 0, next_sum = sum + delta; i < expanded.size();
	     i += 2, sum = next_sum, next_sum += delta) {
		expanded[i] = sum + k[sum & 3];
		expanded[i + 1] = next_sum + k[(next_sum >> 11) & 3];
	}

	return expanded;
}

constexpr round_keys_v2 expand_key_v2(const key& k)
{
	round_keys_v2 expanded = {};

	for (uint32_t i = 0, sum = 0, next_sum = sum + delta; i < expanded.size(); ++i, sum = next_sum, next_sum += delta) {
		expanded[i] = std::make_pair(sum + k[sum & 3], next_sum + k[(next_sum >> 11) & 3]);
	}

	return expanded;
}

inline constexpr auto max_length = 24590u;
inline constexpr auto deadbeef = xtea::key{0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF};

} // namespace xtea

#define BM(x) BENCHMARK(x)->Range(8, xtea::max_length)->UseManualTime();
