#include "cmd.h"
#include "threadpool.h"
const int cmdPORT = 1026;
const int dataPORT = 2121;
int ACTIONPORT = 8081;
const int BUFFER_MAXSIZE = 1024;
const std::string UPLOAD_DIR = "./uploads";
const char *SERVER_IP = "127.0.0.1"; // 10.30.0.109
const std::string COLOUR1 = "\033[1;36m";
const std::string COLOUR2 = "\033[0m";
class FTP{
private:
    void cycle(bool &pasv,int &client_fd);
    void setfd(int &fd);
    void sendPort(int &server_fd,int port);

public:
    void run();
};
void FTP::sendPort(int &server_fd, int port){
    std::string portStr = "PORT ";
    portStr += std::to_string(port);
    Net net;
    net.send1(server_fd, portStr, portStr.size(), 0, "recv port");
}
void FTP::setfd(int &fd){
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}
void FTP::run(){
    Net net;
    bool pasv_fd = false;
    int client_fd = net.socket1(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client_addr;
    net.connect1(client_fd, SERVER_IP, client_addr, cmdPORT);
    std::cout << "已连接到服务器" << SERVER_IP << " " << cmdPORT << std::endl;
    cycle(pasv_fd,client_fd);
}
void FTP::cycle(bool &pasv,int &client_fd){
    pool po;
    Net net;
    bool runflag = false;
    std::string input;
    struct sockaddr_in Adata_addr;
    int Adata_fd = socket(AF_INET, SOCK_STREAM, 0);
    setfd(Adata_fd);
    net.binlis(Adata_fd, sizeof(Adata_addr), Adata_addr, 0);
    socklen_t len = sizeof(Adata_addr);
    getsockname(Adata_fd, (struct sockaddr *)&Adata_addr, &len);
    int port = ntohs(Adata_addr.sin_port);
    sendPort(client_fd, port);
    ACTIONPORT = port;
    while (true)
    {
        runflag = false;
        int cdata_fd = 0;
        int data_fd = Adata_fd;

        if (pasv)
        {
            int fd = net.socket1(AF_INET, SOCK_STREAM, 0);
            data_fd = fd;
            struct sockaddr_in data_addr;
            cdata_fd = data_fd;
            net.connect1(data_fd, SERVER_IP, data_addr, dataPORT);
            std::cout << "已连接到服务器" << SERVER_IP << " " << dataPORT << std::endl;
        }
        else{
            std::cout << "action" << std::endl;
            cdata_fd = net.accept1(data_fd, Adata_addr);
            if (cdata_fd <= 0)
            {
                continue;
            }
            std::cout << "一连接到服务器" << SERVER_IP << " " << ACTIONPORT << std::endl;
        }
        std::cout << COLOUR1 << "ftp> " << COLOUR2;
        getline(std::cin, input);
        if (input == "exit")
        {
            if(data_fd != cdata_fd){
                close(data_fd);
            }
            close(cdata_fd);
            break;
        }
        if (input.empty())
        {
            continue;
        }
        std::mutex mtx;
        std::condition_variable cv;
        po.enqueue([client_fd, cdata_fd, input, &pasv, &runflag,&cv,&mtx]() mutable
                            { Cmd cmd;
            cmd.handle_input(input,client_fd,cdata_fd,pasv);
            {
                std::lock_guard<std::mutex> lock(mtx);
                runflag = true;
            }
            cv.notify_one();        
         });
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&runflag]()
                    { return runflag; });
        }
        std::cout << "task end" << std::endl;
        close(data_fd);
    }
    close(client_fd);
}