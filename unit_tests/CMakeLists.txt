cmake_minimum_required(VERSION 3.16)

project(MafijaQTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


# ============================================================
# Putanja do Mafija projekta
# ============================================================

set(MAFIJA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Mafija")


# ============================================================
# Qt
# ============================================================

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS
    Core
    Network
    Widgets
    Test
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Core
    Network
    Widgets
    Test
)


# ============================================================
# Mafija - game
# ============================================================

add_subdirectory(
    "${MAFIJA_DIR}/game"
    "${CMAKE_CURRENT_BINARY_DIR}/game"
)


# ============================================================
# Mafija - server_client
#
# Ovim uključujemo:
#
#   Shared
#   Server
#   Client
#
# zajedno sa njihovim međusobnim zavisnostima.
# ============================================================

add_subdirectory(
    "${MAFIJA_DIR}/server_client"
    "${CMAKE_CURRENT_BINARY_DIR}/server_client"
)


# ============================================================
# QTest - VoteCounter
# ============================================================

add_executable(qtest_votecounter
    qtest/qtest_votecounter.cpp
)

target_include_directories(qtest_votecounter
    PRIVATE
        "${MAFIJA_DIR}/server_client/Shared"
)

target_link_libraries(qtest_votecounter
    PRIVATE
        Shared
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
)


# ============================================================
# QTest - VotingRound
# ============================================================

add_executable(qtest_votinground
    qtest/qtest_votinground.cpp
)

target_include_directories(qtest_votinground
    PRIVATE
        "${MAFIJA_DIR}/server_client/Shared"
        "${MAFIJA_DIR}/game"
)

target_link_libraries(qtest_votinground
    PRIVATE
        Shared
        game

        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
)


# ============================================================
# QTest - Protocol
# ============================================================

add_executable(qtest_protocol
    qtest/qtest_protocol.cpp
)

target_include_directories(qtest_protocol
    PRIVATE
        "${MAFIJA_DIR}/server_client/Shared"
)

target_link_libraries(qtest_protocol
    PRIVATE
        Shared
        game

        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
)


# ============================================================
# QTest - ChatClient
# ============================================================

add_executable(qtest_chatclient
    qtest/qtest_chatclient.cpp
)

target_include_directories(qtest_chatclient
    PRIVATE
        "${MAFIJA_DIR}/server_client/Client"
        "${MAFIJA_DIR}/server_client/Shared"
        "${MAFIJA_DIR}/game"
)

target_link_libraries(qtest_chatclient
    PRIVATE
        Client
        Shared
        game

        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
)

# ============================================================
# QTest - VotingServer
# ============================================================

add_executable(qtest_votingserver
    qtest/qtest_votingserver.cpp
)

target_include_directories(qtest_votingserver
    PRIVATE
        "${MAFIJA_DIR}/server_client/Server"
        "${MAFIJA_DIR}/server_client/Shared"
        "${MAFIJA_DIR}/game"
)

target_link_libraries(qtest_votingserver
    PRIVATE
        Server
        Shared
        game
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
)



# ============================================================
# QTest - ChatServer
# ============================================================

add_executable(qtest_chatserver
    qtest/qtest_chatserver.cpp
)

target_include_directories(qtest_chatserver
    PRIVATE
        "${MAFIJA_DIR}/server_client/Server"
        "${MAFIJA_DIR}/server_client/Shared"
        "${MAFIJA_DIR}/game"
)

target_link_libraries(qtest_chatserver
    PRIVATE
        Server
        Client
        Shared
        game

        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
)



# ============================================================
# QTest - GameState
# ============================================================

add_executable(qtest_gamestate
    qtest/qtest_gamestate.cpp
)

target_include_directories(qtest_gamestate
    PRIVATE
        "${MAFIJA_DIR}/game"
)

target_link_libraries(qtest_gamestate
    PRIVATE
        game

        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
)


# ============================================================
# CTest
# ============================================================

enable_testing()

add_test(
    NAME qtest_votecounter
    COMMAND qtest_votecounter
)

add_test(
    NAME qtest_votinground
    COMMAND qtest_votinground
)

add_test(
    NAME qtest_chatserver
    COMMAND qtest_chatserver
)

add_test(
    NAME qtest_protocol
    COMMAND qtest_protocol
)

add_test(
    NAME qtest_votingserver
    COMMAND qtest_votingserver
)

add_test(
    NAME qtest_chatclient
    COMMAND qtest_chatclient
)


add_test(
    NAME qtest_gamestate
    COMMAND qtest_gamestate
)
