#define BUFFSIZE 1024

// commands enumeration
enum eComm {
    GET,
    PUT,
    DELETE
};

// types enumeration
enum eType {
    typeFile,
    typeDir
};

// function to get filesize
long fileSize(std::string filename)
{
    FILE *f;
    f=fopen(filename.c_str() , "r");
    if(f == NULL)
    {
        std::cout << "File "<< filename << " not found." << std::endl;
        exit(1);
    }
    fseek(f,0, SEEK_END);
    unsigned long len =(unsigned long)ftell(f);
    fclose(f);
    return len;
}

// function to create Timestamp\r\n
char *createTimestamp() {
    char *ref = (char *)malloc(sizeof(char) * BUFFSIZE);
    time_t timestamp;
    timestamp=time(NULL);
    strcpy(ref,asctime(localtime(&timestamp)));
    *(ref+24) = ' ';
    *(ref+25) = 'C';
    *(ref+26) = 'E';
    *(ref+27) = 'T';
    *(ref+28) = '\r';
    *(ref+29) = '\n';
    *(ref+30) = '\0';
    return ref;
}
