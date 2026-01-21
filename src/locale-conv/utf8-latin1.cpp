#include "locale-conv.h"

#include <benchmark/benchmark.h>
#include <boost/locale.hpp>
#include <simdutf.h>

namespace {

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
BM(Boost_UTF8_to_Latin1);

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
BM(SimdUTF_UTF8_to_Latin1);

} // namespace
