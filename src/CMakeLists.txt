find_package(benchmark CONFIG REQUIRED)

function (add_benchmark)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES INCLUDES LINKS)
    cmake_parse_arguments(PARSE_ARGV 0 arg "" "${oneValueArgs}" "${multiValueArgs}")

    add_executable(${arg_NAME} ${arg_SOURCES})
    target_include_directories(${arg_NAME} PRIVATE ${arg_INCLUDES})
    target_link_libraries(${arg_NAME} PRIVATE benchmark::benchmark benchmark::benchmark_main ${arg_LINKS})
endfunction ()

add_subdirectory(locale-conv)
add_subdirectory(xtea)
