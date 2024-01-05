#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

const int BACKLOG_QUEUE_SIZE = 5; 
const int RESPONSE_SIZE = 10;

int main(void){
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    int openedfd = 0;
    const char* text = "Hello World";

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
        openedfd = accept(socketfd, 0, 0);
        if(openedfd == -1){
            std::cout << "Connection was refused" << std::endl;
        }else{
            std::cout << "Port opened" << std::endl;

            char buffer[RESPONSE_SIZE] = "";
            while(recv(openedfd, buffer, RESPONSE_SIZE, 0)){
                std::cout << "recv: " << buffer << std::endl;
                for(int i = 0; i < RESPONSE_SIZE; i++){
                    buffer[i] = 0;
                }
            }
            
            send(openedfd, text, 11, 0);
            close(openedfd);
        }
    }

    return 0;
}
