
#include "Client.hpp"
#include <onions-common/Log.hpp>
#include <onions-common/Utils.hpp>
#include <botan/botan.h>
#include <popt.h>

// Botan::LibraryInitializer init("thread_safe");

int main(int argc, char** argv)
{
  char* logPath = NULL;
  bool license = false;
  ushort port = 9150;

  struct poptOption po[] = {
      {"output",
       'o',
       POPT_ARG_STRING,
       &logPath,
       0,
       "Specifies the filepath for event logging.",
       "<path>"},
      {"port",
       'p',
       POPT_ARG_SHORT,
       &port,
       0,
       "SOCKS port to use for Tor communication. The default is 9150.",
       "<port>"},
      {"license",
       'L',
       POPT_ARG_NONE,
       &license,
       0,
       "Print software license and exit.",
       NULL},
      POPT_AUTOHELP{NULL, 0, 0, NULL, 0, NULL, NULL}};

  if (!Utils::parse(
          poptGetContext(NULL, argc, const_cast<const char**>(argv), po, 0)))
  {
    std::cout << "Failed to parse command-line arguments. Aborting.\n";
    return EXIT_FAILURE;
  }

  if (license)
  {
    std::cout << "Modified/New BSD License" << std::endl;
    return EXIT_SUCCESS;
  }

  if (logPath && strcmp(logPath, "-") == 0)
    Log::setLogPath(std::string(logPath));

  Client::get().listenForDomains(port);

  return EXIT_SUCCESS;
}
