/*
 *  IPK 2017
 *  Projekt 1
 *  Author: Jan Grossmann
 *
 *  SERVER
 */

#include <string>
#include <regex>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include "both.cpp"


// main struct
struct oper_t {
    eComm command;
    std::string root;
    std::string user;
    std::string remotePath;
    std::string file;
    std::string http;
    std::string error;
    eType type;
    int contentLen;
    int status;
};

// function to refresh struct after each client connection
void refreshOp(struct oper_t *op) {
    op->remotePath = "";
    op->file = "";
    op->http = "";
    op->error = "";
    op->user = "";
    op->status = 0;
    op->contentLen = 0;
}

// function to check if path is a file
bool is_file(const char* path) {
    struct stat sb;
    return ((stat(path, &sb) == 0) && S_ISREG(sb.st_mode));
}

// function to check if path is a directory
bool is_dir(const char* path) {
    struct stat sb;
    return ((stat(path, &sb) == 0) && S_ISDIR(sb.st_mode));

}

// function to print out error
void throwException(std::string ex) {
    std::cerr << ex << std::endl;
    exit(EXIT_FAILURE);
}


// function to parse received packet
int parseRecv(char *recv, struct oper_t *op, std::string &rest, int &recLen, int &ff) {
    std::string recvStr(recv);
    std::string clens(recvStr.substr(recvStr.find("Content-Length: ")+16));
    op->contentLen= atoi(clens.substr(0,clens.find("\r\n")).c_str());

    // find first line of header
    std::string first = recvStr.substr(0,recvStr.find("\r\n"));
    int found = 0;

    // get binary content of first packet
    if((found = recvStr.rfind("\r\n\r\n")) != 0) {
        ff = found+4;
        recLen = BUFFSIZE - found - 4;
        rest = recvStr.substr(found + 4);
        if(op->contentLen > BUFFSIZE) {
            rest.erase(rest.end()-1, rest.end());
        }else {
            recLen = op->contentLen;
        }
    }
    // Match command, GET/ PUT/ DELETE
    if (first.find("GET") == 0){
        op->command = GET;
        first.erase (first.begin(), first.begin()+5); // PUT./
    }else if (first.find("PUT") == 0){
        op->command = PUT;
        first.erase (first.begin(), first.begin()+5);
    }else if (first.find("DELETE") == 0){
        op->command = DELETE;
        first.erase (first.begin(), first.begin()+8);
    }else {
        op->error = "Invalid request (command)";
        return 1;
    }

    //find username
    if ((found = first.find("/")) > 0){
        op->user = first.substr(0,found);
        first.erase(first.begin(), first.begin()+op->user.length());
    }


    // Match filetype
    if (first.find("?type=file")!=std::string::npos) {
        op->type = typeFile;
    }else if (first.find("?type=folder")) {
        op->type = typeDir;
    }else {
        op->error = "Invalid request in remotePath hier";
        return 1;
    }

    // Get filename
    if ((found = first.rfind("?type"))  >= 0) {
        op->file = first.substr(0,found);
        first.erase (first.begin(), first.begin()+found);
    }

    // Match http
    if (first.find("HTTP/1.1")!=std::string::npos) {
        op->http = "HTTP/1.1";
    }else {
        op->error = "Invalid request in HTTP";
        return 1;
    }

    return 0;
}

// function to generate header
char *createHeader(oper_t op, int cLen) {
    char *ref = (char*)malloc(sizeof(char)*BUFFSIZE);

    strcpy(ref,"HTTP/1.1 ");

    // choose correct status code
    switch (op.status) {
        case 200:
            strcat(ref,"200 OK");
            break;
        case 400:
            strcat(ref,"400 Bad Request");
            break;
        case 404:
            strcat(ref,"404 Not Found");
            break;
        default:
            std::string s = std::to_string(op.status);
            strcat(ref,s.c_str());
            strcat(ref," Other");
            break;
    }
    strcat(ref,"\r\nDate: ");

    // paste in mime-type
    strcat(ref,createTimestamp());
    std::string path = "file --mime-type -b ";

    path += op.user;
    path += "/";
    if(op.remotePath != "") {
        path += op.remotePath;
        path += "/";
    }
    path += op.file;

    // get mime-type
    FILE *fp;
    char file_type[40];

    if(op.type == typeFile && op.command == GET) {
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
        strcat(ref,"Content-Type: ");
        strcat(ref, mime.c_str());
        strcat(ref,"\r\n");
    }

    // Add Content-Length
    strcat(ref,"Content-Length: ");
    std::string cLenko = std::to_string(cLen);
    strcat(ref, cLenko.c_str());
    strcat(ref,"\r\n");
    strcat(ref,"\r\n");

    return ref;
}

