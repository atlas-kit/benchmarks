find_package(Boost REQUIRED COMPONENTS locale)
find_package(simdutf CONFIG REQUIRED)

add_benchmark(NAME latin1-utf8 SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/latin1-utf8.cpp" LINKS Boost::locale simdutf::simdutf)
add_benchmark(NAME utf8-latin1 SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/utf8-latin1.cpp" LINKS Boost::locale simdutf::simdutf)
