#include "net.hpp"
#include <string>
#include "threadpool.hpp"
const int cmdPORT = 1026;
const int dataPORT = 2121;
const int BUFFER_MAXSIZE = 1024;
const std::string UPLOAD_DIR = "./uploads";
const char *SERVER_IP = "127.0.0.1";
const std::string COLOUR1 = "\033[1;36m";
const std::string COLOUR2 = "\033[0m";
int main()
{
    Net net1, net2;
    std::atomic<bool> runflag = false;
    int client_fd = net1.socket1(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client_addr, data_addr;
    net1.connect1(AF_INET, SERVER_IP, client_addr, cmdPORT);
    std::cout << "已连接到服务器" << SERVER_IP << " " << cmdPORT << std::endl;
    pool po;
    std::string input;
    while (true)
    {
        runflag = true;
        int data_fd = net2.socket1(AF_INET, SOCK_STREAM, 0);
        net2.connect1(AF_INET, SERVER_IP, data_addr, dataPORT);
        std::cout << "已连接到服务器" << SERVER_IP << " " << dataPORT << std::endl;
        std::cout << COLOUR1 << "ftp> " << COLOUR2;
        getline(std::cin, input);
        if (input == "exit")
        {
            close(data_fd);
            break;
        }
        if (input.empty())
        {
            continue;
        }
        po.enqueue([&net1, client_fd, data_fd, input,&runflag]() mutable
                   { net1.handinput(client_fd, data_fd, input,runflag); });
        while (runflag)
        {
            
        }
        close(data_fd);
    }
    close(client_fd);
    return 0;
}