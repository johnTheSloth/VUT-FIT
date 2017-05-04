/*
 *  IPK 2017
 *  Projekt 1
 *  Author: Jan Grossmann
 *
 *  CLIENT
 */
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <regex>
#include "both.cpp"


// main structure
struct arg_t {
    eComm command;
    std::string host;
    int port;
    std::string user;
    std::string path;
    std::string file;
    eType type;
    std::string error;
    int contentLen;
}arg;

// show error and exit
void throwException(std::string ex) {
    std::cerr << "Error: "<< ex << std::endl;
    exit(EXIT_FAILURE);
}

// parse program arguments
int parseArgs(arg_t *arg, char *cmd, char *str) {
    std::string command(cmd);
    std::string first(str);

    int found = 0;
    // Match command
    if (command.find("mkd") == 0 || command.find("put") == 0){
        arg->command = PUT;
    }else if (command.find("lst") == 0 || command.find("get") == 0){
        arg->command = GET;
    }else if (command.find("del") == 0 || command.find("rmd") == 0){
        arg->command = DELETE;
    }else {
        throwException("Invalid command.");
    }

    // choose if file or directory
    if (command.find("mkd") == 0 || command.find("rmd") == 0 || command.find("lst") == 0){
        arg->type = typeDir;
    }else if (command.find("put") == 0 || command.find("get") == 0 || command.find("del") == 0){
        arg->type = typeFile;
    }else {
        throwException("Invalid command.");
    }

    //now HTTP
    if (first.find("http://") != 0) {
        throwException("Invalid protocol.");
    }else {
        first.erase(first.begin(), first.begin()+7);
    }

    //find hostname
    if ((found = first.find(":")) > 0){
        arg->host = first.substr(0,found);
        first.erase(first.begin(), first.begin()+found+1);
    }else {
        throwException("Invalid hostname.");
    }

    //find PORT
    if ((found = first.find("/")) > 0){
        arg->port = stoi(first.substr(0,found));
        first.erase(first.begin(), first.begin()+found+1);
    }else {
        throwException("Invalid port.");
    }

    // get filename
    if ((found = first.rfind("/")) >= 0) {
        arg->file = first.substr(found+1,100);
        first.erase (first.begin()+found+1, first.end());
    }else {
        throwException("Invalid filename.");
    }

    // get name of user
    if ((found = first.find("/"))  >= 0) {
        arg->user = first.substr(0,found);
        first.erase (first.begin(), first.begin()+found+1);
    }else {
        throwException("Invalid username.");
    }

    // get path
    arg->path = first;
    return 0;
}

// function to generate header
std::string createHeader(arg_t arg, char *localPath) {
    std::string header = "";

    // first create COMMAND
    if(arg.command == 0) {
        header += "GET ";
    }else if(arg.command == 1) {
        header += "PUT ";
    }else if(arg.command == 2) {
        header += "DELETE ";
    }

    header += "/";
    header += arg.user;
    header += "/";
    header += arg.path;
        //header += "/";
    header += arg.file;
    header += "?type=";
    if(arg.type) {
        header+= "folder ";
    }else {
        header += "file ";
    }
    header += "HTTP/1.1\r\nDate: ";
    header += createTimestamp();
    header += "Accept: text/plain\r\nAccept-Encoding: identity\r\n";
    // get mime type
    if(arg.command == PUT && arg.type == typeFile) {
        header += "Content-Length: ";
        std::string lp(localPath);
        std::string cLenko = std::to_string(fileSize(lp));
        header += cLenko;
        header += "\r\n";

        std::string path = "file --mime-type -b ";

        path += localPath;
        FILE *fp;
        char file_type[40];

        if(arg.type == typeFile) {
            fp = popen(path.c_str(), "r");
            if (fp == NULL) {
                std::cerr << "Unable to get mime-type." << std::endl;
                exit(-1);
            }

            std::string mime;
            while (fgets(file_type, sizeof(file_type), fp) != NULL) {
                mime = file_type;
            }
            pclose(fp);

            mime.erase(mime.end()-1,mime.end());
            header += "Content-Type: ";
            header +=  mime.c_str();
            header += "\r\n";
        }
    }
    header += "\r\n";
    return header;
}

