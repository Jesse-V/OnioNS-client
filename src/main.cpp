
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

  struct poptOption po[] = {{
                             "output",
                             'o',
                             POPT_ARG_STRING,
                             &logPath,
                             11001,
                             "Specifies the filepath for event logging.",
                             "<path>",
                            },
                            {"license",
                             'L',
                             POPT_ARG_NONE,
                             &license,
                             11002,
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

  Client::listenForDomains();

  return EXIT_SUCCESS;
}