//============================================================================================================================================
//============================================================================================================================================
// OPERATIONS PART
//============================================================================================================================================
//============================================================================================================================================
int del(struct oper_t op) {

    std::string path = op.root;
    path += "/";
    path += op.user;
    path += "/";
    path += op.remotePath;
    path += "/";
    path += op.file;
    if(is_dir(path.c_str())) { // když REMOTE-PATH ukazuje na adresář, ale je použita operace del, get.

        return 400;
    }else if(!is_file(path.c_str())) { //  když REMOTE-PATH neukazuje na žádny existující objekt při použití operace del, get

        return 404;
    }else if(remove(path.c_str()) == 0){
        return 200;
    }
    return 491;
}

int get(struct oper_t op) {

    std::string path = op.root;
    path += "/";
    path += op.user;
    path += "/";
    path += op.remotePath;
    path += "/";
    path += op.file;
    if(is_dir(path.c_str())) {
        // not a file
        return 400;
    }else if(!is_file(path.c_str())) {
        // file not found
        return 404;
    }
    return 200;
}

int put(struct oper_t op) {

    std::string path = op.root;
    path += "/";
    path += op.user;
    path += "/";
    path += op.remotePath;
    path += "/";
    path += op.file;
    if(is_file(path.c_str())) {
        // already exists
        return 400;
    }
    return 200;
}

int lst(struct oper_t op, std::string &body) {
    DIR *d;
    struct dirent *dir;
    std::string path = op.root;
    path += "/";
    path += op.user;
    path += "/";
    path += op.remotePath;
    path += "/";
    path += op.file;
    if(is_file(path.c_str())) {
        return 400;
    }else if(!is_dir(path.c_str())) {
        return 404;
    }
    d = opendir(path.c_str());
    if(d) {
        while ((dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name,".") && strcmp(dir->d_name,"..")) {
                std::string add(dir->d_name);
                body += add;
                body += "\n";
            }
        }
        closedir(d);
    }
    return 200;
}

int mkd(struct oper_t op) {

    std::string path = op.root;
    path += "/";
    path += op.user;
    path += "/";
    path += op.remotePath;

    if (is_dir(path.c_str())) {
        path += "/";
        path += op.file;
        if (!is_dir(path.c_str())) {
            mkdir(path.c_str(), 0700);
        }else {
            //op.error ="Already exists.";
            return 400;
        }
    }else {
        //op.error ="Chybi soubory k ";
        return 404;
    }
    return 200;
}

int rmd(struct oper_t op) {

    std::string path = op.root;
    path += "/";
    path += op.user;
    path += "/";
    path += op.remotePath;
    path += "/";
    path += op.file;
    if(strcmp(op.file.c_str(), "/")==0) {
        return 492;
    }
    if(is_file(path.c_str())) {
        return 400;
    }else if(!is_dir(path.c_str())) {
        return 404;
    }
    if(rmdir(path.c_str()) != 0) {
        return 492;
    }
    return 200;
}

int checkUser(struct oper_t op) {

    std::string path = op.root;
    path += "/";
    path += op.user;
    if (!is_dir(path.c_str())) {
        // account not found
        return 490;
    }
    return 1;
}

//============================================================================================================================================
//============================================================================================================================================
// MAIN
//============================================================================================================================================
//============================================================================================================================================


