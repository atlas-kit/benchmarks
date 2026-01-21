#pragma once

#include <random>
#include <string>

// Generate random Latin1 input string
std::string generate_latin1_data(size_t size)
{
	std::string result(size, '\0');
	std::mt19937 gen(123);
	std::uniform_int_distribution<> dist(0x00, 0xFF);

	for (auto& ch : result) ch = static_cast<char>(dist(gen));

	return result;
}

#define BM(x) BENCHMARK(x)->RangeMultiplier(4)->Range(16, 4096);
