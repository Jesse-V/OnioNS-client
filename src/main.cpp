
#include "Client.hpp"
//#include <onions-common/Flags.hpp>
#include <botan/botan.h>

Botan::LibraryInitializer init("thread_safe");

int main(int argc, char** argv)
{
  // if (!Flags::get().parse(argc, argv))
  //  return EXIT_FAILURE;

  Client::get().listenForDomains();

  return EXIT_SUCCESS;
}