// Parse received header
int parseRecvHead(char *rec, struct arg_t *arg, std::string &rest, int &recLen, int &ff) {

    std::string header(rec);
    std::string clens(header.substr(header.find("Content-Length: ")+16));
    arg->contentLen= atoi(clens.substr(0,clens.find("\r\n")).c_str());
    // check first line of header
    std::string first = header.substr(0,header.find("\r\n"));
    int found = 0;

    // find where content begins
    if((found = header.find("\r\n\r\n")) != 0) {
        ff = found+4;
        recLen = BUFFSIZE - found - 4;
        rest = header.substr(found + 4);
        if(arg->contentLen > BUFFSIZE) {
            rest.erase(rest.end()-1, rest.end());
        }else {
            recLen = arg->contentLen;
        }
    }

    // cherck status code and show error if needed
    if (first.find("HTTP/1.1 200") == 0){
    }else if (first.find("HTTP/1.1 400") == 0){
        if(arg->command == DELETE) {
            if(arg->type == typeFile) {
                throwException("Not a file.");
            }else {
                throwException("Not a directory.");
            }
        }else if(arg->command == PUT) {
            if(arg->type == typeFile) {
                throwException("Already exists.");
            }else {
            //    std::cerr << "Directory not found." << std::endl;
            }
        }else if(arg->command == GET) {
            if(arg->type == typeFile) {
                throwException("Not a file.");
            }else {
                throwException("Not a directory.");
            }
        }
    }else if (first.find("HTTP/1.1 404") == 0){
        if(arg->command == DELETE) {
            if(arg->type == typeFile) {
                throwException("File not found.");
            }else {
                throwException("Directory not found.");
            }
        }else if(arg->command == PUT) {
            if(arg->type == typeFile) {
                //std::cerr << "File not found." << std::endl;
            }else {
                throwException("Directory not found.");
            }
        }else if(arg->command == GET) {
            if(arg->type == typeFile) {
                throwException("File not found.");
            }else {
                throwException("Directory not found.");
            }
        }
    }else if (first.find("HTTP/1.1 492") == 0){
        throwException("Directory not empty.");
    }else if (first.find("HTTP/1.1 490") == 0){
        throwException("User Account Not Found.");
    }else {
        throwException("Invalid header.");
    }
    return 0;
}


