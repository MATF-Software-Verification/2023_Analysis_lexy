cmake_minimum_required(VERSION 3.16)

project(MafijaLibFuzzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# Putanja do Mafija projekta
# ============================================================

set(MAFIJA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Mafija")


# ============================================================
# Qt
# ============================================================

find_package(Qt6 REQUIRED COMPONENTS
    Core
)


# ============================================================
# Fuzz target - protocol
# ============================================================

add_executable(fuzz_protocol
    fuzz_protocol.cpp

    "${MAFIJA_DIR}/server_client/Shared/protocol.cpp"
)


target_include_directories(fuzz_protocol
    PRIVATE
        "${MAFIJA_DIR}/server_client/Shared"
)


target_link_libraries(fuzz_protocol
    PRIVATE
        Qt6::Core
)


# ============================================================
# LibFuzzer + sanitizers
# ============================================================

target_compile_options(fuzz_protocol
    PRIVATE
        -g
        -O1
        -fno-omit-frame-pointer
        -fsanitize=fuzzer,address,undefined
)


target_link_options(fuzz_protocol
    PRIVATE
        -fsanitize=fuzzer,address,undefined
)
