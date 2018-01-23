CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (IoTChain)

OPTION (ENABLE_DEBUG_MSG "Print debug log of network messages." ON)
OPTION (ENABLE_FILE_LOGGING "Enable log to file." ON)
OPTION (ENABLE_DEBUG_CLIENT_SEND_PING "Enable ping-pong msg after connected to peer." ON)

IF (ENABLE_DEBUG_MSG)
	ADD_DEFINITIONS(-DDEBUG_MSG)
ENDIF ()

IF (ENABLE_FILE_LOGGING)
	ADD_DEFINITIONS(-DENABLE_FILE_LOGGING)
ENDIF ()

IF (ENABLE_DEBUG_CLIENT_SEND_PING)
	ADD_DEFINITIONS(-DDEBUG_CLIENT_SEND_PING)
ENDIF ()

IF ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    MESSAGE( "64 bits compiler detected" )
    SET( EX_PLATFORM 64 )
    SET( EX_PLATFORM_NAME "x64" )
ELSE ( CMAKE_SIZEOF_VOID_P EQUAL 8 ) 
    MESSAGE( "32 bits compiler detected" )
    SET( EX_PLATFORM 32 )
    SET( EX_PLATFORM_NAME "x86" )
ENDIF ( CMAKE_SIZEOF_VOID_P EQUAL 8 )

#set up some important folders
SET (BLOCKCHAIN_DIR ${CMAKE_SOURCE_DIR}/iotchain)
SET (P2P_DIR ${CMAKE_SOURCE_DIR}/p2p-network)
SET (EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

#set external libraries to link
IF (UNIX)
	#use c++14
	ADD_DEFINITIONS("-std=c++14")
	MESSAGE ( "searching for libraries" )
	FIND_LIBRARY (SECP256K1_LIB_DBG NAMES libsecp256k1.a)
	FIND_LIBRARY (CRYPTOPP_LIB_DBG NAMES libcryptopp.a)
	SET (SECP256K1_LIB_REL ${SECP256K1_LIB_DBG})
	SET (CRYPTOPP_LIB_REL ${CRYPTOPP_LIB_DBG})
ELSE (UNIX)
	IF ( EX_PLATFORM EQUAL 64 )		#if 64 bit compiling
		SET (SECP256K1_LIB_DBG ${EXTERNAL_DIR}/secp256k1/lib/x64/Debug/secp256k1.lib)
		SET (CRYPTOPP_LIB_DBG ${EXTERNAL_DIR}/cryptopp/lib/x64/Debug/cryptlib.lib)
		SET (SECP256K1_LIB_REL ${EXTERNAL_DIR}/secp256k1/lib/x64/Release/secp256k1.lib)
		SET (CRYPTOPP_LIB_REL ${EXTERNAL_DIR}/cryptopp/lib/x64/Release/cryptlib.lib)
	ELSE ( EX_PLATFORM EQUAL 64 )	#if 32bit compiling
		SET (SECP256K1_LIB_DBG ${EXTERNAL_DIR}/secp256k1/lib/Win32/Debug/secp256k1.lib)
		SET (CRYPTOPP_LIB_DBG ${EXTERNAL_DIR}/cryptopp/lib/Win32/Debug/cryptlib.lib)
		SET (SECP256K1_LIB_REL ${EXTERNAL_DIR}/secp256k1/lib/Win32/Release/secp256k1.lib)
		SET (CRYPTOPP_LIB_REL ${EXTERNAL_DIR}/cryptopp/lib/Win32/Release/cryptlib.lib)
	ENDIF ( EX_PLATFORM EQUAL 64 )
ENDIF (UNIX)

MESSAGE(STATUS "EC lib: ${SECP256K1_LIB_DBG}")
MESSAGE(STATUS "CryptoPP lib: ${CRYPTOPP_LIB_DBG}")

INCLUDE_DIRECTORIES (${BLOCKCHAIN_DIR}/include)
INCLUDE_DIRECTORIES (${P2P_DIR}/include)
INCLUDE_DIRECTORIES (${EXTERNAL_DIR}/secp256k1/include)
INCLUDE_DIRECTORIES (${EXTERNAL_DIR}/cryptopp/include)
INCLUDE_DIRECTORIES (${EXTERNAL_DIR}/asio/include)

MESSAGE (STATUS "INCLUDE DIRECTORIES:")
MESSAGE (STATUS "${BLOCKCHAIN_DIR}/include")
MESSAGE (STATUS "${P2P_DIR}/include")
MESSAGE (STATUS "${EXTERNAL_DIR}/secp256k1/include")
MESSAGE (STATUS "${EXTERNAL_DIR}/cryptopp/include")
MESSAGE (STATUS "${EXTERNAL_DIR}/asio/include")
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
    iotchain/src/Core/block_container.cpp
    iotchain/src/Core/Blockchain.cpp
    iotchain/src/Logging/logger.cpp
    iotchain/src/Logging/spdlog/fmt/bundled/format.cc
    iotchain/src/Logging/spdlog/fmt/bundled/ostream.cc
    iotchain/src/Logging/spdlog/fmt/bundled/posix.cc
    iotchain/src/Logging/spdlog/fmt/bundled/printf.cc
)	

ADD_LIBRARY (p2p-network
	STATIC
	${P2P_NETWORK_SOURCES})
	
ADD_LIBRARY (iotchain
	STATIC
	${IOTCHAIN_SOURCES})
	
IF (UNIX)
	TARGET_LINK_LIBRARIES(p2p-network -static-libgcc -static-libstdc++ -pthread)
	TARGET_LINK_LIBRARIES(iotchain -static-libgcc -static-libstdc++ -pthread)
ENDIF ()

TARGET_LINK_LIBRARIES (iotchain p2p-network)
TARGET_LINK_LIBRARIES (iotchain debug ${SECP256K1_LIB_DBG} optimized ${SECP256K1_LIB_REL})
TARGET_LINK_LIBRARIES (iotchain debug ${CRYPTOPP_LIB_DBG} optimized ${CRYPTOPP_LIB_REL})

ADD_EXECUTABLE(iotchain_tests 
	iotchain_tests/main.cpp
	iotchain_tests/default_test.cpp
	iotchain_tests/p2p_test.cpp)
	
IF (UNIX)
	TARGET_LINK_LIBRARIES(iotchain_tests -static-libgcc -static-libstdc++ -pthread)
ENDIF ()
	
TARGET_LINK_LIBRARIES (iotchain_tests iotchain p2p-network)
TARGET_LINK_LIBRARIES (iotchain_tests debug ${SECP256K1_LIB_DBG} optimized ${SECP256K1_LIB_REL})
TARGET_LINK_LIBRARIES (iotchain_tests debug ${CRYPTOPP_LIB_DBG} optimized ${CRYPTOPP_LIB_REL})