int main(int argc, char ** argv){
    char *command;
    char *remotePath;
    char *localPath;
    // Argc check
    if(argc != 2 && argc != 3 && argc != 4) {
        throwException("Error argc.");
    }else if(argc == 3){
        if(strcmp(argv[1],"del") == 0) {                // DEL
            command = argv[1];
            remotePath = argv[2];
        }else if(strcmp(argv[1],"lst") == 0) {          // LST
            command = argv[1];
            remotePath = argv[2];
        }else if(strcmp(argv[1],"mkd") == 0) {          // MKD
            command = argv[1];
            remotePath = argv[2];
        }else if(strcmp(argv[1],"rmd") == 0) {          // RMD
            command = argv[1];
            remotePath = argv[2];
        }else if(strcmp(argv[1],"get") == 0) {          // GET
            command = argv[1];
            remotePath = argv[2];
        }else {
            throwException("Invalid args.");
        }
    }else if(argc == 4) {
        if(strcmp(argv[1],"put") == 0) {          // PUT
            command = argv[1];
            remotePath = argv[2];
            localPath = argv[3];
        }else {
            throwException("Invalid args.");
        }
    }

    char header[BUFFSIZE];
    if(parseArgs(&arg, command, remotePath)){
        std::cout << "Error: " << arg.error << std::endl;
    }else {
        // generate header
        strcpy(header,createHeader(arg, localPath).c_str());

        // Client part
        int clientSocket, bytestx, bytesrx;
        struct sockaddr_in serverAddress;
        struct hostent *server;
        char buffer[BUFFSIZE];

        // create socket
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket <= 0) {
            throwException("Error in socket().");
        }

        // resolve hostname
        server = gethostbyname(arg.host.c_str());
        if(server == NULL) {
            throwException("Error in hostname.");
        }

        // validate port range
        if(arg.port < 1023 || arg.port > 65536) {
            throwException("Error in port.");
        }

        // prepare structure addr.
        memset(&serverAddress,0,sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(arg.port);
        memmove((char *)&serverAddress.sin_addr.s_addr, (char *)server->h_addr_list[0], server->h_length);

        //create connection
        if (connect(clientSocket,(const struct sockaddr *) &serverAddress, sizeof(serverAddress)) != 0)
        {
            throwException("Error in connect().");
        }

        // SEND PART
        if(arg.type == 0 && arg.command == 1) {   //file and put -- SEND FILE
            std::string filestr(localPath);
            std::string headStr(header);
            // Open file to send, binary
            FILE *fs = fopen(filestr.c_str(), "rb");
            if(fs == NULL)
            {
                throwException("File not found.");
            }
            bzero(buffer, BUFFSIZE);
            int hlen = headStr.length();
            // add header to packet
            if(hlen > 0) {
                strcpy(buffer, headStr.c_str());
            }
            int fs_block_sz = 0;
            if(arg.contentLen == 0) {
                // send empty file if needed
                if(send(clientSocket, buffer, fs_block_sz+hlen, 0) < 0)
                {
                    std::cout << "Failed to send "<< filestr << "." << std::endl;
                    exit(1);
                }
            }

            // read and send whole file
            while((fs_block_sz = fread(buffer+hlen, sizeof(char), BUFFSIZE-hlen, fs))>0)
            {
                if(send(clientSocket, buffer, fs_block_sz+hlen, 0) < 0)
                {
                    std::cout << "Failed to send "<< filestr << "." << std::endl;
                    exit(1);
                }
                bzero(buffer, BUFFSIZE);
                if(hlen > 0) {
                    hlen = 0;
                }
            }
            fclose(fs);
        }else {
            // send header only
            bytestx = send(clientSocket, header, strlen(header), 0);
            if (bytestx < 0) {
                throwException("Error in send().");
            }
        }

        // refresh buffer
        bzero(buffer, BUFFSIZE);
        char buffer2[BUFFSIZE];

        // receive respond from server
        bytesrx = recv(clientSocket, buffer2, BUFFSIZE, 0);
        if(bytesrx <= 0) {
            throwException("No Header recieved");
        }

        std::string rest("");
        int recLen = 0, ff =0;
        parseRecvHead(buffer2, &arg, rest, recLen, ff);
        if(arg.type == 0 && arg.command == 0) {         // file and get -> RECEIVE FILE
            FILE *fr = fopen(arg.file.c_str(), "wb");
            if(fr == NULL)
                throwException("File cannot be opened.");
            else
            {
                bzero(buffer, BUFFSIZE);
                strcpy(buffer, rest.c_str());

                // get binary content from first packet
                for (int i = 0; i < recLen; i++) {
                    *(buffer+i) = *(buffer2+i+ff);
                }
                int write_sz = fwrite(buffer, sizeof(char), recLen, fr);

                bzero(buffer, BUFFSIZE);
                int fr_block_sz = 0;

                // get whole file if content length is bigger than one packet
                while(recLen < arg.contentLen && (fr_block_sz = recv(clientSocket, buffer, BUFFSIZE, 0)) > 0)
                {
                    recLen += BUFFSIZE;
                    write_sz = fwrite(buffer, sizeof(char), fr_block_sz, fr);
                    if(write_sz < fr_block_sz)
                    {
                        throwException("File write fail.");
                    }
                    bzero(buffer, BUFFSIZE);
                    if (fr_block_sz == 0)
                    {
                        break;
                    }
                    bzero(buffer, BUFFSIZE);
                }
                if(fr_block_sz < 0)
                {
                    throwException("Error in receiving.");
                }
                fclose(fr);
            }
        }else {
            std::cout << rest; //show lst
        }

        // cancel connection
        shutdown(clientSocket, 2);
    }
    return 0;
}
