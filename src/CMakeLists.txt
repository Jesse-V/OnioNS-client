cmake_minimum_required(VERSION 2.8)

project(onions-client)

find_package(Boost COMPONENTS system REQUIRED)

#compiler options for Clang and GCC/G++
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-g -O3 --std=c++11 -Wno-c++98-compat-pedantic -pedantic -Weverything -Wno-exit-time-destructors -Wno-weak-vtables -Wno-documentation -Wno-documentation-unknown-command -Wno-reserved-id-macro")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "-g -O3 --std=c++11 -Wall -Wextra -pedantic -Wdouble-promotion -Wfloat-equal -Wunsafe-loop-optimizations")
endif()

add_executable(onions-client
  main.cpp
  Client.cpp

  tcp/IPC.cpp
  tcp/IPCSession.cpp
)

add_executable(onions-tbb
  tbb-mitm/launch.cpp
)

add_dependencies(onions-client onions-tbb)

include_directories(/usr/include/onions-common/include /usr/include/botan-1.10)

target_link_libraries(onions-client onions-common onions-jsoncpp onions-cyoencode popt botan-1.10  pthread ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_LIBRARIES})

#installation of files
set(ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
install(TARGETS onions-client               DESTINATION /usr/bin/)
install(FILES ${ASSETS}/client.py           DESTINATION /var/lib/tor-onions/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/onions-tbb DESTINATION /usr/bin/ PERMISSIONS WORLD_READ WORLD_EXECUTE GROUP_READ GROUP_EXECUTE OWNER_READ OWNER_WRITE OWNER_EXECUTE)
install(FILES ${ASSETS}/onions-client.1.gz  DESTINATION /usr/share/man/man1)
