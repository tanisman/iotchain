CMAKE_MINIMUM_REQUIRED (VERSION 3.0)

OPTION (HUNTER_USE_CACHE_SERVERS "Use cached external libs (increases compile time)" ON)

INCLUDE ("cmake/HunterGate.cmake")

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.19.232.tar.gz"
    SHA1 "a412c45fe4c5a72fed386f62dd8d753bd4fd3d11"
    LOCAL
)

PROJECT (IoTChain)

SET (CMAKE_CXX_STANDARD 14)
SET (CMAKE_CXX_STANDARD_REQUIRED ON)

OPTION (ENABLE_DEBUG_MSG "Print debug log of network messages." ON)
OPTION (ENABLE_FILE_LOGGING "Enable log to file." ON)
OPTION (ENABLE_DEBUG_CLIENT_SEND_PING "Enable ping-pong msg after connected to peer." ON)
OPTION (DEFINE_DEBUG "Enables misc debug options" OFF)
OPTION (BUILD_FOR_RP3 "Build for Raspberry Pi3" OFF)

IF (ENABLE_DEBUG_MSG)
	ADD_DEFINITIONS(-DDEBUG_MSG)
ENDIF ()

IF (ENABLE_FILE_LOGGING)
	ADD_DEFINITIONS(-DENABLE_FILE_LOGGING)
ENDIF ()

IF (ENABLE_DEBUG_CLIENT_SEND_PING)
	ADD_DEFINITIONS(-DDEBUG_CLIENT_SEND_PING)
ENDIF ()

IF (DEFINE_DEBUG)
	ADD_DEFINITIONS(-D_DEBUG)
ENDIF ()

IF (BUILD_FOR_RP3)
	ADD_DEFINITIONS(-DBUILD_RP3)
ENDIF ()

#set up some important folders
SET (BLOCKCHAIN_DIR ${CMAKE_SOURCE_DIR}/iotchain)
SET (P2P_DIR ${CMAKE_SOURCE_DIR}/p2p-network)
SET (EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)
SET (TEST_BIN_DIR ${CMAKE_BINARY_DIR}/tests)

FILE (MAKE_DIRECTORY ${TEST_BIN_DIR})

FIND_LIBRARY (LIB_UUID NAMES libuuid.a)
MESSAGE (STATUS "uuid lib: ${LIB_UUID}")

INCLUDE_DIRECTORIES (${BLOCKCHAIN_DIR}/include)
INCLUDE_DIRECTORIES (${P2P_DIR}/include)
INCLUDE_DIRECTORIES (${EXTERNAL_DIR}/secp256k1/include)
INCLUDE_DIRECTORIES (${EXTERNAL_DIR}/cryptopp/include)
INCLUDE_DIRECTORIES (${EXTERNAL_DIR}/asio/include)
INCLUDE_DIRECTORIES (${EXTERNAL_DIR}/cxxopts/include)
INCLUDE_DIRECTORIES (${EXTERNAL_DIR}/rocksdb/include)

MESSAGE (STATUS "INCLUDE DIRECTORIES:")
MESSAGE (STATUS "${BLOCKCHAIN_DIR}/include")
MESSAGE (STATUS "${P2P_DIR}/include")
MESSAGE (STATUS "${EXTERNAL_DIR}/secp256k1/include")
MESSAGE (STATUS "${EXTERNAL_DIR}/cryptopp/include")
MESSAGE (STATUS "${EXTERNAL_DIR}/asio/include")
MESSAGE (STATUS "${EXTERNAL_DIR}/cxxopts/include")
MESSAGE (STATUS "${EXTERNAL_DIR}/rocksdb/include")
MESSAGE (STATUS "-------------------------------")


SET (P2P_NETWORK_SOURCES
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

SET (IOTCHAIN_SOURCES
    iotchain/src/P2P/msg_handler.cpp
    iotchain/src/P2P/p2p_node.cpp
    iotchain/src/CryptoTypes.cpp
    iotchain/src/Key/private_key.cpp
    iotchain/src/Key/public_key.cpp
    iotchain/src/base58.cpp
    iotchain/src/ChainTypes.cpp
    iotchain/src/serialization.cpp
    iotchain/src/Core/block_container.cpp
    iotchain/src/Core/block_index.cpp
    iotchain/src/Core/Blockchain.cpp
    iotchain/src/Logging/logger.cpp
    iotchain/src/Logging/spdlog/fmt/bundled/format.cc
    iotchain/src/Logging/spdlog/fmt/bundled/ostream.cc
    iotchain/src/Logging/spdlog/fmt/bundled/posix.cc
    iotchain/src/Logging/spdlog/fmt/bundled/printf.cc
    iotchain/src/db/db.cpp
    iotchain/src/VM/MCLVirtualMachine.cpp
)

ADD_SUBDIRECTORY(external/secp256k1)
ADD_SUBDIRECTORY(external/cxxopts)
ADD_SUBDIRECTORY(external/rocksdb)

ADD_LIBRARY (p2p-network
	STATIC
	${P2P_NETWORK_SOURCES})
	
ADD_LIBRARY (iotchain
	STATIC
	${IOTCHAIN_SOURCES})

hunter_add_package(cryptopp)
FIND_PACKAGE (cryptopp CONFIG REQUIRED)

INCLUDE ("${cryptopp_DIR}/cryptopp-config.cmake")
	
IF (UNIX)
	TARGET_LINK_LIBRARIES(p2p-network -pthread)
	TARGET_LINK_LIBRARIES(p2p-network ${LIB_UUID})
	TARGET_LINK_LIBRARIES(iotchain ${LIB_UUID})
ENDIF ()

IF (BUILD_FOR_RP3)
	TARGET_LINK_LIBRARIES(iotchain -lwiringPi)
ENDIF ()

TARGET_LINK_LIBRARIES (iotchain p2p-network secp256k1 cryptopp-static rocksdb)

ADD_EXECUTABLE(p2p
	iotchain_tests/main.cpp
	iotchain_tests/default_test.cpp
	iotchain_tests/p2p_test.cpp)

SET_TARGET_PROPERTIES(p2p PROPERTIES COMPILE_DEFINITIONS "RUN_P2P_TEST=1")

ADD_CUSTOM_COMMAND(TARGET p2p
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:p2p> ${TEST_BIN_DIR})

ADD_EXECUTABLE(sunum
	iotchain_tests/main.cpp
	iotchain_tests/default_test.cpp
	iotchain_tests/sunum.cpp)

SET_TARGET_PROPERTIES(sunum PROPERTIES COMPILE_DEFINITIONS "RUN_SUNUM_TEST=1")

ADD_CUSTOM_COMMAND(TARGET sunum
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sunum> ${TEST_BIN_DIR})

IF (UNIX)
	TARGET_LINK_LIBRARIES(p2p -pthread)
	TARGET_LINK_LIBRARIES(p2p ${LIB_UUID})
	TARGET_LINK_LIBRARIES(sunum -pthread)
	TARGET_LINK_LIBRARIES(sunum ${LIB_UUID})
ENDIF ()
IF (BUILD_FOR_RP3)
	TARGET_LINK_LIBRARIES(p2p -lwiringPi)
	TARGET_LINK_LIBRARIES(sunum -lwiringPi)
ENDIF ()
	
TARGET_LINK_LIBRARIES (p2p iotchain p2p-network cryptopp-static rocksdb)
TARGET_LINK_LIBRARIES (sunum iotchain p2p-network cryptopp-static rocksdb)
