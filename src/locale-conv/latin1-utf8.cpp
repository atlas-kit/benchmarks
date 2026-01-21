#include "locale-conv.h"

#include <benchmark/benchmark.h>
#include <boost/locale.hpp>
#include <simdutf.h>

namespace {

void Boost_Latin1_to_UTF8(benchmark::State& state)
{
	const std::string latin1_data = generate_latin1_data(state.range(0));

	for (auto _ : state) {
		auto utf8 = boost::locale::conv::from_utf(latin1_data, "ISO-8859-1");
		benchmark::DoNotOptimize(utf8);
	}
	state.SetBytesProcessed(state.range(0) * state.iterations());
}
BM(Boost_Latin1_to_UTF8);

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
BM(SimdUTF_Latin1_to_UTF8);

} // namespace
