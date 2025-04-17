#include "net.hpp"
const int dataPORT = 2121;
const int cmdPORT = 1026;
const std::string UPLOAD_DIR = "./uploads";
int main(){
    pathtask pt;
    pt.createDir(UPLOAD_DIR);
    Net net1,net2;
    int server_fd = net1.socket1(AF_INET, SOCK_STREAM, 0);

    int data_fd = net2.socket1(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_addr,data_addr;
    net1.binlis(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr), server_addr, cmdPORT);
    net2.binlis(data_fd, (struct sockaddr *)&data_addr, sizeof(data_addr), data_addr, dataPORT);
    std::cout << "listen : " << dataPORT << "&" << cmdPORT << std::endl;
    pool po;
    while(true){
        struct sockaddr_in client_addr;
        struct sockaddr_in cdata_addr;
        int client_fd = net1.accept1(server_fd, client_addr);
        int cdata_fd = net2.accept1(data_fd, cdata_addr);
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "Connecting from :" << client_ip << std::endl;
        po.enqueue([client_fd, cdata_fd, &net1, &net2]()
                   { 
            char buffer[1024];
            net1.recmd(client_fd,buffer);
            std::cout << buffer << std::endl;
            net2.handclient(cdata_fd,client_fd,buffer); });
    }
    close(data_fd);
    close(server_fd);
    return 0;
}