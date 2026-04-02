#include "ArgParse.hpp"
#include "Logging.hpp"
#include "Timer.hpp"
#include "Runner.hpp"

#include <iostream>

#include <unistd.h>

int main(int argc, char *argv[]) {
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
