#include <benchmark/benchmark.h>
#include <boost/locale.hpp>
#include <random>
#include <simdutf.h>

namespace {

// Generate random Latin1 input string
std::string generate_latin1_data(size_t size)
{
	std::string result(size, '\0');
	std::mt19937 gen(123);
	std::uniform_int_distribution<> dist(0x00, 0xFF);

	for (auto& ch : result) ch = static_cast<char>(dist(gen));

	return result;
}

void Boost_Latin1_to_UTF8(benchmark::State& state)
{
	const std::string latin1_data = generate_latin1_data(state.range(0));

	for (auto _ : state) {
		auto utf8 = boost::locale::conv::from_utf(latin1_data, "ISO-8859-1");
		benchmark::DoNotOptimize(utf8);
	}
	state.SetBytesProcessed(state.range(0) * state.iterations());
}
BENCHMARK(Boost_Latin1_to_UTF8)->RangeMultiplier(4)->Range(16, 4096);

void SimdUTF_Latin1_to_UTF8(benchmark::State& state)
{
	const std::string latin1_data = generate_latin1_data(state.range(0));
	auto stringLen = simdutf::utf8_length_from_latin1(latin1_data.data(), latin1_data.size());

	std::string output(stringLen, '\0');
	for (auto _ : state) {
		size_t len =
		    simdutf::convert_latin1_to_utf8_safe(latin1_data.data(), latin1_data.size(), output.data(), stringLen);
		benchmark::DoNotOptimize(len);
	}
	state.SetBytesProcessed(state.range(0) * state.iterations());
}
BENCHMARK(SimdUTF_Latin1_to_UTF8)->RangeMultiplier(4)->Range(16, 4096);

void Boost_UTF8_to_Latin1(benchmark::State& state)
{
	const std::string latin1_data = generate_latin1_data(state.range(0));

	const auto utf8 = boost::locale::conv::from_utf(latin1_data, "ISO-8859-1");
	for (auto _ : state) {
		auto latin1 = boost::locale::conv::to_utf<char>(utf8, "ISO-8859-1");
		benchmark::DoNotOptimize(latin1);
	}
	state.SetBytesProcessed(state.range(0) * state.iterations());
}
BENCHMARK(Boost_UTF8_to_Latin1)->RangeMultiplier(4)->Range(16, 4096);

void SimdUTF_UTF8_to_Latin1(benchmark::State& state)
{
	const std::string latin1_data = generate_latin1_data(state.range(0));
	std::vector<char> utf8_data;
	utf8_data.resize(latin1_data.size() * 2);

	auto stringLen = simdutf::latin1_length_from_utf8(utf8_data.data(), utf8_data.size());

	std::string output(stringLen, '\0');
	for (auto _ : state) {
		size_t len = simdutf::convert_utf8_to_latin1(utf8_data.data(), utf8_data.size(), output.data());
		benchmark::DoNotOptimize(len);
	}
	state.SetBytesProcessed(state.range(0) * state.iterations());
}
BENCHMARK(SimdUTF_UTF8_to_Latin1)->RangeMultiplier(4)->Range(16, 4096);

} // namespace
