#include <stdlib.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <math.h>
#include <climits>
#include <stdexcept>
#include <iomanip>
#include <sstream>

#define BUFFSIZE 1024
#define LOGIN "xgross09"

void throwException(const char *message);
std::string solveEquation(char *recv);
int main(int argc, char *argv[]);
