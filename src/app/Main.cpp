#include "Runner.hpp"
#include "utils/ArgParse.hpp"
#include "utils/Logging.hpp"

#include <ctime>
#include <unistd.h>

int main(int argc, char *argv[]) {
  getLoggerConfig();
#ifdef LOG
  openLogFile("log/run");
#endif
  Args a = parseArgs(argc, argv);
#ifdef DEBUG
  srand48(1234); // seed the random number generator
#else
  srand48(time(0) + getpid());
#endif
  return run(a);
}
