#include "net.cc"
#include <string>
#include "threadpool.hpp"
const int cmdPORT = 1026;
const int dataPORT = 2121;
const int BUFFER_MAXSIZE = 1024;
const std::string UPLOAD_DIR = "./uploads";
const char* SERVER_IP = "127.0.0.1";
const std::string COLOUR1 = "\033[1;36m";
const std::string COLOUR2 = "\033[0m";
int main()
{
    Net net1, net2;
    int client_fd = net1.socket1(AF_INET, SOCK_STREAM, 0);
    int data_fd = net2.socket1(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client_addr, data_addr;
    net1.connect1(AF_INET, SERVER_IP, client_addr, cmdPORT);
    net2.connect1(AF_INET, SERVER_IP, data_addr, dataPORT);
    std::cout << "已连接到服务器" << SERVER_IP << " " << cmdPORT << std::endl;
    std::cout << "已连接到服务器" << SERVER_IP << " " << dataPORT << std::endl;
    pool po;
    std::string input;
    while(true){
        std::cout << COLOUR1 << "ftp> " << COLOUR2;
        getline(std::cin, input);
        if(input == "exit"){
            break;
        }
        if(input.empty()){
            continue;
        }
        po.enqueue([&net1, client_fd, data_fd, input]() mutable
                   { net1.handinput(client_fd, data_fd, input); });
    }
    close(client_fd);
    close(data_fd);
    return 0;
}