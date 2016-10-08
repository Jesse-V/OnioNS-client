cmake_minimum_required(VERSION 2.8)

project(onions-client)

# define compiler options for Clang and GCC/G++
# I'm aware of http://voices.canonical.com/jussi.pakkanen/2013/03/26/a-list-of-common-cmake-antipatterns/
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11 -fstack-protector -D_FORTIFY_SOURCE=1")
SET(CLANG_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -Wno-c++98-compat-pedantic -pedantic -Weverything -Wno-exit-time-destructors -Wno-weak-vtables -Wno-padded -Wno-deprecated -Wno-documentation -Wno-documentation-unknown-command -Wno-reserved-id-macro -Wno-missing-noreturn -Wno-sign-conversion -Wno-shadow")
SET(GCC_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -pedantic -Wdouble-promotion -Wfloat-equal -Wunsafe-loop-optimizations")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS_DEBUG "${CLANG_DEBUG}")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS_DEBUG "${GCC_DEBUG}")
endif()

include_directories(${CMAKE_INSTALL_PREFIX}/include/onions-common /usr/include/botan-1.10)

add_definitions(-DINSTALL_PREFIX=std::string\("${CMAKE_INSTALL_PREFIX}"\))

add_executable(onions-client
  main.cpp
  TorManager.cpp
  Resolver.cpp
)

FIND_LIBRARY(botan NAMES "botan-1.10" REQUIRED)
FIND_LIBRARY(argtable2 NAMES "argtable2" REQUIRED)

# find main onions-common library components
FIND_LIBRARY(onions-common NAMES "onions-common" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-jsonrpccpp-common NAMES "jsonrpccpp-common" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-jsonrpccpp-client NAMES "jsonrpccpp-client" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-jsonrpccpp-server NAMES "jsonrpccpp-server" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)

if (${onions-common} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "OnioNS-common could not be found! Please install it.")
else()
  message(STATUS "Found OnioNS-common library at ${onions-common}")
  message(STATUS "Found JSON-RPC-CPP library at ${onions-jsonrpccpp-client}")
endif()

target_link_libraries(onions-client ${onions-common} ${onions-jsonrpccpp-common} ${onions-jsonrpccpp-client} ${onions-jsonrpccpp-server} jsoncpp pthread botan-1.10)

#installation of files
set(ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
install(TARGETS onions-client               DESTINATION bin/)
install(FILES ${ASSETS}/onions-client.1.gz  DESTINATION share/man/man1/)
