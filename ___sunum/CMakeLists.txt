CMAKE_MINIMUM_REQUIRED (VERSION 3.0)

project(sunum_chainthings)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DDEBUG_MSG)
add_definitions(-DENABLE_FILE_LOGGING)
add_definitions(-DDEBUG_CLIENT_SEND_PING)
add_definitions(-D_DEBUG)

set(P2P_DIR ${CMAKE_SOURCE_DIR}/p2p-network)
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

set(LIB_UUID /usr/lib/arm-linux-gnueabihf/libuuid.a)

include_directories(${P2P_DIR}/include)
include_directories(${EXTERNAL_DIR}/asio/include)
include_directories(${EXTERNAL_DIR}/cxxopts/include)

set(P2P_NETWORK_SOURCES
    p2p-network/include/impl/client_impl.cpp
    p2p-network/include/impl/server_impl.cpp
    p2p-network/src/Logging/logger.cpp
    p2p-network/src/Logging/spdlog/fmt/bundled/format.cc
    p2p-network/src/Logging/spdlog/fmt/bundled/ostream.cc
    p2p-network/src/Logging/spdlog/fmt/bundled/posix.cc
    p2p-network/src/Logging/spdlog/fmt/bundled/printf.cc
    p2p-network/src/peer.cpp
    p2p-network/src/peer_list.cpp
    p2p-network/src/protocol.cpp
    p2p-network/src/message.cpp
    p2p-network/src/uuid.cpp
)

add_subdirectory(external/cxxopts)

add_library(p2p-network
	STATIC
	${P2P_NETWORK_SOURCES})

IF (UNIX)
	target_link_libraries(p2p-network -pthread)
	target_link_libraries(p2p-network ${LIB_UUID})
ENDIF ()

add_executable(sunum
	main.cpp
	default_test.cpp
	sunum.cpp
	vm.cpp
	base58.cpp)

IF (UNIX)
	target_link_libraries(sunum -pthread)
	target_link_libraries(sunum ${LIB_UUID})
	target_link_libraries(sunum -lwiringPi)
ENDIF ()

target_link_libraries(sunum p2p-network)