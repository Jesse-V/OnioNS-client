
#include <iostream>
//#include <vector>

#include <sys/wait.h>
#include <unistd.h>


char** getStemProcess()
{
  const char** args = new const char* [3];
  args[0] = "python\0";
  args[1] = "/var/lib/tor-onions/client.py\0";
  args[2] = NULL;
  return const_cast<char**>(args);
}



char** getOnionsClientProcess()
{
  const char** args = new const char* [2];
  args[0] = "onions-client\0";
  args[2] = NULL;
  return const_cast<char**>(args);
}



void startProcess(char** args)
{  // http://www.cplusplus.com/forum/lounge/17684/
  pid_t pid = fork();

  switch (pid)
  {
    case -1:
      std::cerr << "Uh-Oh! fork() failed.\n";
      exit(1);
    case 0:                   // Child process
      execvp(args[0], args);  // Execute the program
      std::cerr << "Failed to execute process!\n";
      exit(1);
    default: /* Parent process */
      std::cout << "Process created with pid " << pid << "\n";
      wait(&pid);
      std::cout << "Process \"" << args[0] << "\" exited." << std::endl;
  }
}

int main(int argc, char* argv[])
{
  // start Tor
  std::cout << "Launching Tor..." << std::endl;
  argv[0] = const_cast<char*>("./torbin");
  startProcess(argv);

  std::cout << "Launching OnioNS client software..." << std::endl;
  startProcess(getOnionsClientProcess());  // launch onions-client

  std::cout << "Launching OnioNS-TBB software..." << std::endl;
  startProcess(getStemProcess());  // launch Stem script
}

/*
char* args[256];
char arg1[256] = "python";
char arg2[256] = "hi.py";
args[0] = arg1;
args[1] = arg2;

pid_t childID = fork();
if (childID == 0)
  execvp(args[0], args);  // instantly switch to the called process
else if (childID < 0)
  std::cout << "Failed to create child process " << childID << "!\n";

wait(NULL);*/



/*
//output goes straight to the console, there's no piping
int handleSingleCommand(char** givenTokens)
{
  struct timeval startTime, endTime;
  gettimeofday(&startTime, NULL);

  pid_t childID = fork();
  if (childID == 0)
    execvp(givenTokens[0], givenTokens); //instantly switch to the called
process
  else if (childID < 0)
    cout << "Failed to create child process " << childID << "!" << endl;

  wait(NULL); //parent needs to wait for child to finish before continuing

  gettimeofday(&endTime, NULL);

  return endTime.tv_usec - startTime.tv_usec;
}



//handles the piping to redirect the output of one process to the input of
another. P1 -> pipe -> P2 -> console.
int handlePiping(char** givenTokens, int tokenCount, int redirectIndex)
{
  int pipes[PIPE_COUNT];
  pipe(pipes);

  int savedOut = dup(STDOUT), savedIn = dup(STDIN); //save off standard reads
and writes so we can restore them later

  char** firstTokenSet = getFirstSetOfTokens(givenTokens, redirectIndex);
//tokens before the redirect symbol, such as {"cat", "Shell.cpp"}
  char** secondTokenSet = getSecondSetOfTokens(givenTokens, tokenCount,
redirectIndex); //tokens after the redirect symbol, such as {"more"}

  struct timeval startTime, endTime;
  gettimeofday(&startTime, NULL);

  pid_t child1 = fork();
  if (child1 == 0)
  {
    dup2(pipes[PIPE_WRITE_END], STDOUT); //instead of writing to cout, now write
to pipe
    execvp(firstTokenSet[0], firstTokenSet);
  }

  pid_t child2 = fork();
  if (child2 == 0)
  {
    dup2(pipes[PIPE_READ_END], STDIN); //instead of reading from cin, now read
from pipe
    close(pipes[PIPE_WRITE_END]);
    execvp(secondTokenSet[0], secondTokenSet);
  }

  wait(&child1);

  close(pipes[PIPE_READ_END]); //close pipes
  close(pipes[PIPE_WRITE_END]);

  wait(&child2);

  dup2(savedIn, STDIN); //restore
  dup2(savedOut, STDOUT); //restore

  gettimeofday(&endTime, NULL);

  return endTime.tv_usec - startTime.tv_usec;
}



//returns an array of tokens before the redirect. In the example this will
return {"cat", "Shell.cpp"}
char** getFirstSetOfTokens(char** givenTokens, int redirectIndex)
{
  int firstTokensCount = redirectIndex;

  char** firstTokens = new char*[firstTokensCount + 1]; //leave one more for the
NULL
  for (int j = 0; j < firstTokensCount; j++)
    firstTokens[j] = givenTokens[j];
  firstTokens[firstTokensCount] = NULL;

  //for (int j = 0; firstTokens[j] != NULL; j++)
  //  cout << " " << firstTokens[j] << endl;

  return firstTokens;
}



//returns an array of tokens after the redirect. In the example this will return
{"more"}
char** getSecondSetOfTokens(char** givenTokens, int tokenCount, int
redirectIndex)
{
  int secondTokensCount = tokenCount - redirectIndex - 1; //we're ignoring the
actual redirect command

  char** secondTokens = new char*[secondTokensCount + 1]; //leave one more for
the NULL
  for (int j = 0; j < secondTokensCount; j++)
    secondTokens[j] = givenTokens[redirectIndex + j + 1]; //we're ignoring the
actual redirect command
  secondTokens[secondTokensCount] = NULL;

  //for (int j = 0; secondTokens[j] != NULL; j++)
  //  cout << " " << secondTokens[j] << endl;

  return secondTokens;
}



//tokenizes the command and returns an array of tokens.
//For example "cat Shell.cpp > more" returns {"cat", "Shell.cpp", ">", "more"}
char** parseCommand(string command)
{
  char* cstr = new char[command.size() + 1];
  strcpy(cstr, command.c_str());
  char* arg = strtok(cstr, " ");

  vector<char*> argsVector;
  while (arg != NULL)
  {
    argsVector.push_back(arg);
    arg = strtok(NULL, " ");
  }

  char** args = new char*[argsVector.size() + 1];
  copy(argsVector.begin(), argsVector.end(), args);
  args[argsVector.size()] = NULL;

  return args;
}



//asks the user for a command and returns it.
//It will keep asking if no command is actually entered, similar to the standard
shell
string getCommand()
{
  string str;
  while (str.empty())
  {
    cout << "[cmd]: ";
    getline(cin, str);
  }
  return str;
}
*/
