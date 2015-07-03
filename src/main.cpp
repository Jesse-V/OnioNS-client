

#include "Flags.hpp"
#include "common/Environment.hpp"
#include "client/Client.hpp"
#include "hs/HS.hpp"
#include "mirror/Mirror.hpp"
#include <botan/botan.h>
#include <fstream>
#include <iostream>

Botan::LibraryInitializer init("thread_safe");

int main(int argc, char** argv)
{
  if (!Flags::get().parse(argc, argv))
    return EXIT_FAILURE;

  Client::get().listenForDomains();

  return EXIT_SUCCESS;
}
