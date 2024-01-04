#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

const int BACKLOG_QUEUE_SIZE = 5; 

int main(void){
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    int opened_fd = 0;

    sockaddr_in address = {
        AF_INET,
        htons(80),
        {INADDR_ANY},
        {0}
    }; 

    if(bind(socketfd, (sockaddr*)&address, sizeof(address)) == -1){
        std::cout << "Error while binding to an adress. Error Number: " << errno << std::endl;
        return 1;
    }

    if(listen(socketfd, BACKLOG_QUEUE_SIZE) == -1){
        std::cout << "Error listening to the specifed port" << std::endl;
        return 1;
    }

    while(1){
        std::cout << "Waiting for connection..." << std::endl;
        opened_fd = accept(socketfd, 0, 0);
        if(opened_fd == -1){
            std::cout << "Connection was refused" << std::endl;
        }else{
            std::cout << "Port opened" << std::endl;
        }
    }

    return 0;
}
