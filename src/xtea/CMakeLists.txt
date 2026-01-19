add_benchmark(
    NAME xtea-encrypt
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/encrypt.cpp"
)


add_benchmark(
    NAME xtea-decrypt
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/decrypt.cpp"
)