int main(int argc, char ** argv) {
    int portNum = 6677;
    std::string rootFolder;
    //char *rootFolder;
    char ucwd[1024];
    getcwd(ucwd, sizeof(ucwd));
    std::string cwd(ucwd);
    struct oper_t op;

    // Argc check
    if(argc != 1 && argc != 3 && argc != 5) {
        throwException("Error argc.");
    }else if(argc == 1) {
        ;
    }else {
        if(strcmp(argv[1],"-p") == 0) {
            portNum = (int) strtol(argv[2], (char **)NULL, 10);
        }else if(argc > 3 && strcmp(argv[3],"-p") == 0) {
            portNum = (int) strtol(argv[4], (char **)NULL, 10);
        }
        if(strcmp(argv[1],"-r") == 0) {
            rootFolder = argv[2];
        }else if(argc > 3 && strcmp(argv[3],"-r") == 0) {
            rootFolder = argv[4];
        }
    }

    // if root folder is not set, root folder is cwd
    if(rootFolder == "") {
        rootFolder = cwd;
    }

    // check if rootfolder exists
    if(!is_dir(rootFolder.c_str())) {
        throwException("-r is not directory");
    }

    op.root = rootFolder;
    //std::cout << "RootFolder "<< rootFolder << std::endl;

    // Server part
    int rc, welcome_socket, comm_socket;
    socklen_t sa_client_len;
    struct sockaddr_in sa;
    struct sockaddr_in sa_client;

    // check port number range
    if(portNum < 1023 || portNum > 65536) {
        throwException("Error in port.");
    }
    memset(&sa,0,sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons(portNum);

    // check error in socket
    welcome_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(welcome_socket < 0) {
        fprintf(stderr,"Error in socket().\n");
        exit(EXIT_FAILURE);
    }

    // bind part
    if((rc = bind(welcome_socket, (struct sockaddr*)&sa, sizeof(sa))) < 0) {
        fprintf(stderr,"Error in bind().\n");
        exit(EXIT_FAILURE);
    }

    // listen part
    if((listen(welcome_socket, 1)) < 0) {
        fprintf(stderr,"Error in listen().\n");
        exit(EXIT_FAILURE);
    }

    //
    //printf("Server is running on port %d\n", portNum);
    sa_client_len = sizeof(sa_client);

    // accept part
    while (1) {
        // accept connection
        comm_socket = accept(welcome_socket, (struct sockaddr*)&sa_client, &sa_client_len);
        if(comm_socket > 0) {

            char buffer[BUFFSIZE];
            int res = 0;
            for(;;) {
                // fresh the buffers
                bzero(buffer, BUFFSIZE);
                char buffer2[BUFFSIZE+1];
                bzero(buffer, BUFFSIZE+1);
                // refresh main structure with each loop
                refreshOp(&op);
                res = recv(comm_socket, buffer2, BUFFSIZE, 0);
                if(res <= 0) {
                    break;
                }else {
                    std::string rest = "";
                    //std::cout << buffer << std::endl;
                    int recLen = 0, ff = 0;
                    // receive first packet
                    if(parseRecv(buffer2, &op, rest, recLen, ff)) {
                        std::cout << "There is an error: " << op.error << std::endl;
                    }else {
                        if(checkUser(op) == 1){

                            bzero(buffer, BUFFSIZE);
                            switch (op.command) {
                                case 0:                     // get
                                    if(op.type) {
                                        // lst command
                                        std::string ls;
                                        op.status = lst(op, ls);
                                        strcpy(buffer, createHeader(op, ls.length()));
                                        strcat(buffer, ls.c_str());
                                        send(comm_socket, buffer, BUFFSIZE, 0);
                                    }else {
                                        // get command
                                        op.status = get(op);
                                        std::string path = op.root;
                                        path += "/";
                                        path += op.user;
                                        path += op.remotePath;
                                        path += op.file;
                                        std::string headStr(createHeader(op, fileSize(path)));

                                        // open file to send
                                        FILE *fs = fopen(path.c_str(), "rb");
                                        if(fs == NULL)
                                        {
                                            std::cerr << "File "<< path << " not found." << std::endl;
                                            exit(1);
                                        }

                                        bzero(buffer, BUFFSIZE);
                                        int hlen = headStr.length();
                                        if(hlen > 0) {
                                            strcpy(buffer, headStr.c_str());
                                        }
                                        int fs_block_sz = 0;

                                        // send empty file
                                        if(op.contentLen == 0) {
                                            if(send(comm_socket, buffer, fs_block_sz+hlen, 0) < 0)
                                            {
                                                std::cerr << "Failed to send "<< path << "." << std::endl;
                                                exit(1);
                                            }
                                        }

                                        // while whole file is not read, read packet/BUFFSIZE and send it
                                        while((fs_block_sz = fread(buffer+hlen, sizeof(char), BUFFSIZE-headStr.length(), fs))>0)
                                        {
                                            if(send(comm_socket, buffer, fs_block_sz+hlen, 0) < 0)
                                            {
                                                std::cout << "Failed to send "<< path << "." << std::endl;
                                                exit(1);
                                            }
                                            bzero(buffer, BUFFSIZE);
                                            if(hlen > 0) {
                                                hlen = 0;
                                            }
                                        }
                                        // don't forget to close file
                                        fclose(fs);
                                    }
                                    break;
                                case 1:                     // put
                                    if(op.type) {
                                        // mkd command
                                        op.status = mkd(op);
                                        strcpy(buffer, createHeader(op, 0));
                                        send(comm_socket, buffer, BUFFSIZE, 0);
                                    }else {
                                        // put command
                                        op.status = put(op);
                                            std::string path = op.root;
                                            path += "/";
                                            path += op.user;
                                            path += op.remotePath;
                                            path += op.file;
                                            // open file to write
                                            FILE *fr = fopen(path.c_str(), "wb");

                                            // same implementation as on client side
                                            if(fr == NULL)
                                                std::cout << "File cannot be opened " << path << std::endl;
                                            else
                                            {

                                                bzero(buffer, BUFFSIZE);
                                                strcpy(buffer, rest.c_str());

                                                for (int i = 0; i < recLen; i++) {
                                                    *(buffer+i) = *(buffer2+i+ff);
                                                }
                                                int write_sz;
                                                if(op.status == 200) {
                                                    write_sz = fwrite(buffer, sizeof(char), recLen, fr);
                                                }

                                                bzero(buffer, BUFFSIZE);
                                                int fr_block_sz = 0;
                                                while(recLen < op.contentLen && (fr_block_sz = recv(comm_socket, buffer, BUFFSIZE, 0)) > 0)
                                                {
                                                    recLen += BUFFSIZE;
                                                    if(op.status == 200) {
                                                        write_sz = fwrite(buffer, sizeof(char), fr_block_sz, fr);
                                                    }
                                                    if(write_sz < fr_block_sz)
                                                    {
                                                        std::cerr << "File write fail" << std::endl;
                                                        break;
                                                    }
                                                    bzero(buffer, BUFFSIZE);
                                                    if (fr_block_sz == 0)
                                                    {
                                                        std::cerr << "ERR here" << std::endl;
                                                        break;
                                                    }
                                                    bzero(buffer, BUFFSIZE);
                                                }
                                                if(fr_block_sz < 0)
                                                {
                                                    std::cout << "Error in receiving" << std::endl;
                                                    return 1;
                                                }
                                                fclose(fr);
                                            }
                                        //}
                                        strcpy(buffer, createHeader(op, 0));
                                        send(comm_socket, buffer, BUFFSIZE, 0);
                                    }
                                    break;
                                case 2:                     // delete
                                    if(op.type) {
                                        // rmd command
                                        op.status = rmd(op);
                                        strcpy(buffer, createHeader(op, 666));
                                        send(comm_socket, buffer, BUFFSIZE, 0);
                                    }else {
                                        // del command
                                        op.status = del(op);
                                        strcpy(buffer, createHeader(op, 666));
                                        send(comm_socket, buffer, BUFFSIZE, 0);
                                    }
                                    break;
                                default: break;
                            }
                        }else {
                            // send user acc not found
                            op.status = checkUser(op);
                            strcpy(buffer, createHeader(op, 0));
                            send(comm_socket, buffer, BUFFSIZE, 0);
                        }

                    }
                }
            }
        }
    }

    return 0;
}
