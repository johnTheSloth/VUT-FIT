#include "ipk-client.h"
#include "md5.h"

// Basic function to throw exception in case of error
void throwException(const char *message) {
    std::cerr << (message) << std::endl;
    exit(EXIT_FAILURE);
}

// Function to get command - SOLVE(1) or BYE(0)
int getCom(char *recv) {
    std::string recvStr(recv);
    std::string cmd = recvStr.substr(0,recvStr.find(" "));
    if (cmd == "BYE") {
        return 0;
    }else if(cmd == "SOLVE") {
        return 1;
    }else {
        // Ignore
        return -1;
    }
    return 0;
}

// Function to solve sent equation
std::string solveEquation(char *recv) {
    std::string recvStr(recv);
    recvStr = recvStr.substr(0,recvStr.find('\n')+1);
    // get equation string
    std::string eq = recvStr.substr(recvStr.find(" ")+1);
    // get fistr number
    std::string firstNum = eq.substr(0,eq.find(" "));
    // get second number
    std::string secondNum = eq.substr(eq.rfind(" ")+1, eq.find("\n")-eq.rfind(" ")-1);

    double firstF;
    double secondF;

    // Parse numbers to doubles
    try{
        size_t i(0);
        size_t j(0);
        firstF = std::stof(firstNum,&i);
        secondF = std::stof(secondNum,&j);
        if(i != firstNum.length() || j != secondNum.length()) {
            return std::string("SELEKTHA");
        }
    }catch(const std::invalid_argument& ia) {
        return std::string("SELEKTHA");
    }catch(const std::out_of_range& oor) {
        return std::string("SELEKTHA");
    }

    // get operator
    int m = eq.find(" ");
    std::string op = eq.substr(m+1, eq.rfind(" ")-m-1);

    // check if operator part is valid
    if(op.size() != 1) {
        return std::string("SELEKTHA");
    }

    if(firstF > INT_MAX || firstF < INT_MIN || secondF > INT_MAX || secondF < INT_MIN) {
        return std::string("SELEKTHA");
    }
    double res;
    std::stringstream stream;
    // solve equation and return it as string, or return "ERROR"
    switch (*(op.c_str())) {
        case '*':
            res = firstF * secondF;
            if(res > INT_MAX || res < INT_MIN) {
                return std::string("SELEKTHA");
            }
            stream << std::fixed << std::setprecision(10) << res;
            return stream.str().substr(0,stream.str().find('.')+3);

        case '+':
            res = firstF + secondF;
            stream << std::fixed << std::setprecision(10) << res;
            return stream.str().substr(0,stream.str().find('.')+3);
        case '/':
            if(secondF == 0) {
                return std::string("ERROR");
            }
            res = firstF / secondF;

            if(res > INT_MAX || res < INT_MIN) {
                return std::string("SELEKTHA");
            }
            stream << std::fixed << std::setprecision(10) << res;
            return stream.str().substr(0,stream.str().find('.')+3);
        case '-':
            res = firstF - secondF;
            stream << std::fixed << std::setprecision(10) << res;
            return stream.str().substr(0,stream.str().find('.')+3);
        default:
            return std::string("SELEKTHA");
    }
    // just to be sure
    return std::string("SELEKTHA");
}

// Main function
int main(int argc, char *argv[]) {
    // Check if program is run with IP adress
    if(argc != 2) {
        throwException("Error: Invalid number of arguments.");
    }
    // Default port
    unsigned short port = 55555;
    int client_socket, ret;
    char response[BUFFSIZE];
    bzero(response, BUFFSIZE);
    // Server strctures
    struct hostent *server;
    struct addrinfo hints, *res = NULL;
    hints.ai_family = PF_UNSPEC;
    hints.ai_flags = SOCK_STREAM;

    // Translate domain name to IP
    bzero(&hints, sizeof(hints));
    if ((ret = getaddrinfo(argv[1], NULL, &hints, &res))) {
        throwException("Error: Invalid IP address.");
    }

    // manage connection and type of protocol
    if (res->ai_family == AF_INET) {
        // use IPv4 protocol
        struct sockaddr_in serveraddr4;
        serveraddr4 = *(struct sockaddr_in *) res->ai_addr;
        serveraddr4.sin_port = htons(port);
        serveraddr4.sin_family = res->ai_family;

        if ((client_socket = socket(serveraddr4.sin_family, SOCK_STREAM, 0)) < 0) {
            throwException("Error: Could not open client socket.\n");
        }
        if (connect(client_socket, (struct sockaddr *)&serveraddr4, sizeof(serveraddr4)) < 0) {
            throwException("Error: Couldn't connect to server.\n");
        }
    } else if (res->ai_family == AF_INET6) {
        // use IPv6 protocol, not working ATM
        server = gethostbyname2(argv[1],AF_INET6);
        struct sockaddr_in6 serveraddr6;
        serveraddr6.sin6_family = res->ai_family;
		serveraddr6.sin6_port = htons(port);
        serveraddr6.sin6_flowinfo = 0;

        memmove((char *) &serveraddr6.sin6_addr.s6_addr, (char *) server->h_addr, server->h_length);

        if ((client_socket = socket(serveraddr6.sin6_family, SOCK_STREAM, 0)) <= 0) {
            throwException("Error: Could not open client socket.\n");
        }
        if ((connect(client_socket, (struct sockaddr *)&serveraddr6, sizeof(serveraddr6))) < 0) {
            throwException("Error: Couldn't connect to server.\n");
        }
    } else {
        throwException("Error: Invalid protocol family.");
    }
    // just to be sure
    freeaddrinfo(res);

    // Hash login
    std::string message = std::string("HELLO ");
    message += md5(LOGIN);
    message += "\n";

    // send our HELLO message
    int sent = send(client_socket, message.c_str(), message.size(), 0);
    if(sent <0) throwException("Error: HELLO not sent.");

    int rcv;

    // Main loop to receive and send
    for (;;) {
        // clear buffer
        bzero(response, BUFFSIZE);
        if ((rcv = recv(client_socket, response, BUFFSIZE, 0)) > 0) {
            //std::cout << "-> " << response;
            // If SOLVE is received
            if(getCom(response) == 1) {
                std::string tajak = solveEquation(response);
                // If there is no SELEKTHA, send RESULT
                if(strcmp(tajak.c_str(),"SELEKTHA") != 0) {
                    std::string result = "RESULT " + tajak +"\n";
                    bzero(response, BUFFSIZE);
                    strcpy(response, result.c_str());
                    send(client_socket, response, result.size(), 0);
                    //std::cout << "<- "<< response << std::endl;
                }else {
                    // THERE is SELEKTHA placeholder so ignore this piece of message
                }
            // if BYE is received
           }else if(getCom(response) == 0){
               // Everythings done, BYE string received
               std::cout << std::string(response).substr(4,std::string(response).find("\n")-4);// << std::endl;
               break;
           }
           //else PASS
        } else {
            // Error in receiving
            if (rcv == 0) break;
            throwException("Error: Couldn't read data from server correctly.");
        }
    }

    if (close(client_socket) != 0) throwException("Error: Could not close socket.\n");

    return 0;
}

//./ipk-client pcgregr.fit.vutbr.cz && ./ipk-client 147.229.13.210 && ./ipk-client 2001:67c:1220:80c::93e5:dd2
