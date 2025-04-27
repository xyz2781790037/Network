#include "cmd.h"
#include "threadpool.hpp"
const int cmdPORT = 1026;
const int dataPORT = 2121;
const int BUFFER_MAXSIZE = 1024;
const std::string UPLOAD_DIR = "./uploads";
const char *SERVER_IP = "127.0.0.1";
const std::string COLOUR1 = "\033[1;36m";
const std::string COLOUR2 = "\033[0m";
class FTP{
private:
    void cycle(std::atomic<bool> &pasv,int &client_fd);

public:
    void run();
};
void FTP::run(){
    Net net;
    std::map<int, std::atomic<bool>> pasv_fd;
    int client_fd = net.socket1(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client_addr;
    net.connect1(client_fd, SERVER_IP, client_addr, cmdPORT);
    std::cout << "已连接到服务器" << SERVER_IP << " " << cmdPORT << std::endl;
    pasv_fd[client_fd] = false;
    cycle(pasv_fd[client_fd],client_fd);
}
void FTP::cycle(std::atomic<bool> &pasv,int &client_fd){
    pool po;
    Net net;
    std::string input;
    std::atomic<bool> runflag = false;
    while(true){
        runflag = true;
        struct sockaddr_in data_addr;
        int data_fd = net.socket1(AF_INET, SOCK_STREAM, 0);
        int cdata_fd = 0;
        if (pasv){
            cdata_fd = data_fd;
            net.connect1(data_fd, SERVER_IP, data_addr, dataPORT);
            std::cout << "已连接到服务器" << SERVER_IP << " " << dataPORT << std::endl;
        }
        else{
            net.binlis(data_fd, sizeof(data_addr), data_addr, dataPORT);
            cdata_fd = net.accept1(data_fd, data_addr);
            if (cdata_fd <= 0)
            {
                continue;
            }
            std::cout << "一连接到服务器" << SERVER_IP << " " << dataPORT << std::endl;
        }
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
        input += "\r\n";
        po.enqueue([client_fd, data_fd, input, &runflag,&pasv]() mutable
                   { Cmd cmd;
            cmd.handle_input(input,client_fd,data_fd,pasv);
            runflag = false;
                 });
        while (runflag)
        {
            sleep(1);
        }
        close(data_fd);
    }
    close(client_fd);
}