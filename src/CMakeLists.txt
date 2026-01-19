find_package(benchmark CONFIG REQUIRED)

function (add_benchmark)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES INCLUDES LINKS)
    cmake_parse_arguments(PARSE_ARGV 0 arg "" "${oneValueArgs}" "${multiValueArgs}")

    add_executable(${arg_NAME} ${arg_SOURCES})
    target_include_directories(${arg_NAME} PRIVATE ${arg_INCLUDES})
    target_link_libraries(${arg_NAME} PRIVATE benchmark::benchmark benchmark::benchmark_main ${arg_LINKS})
endfunction ()

find_package(Boost REQUIRED COMPONENTS locale)
find_package(simdutf CONFIG REQUIRED)
add_benchmark(
    NAME convert-latin1-utf8
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/convert-latin1-utf8.cpp"
    LINKS Boost::locale simdutf::simdutf
)
