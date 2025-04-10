#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include "threadpool.hpp"
const int PORT = 2121;
const int BUFFER_MAXSIZE = 1024;
const std::string UPLOAD_DIR = "./uploads";
const char* SERVER_IP = "127.0.0.1";
const std::string COLOUR1 = "\033[1;36m";
const std::string COLOUR2 = "\033[0m";
class handstr{
public:
    std::string segstrspace(std::string &order, int count = 0)
    {
        while (count < order.size())
        {
            if ((order[count] == ' ' && count == 0) || (order[count] == ' ' && count == order.size() - 1) || (count + 1 < order.size() && order[count] == ' ' && order[count + 1] == ' '))
            {
                order.erase(count, 1);
            }
            else
            {
                count++;
            }
        }
        return order;
    }
    std::string inputseg(std::string &input,std::string &path)
    {
        input = segstrspace(input);
        size_t path1pos = input.find_first_of(' ');
        size_t path2pos = input.find_last_of(' ');
        std::string filename = input.substr(path1pos + 1, path2pos - path1pos - 1);
        path = filename;
        std::string in1 = input.substr(0, path1pos);
        std::string in2 = input.substr(path2pos);
        std::string tmp = in1;
        tmp += in2;
        input = tmp;
        return input;
    }
    void filehand(std::string filename,int client_fd){
        int file_fd = open(filename.c_str(), O_RDONLY);
        if (file_fd < 0)
        {
            perror("open");
            return;
        }
        struct stat st;
        fstat(file_fd, &st);
        sendfile(client_fd, file_fd, NULL, st.st_size);
    }
};
void handinput(int fd, std::string input)
{
    handstr hs;
    std::string filename;
    input = hs.inputseg(input,filename);
    ssize_t send_bytes = send(fd, input.c_str(), input.size(), 0);
    if (send_bytes < 0)
    {
        perror("send");
        return;
    }
    char buffer[BUFFER_MAXSIZE];
    ssize_t recv_bytes = recv(fd, buffer, BUFFER_MAXSIZE, 0);
    if (recv_bytes < 0)
    {
        perror("recv");
        return;
    }
    hs.filehand(filename, fd);
}
int main()
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd < 0){
        perror("socket");
        exit(1);
    }
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &client_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(client_fd);
        exit(1);
    }
    if(connect(client_fd,(struct sockaddr *)&client_addr,sizeof(client_addr)) < 0){
        perror("connect");
        close(client_fd);
        exit(1);
    }
    std::cout << "已连接到服务器" << SERVER_IP << " " << PORT << std::endl;
    pool po;
    while(true){
        std::cout << COLOUR1 << "ftp> " << COLOUR2;
        std::string input;
        getline(std::cin, input);
        if(input == "exit"){
            break;
        }
        po.enqueue(handinput, client_fd, input);
    }
    return 0;
}