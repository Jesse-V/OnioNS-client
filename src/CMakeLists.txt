cmake_minimum_required(VERSION 2.8)

project(onions-client)

# define compiler options for Clang and GCC/G++
SET(COMMON_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -std=c++11 -fstack-protector -D_FORTIFY_SOURCE=1")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -Wno-c++98-compat-pedantic -pedantic -Weverything -Wno-exit-time-destructors -Wno-weak-vtables -Wno-padded -Wno-deprecated -Wno-documentation -Wno-documentation-unknown-command -Wno-reserved-id-macro -Wno-missing-noreturn -Wno-sign-conversion -Wno-shadow")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -Wall -Wextra -pedantic -Wdouble-promotion -Wfloat-equal -Wunsafe-loop-optimizations")
endif()

include_directories(${CMAKE_INSTALL_PREFIX}/include/onions-common /usr/include/botan-1.10)

add_definitions(-DINSTALL_PREFIX=std::string\("${CMAKE_INSTALL_PREFIX}"\))

add_executable(onions-client
  main.cpp
  TorManager.cpp
  resolver/Resolver.cpp
  resolver/LocalServer.cpp
)

# find main onions-common library components
FIND_LIBRARY(onions-common NAMES "onions-common" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-cyoencode NAMES "onions-cyoencode" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-jsoncpp NAMES "onions-jsoncpp" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)

# find json-rpc-cpp components
FIND_LIBRARY(onions-jsonrpccpp-common NAMES "jsonrpccpp-common" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common/lib NO_DEFAULT_PATH)
FIND_LIBRARY(onions-jsonrpccpp-client NAMES "jsonrpccpp-client" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common/lib NO_DEFAULT_PATH)
FIND_LIBRARY(onions-jsonrpccpp-server NAMES "jsonrpccpp-server" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common/lib NO_DEFAULT_PATH)

if (${onions-common} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "OnioNS-common could not be found! Please install it.")
else()
  message(STATUS "Found OnioNS-common library at ${onions-common}")
  message(STATUS "Found JSON-RPC-CPP library at ${onions-jsonrpccpp-common}")
endif()

target_link_libraries(onions-client ${onions-common} ${onions-cyoencode}
  ${onions-jsoncpp} ${onions-jsonrpccpp-common} ${onions-jsonrpccpp-server} botan-1.10 pthread)

#installation of files
set(ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
install(TARGETS onions-client               DESTINATION bin/)
install(FILES ${ASSETS}/onions-client.1.gz  DESTINATION share/man/man1/)
