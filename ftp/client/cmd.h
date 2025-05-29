#include <atomic>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <cctype>
#include <string.h>
#include <fcntl.h>
#include "net.h"
class Cmd {
    Net net;
    std::string segstrspace(std::string &order, int count = 0);
    void recvlist(int &data_fd);
    void stor(std::string filename, int &data_fd);
    void retr(std::string args, int &dfd);
    void list(std::string input, int &fd, int &data_fd);
    void create_directory(int &fd, std::string input);
    void stor_helper(std::string input, int &fd, int &data_fd);
    void retr_helper(std::string input, int &fd, int &dfd);
    void transform(std::string &str);

public:
    void handle_input(std::string &input, int &fd, int &data_fd, bool &pasv);
};
void Cmd::transform(std::string &str){
    std::string a;
    bool flag = true;
    for (char c : str)
    {
        if(c != ' ' && flag){
            a += toupper(c);
        }else{
            flag = false;
            a += c;
        }
    }
    str = a;
}
std::string Cmd::segstrspace(std::string &order, int count)
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
void Cmd::handle_input(std::string &input, int &fd, int &data_fd,bool &pasv){
    transform(input);
    input = segstrspace(input);
    size_t pathpos = input.find_first_of(' ');
    std::string order;
    if (pathpos == std::string::npos)
    {
        order = input;
    }
    else
    {
        order = input.substr(0, pathpos);
    }
    if (order == "PASV")
    {
        send(fd, "PASV", 4, 0);
        pasv = true;
    }
    else if (order == "LIST")
    {
        list(input, fd, data_fd);
    }
    else if (order == "STOR")
    {
        stor_helper(input, fd, data_fd);
    }
    else if (order == "RETR")
    {
        retr_helper(input, fd, data_fd);
    }
    else if (order == "MKD")
    {
        create_directory(fd, input);
    }
    else
    {
        std::cout << "error input" << std::endl;
        return;
    }
}
void Cmd::list(std::string input, int &fd, int &data_fd)
{
    int send_bytes = net.send1(fd, input, input.size(), 0, "send1");
    std::string buffer;
    int recv_bytes = net.recv1(fd, buffer, 1024, 0, "recv list");
    recvlist(data_fd);
}
void Cmd::recvlist(int &data_fd)
{
    char buffer[1025];
    ssize_t bytes;
    printf("list start\n");
    while ((bytes = recv(data_fd, buffer, 1024, 0)) > 0)
    {
        buffer[bytes] = '\0';
        std::cout << buffer;
    }
    if (bytes == 0)
    {
        std::string sent = "send ok";
        net.send1(data_fd, sent, sizeof("send ok"), 0, "send ok");
    }
    else if (bytes < 0)
    {
        perror("接收失败");
    }
}
void Cmd::create_directory(int &fd, std::string input){
    int send_bytes = net.send1(fd, input, input.size(), 0, "send");
    std::string buffer;
    ssize_t recv_bytes = net.recv1(fd, buffer, 1024, 0, "recv");
}
void Cmd::stor_helper(std::string input, int &fd, int &data_fd)
{
    size_t pathpos = input.find_first_of(' ');
    size_t pathpos2 = input.find_last_of(' ');
    std::string tmp1 = input.substr(0, pathpos);
    std::string tmp2 = input.substr(pathpos2);
    tmp1 += tmp2;
    ssize_t send_bytes = net.send1(fd, tmp1, tmp1.size(), 0, "send1");
    std::string buffer;
    ssize_t recv_bytes = net.recv1(fd, buffer, 1024, 0, "recv1");
    std::string filename = input.substr(pathpos + 1, pathpos2 - pathpos - 1);
    stor(filename, data_fd);
}
void Cmd::stor(std::string filename, int &data_fd){
    char filname[filename.size()+30];
    strcpy(filname, filename.c_str());
    int file_fd = open(filname, O_RDONLY, 0644);
    if (file_fd < 0)
    {
        perror("open");
        return;
    }
    std::cout << "filename :" << filname << std::endl;
    struct stat st;
    fstat(file_fd, &st);
    off_t offset = 0;
    ssize_t bytes_sent = 0;
    while(offset < st.st_size){
        bytes_sent = sendfile(data_fd, file_fd, &offset, 1024);
        if (bytes_sent < 0)
        {
            std::cerr << "Error send file" << std::endl;
            close(file_fd);
            return;
        }
        else if (bytes_sent == 0)
        {
            break;
        }
    }
    shutdown(data_fd, SHUT_WR);
    close(file_fd);
    std::string buffer;
    ssize_t recv_bytes = net.recv1(data_fd, buffer, 1024, 0, "recv sendfile");
}
void Cmd::retr_helper(std::string input, int &fd, int &dfd){
    size_t filepos = input.find_last_of(' ');
    std::string buffers = input.substr(0, filepos);
    std::string path = input.substr(filepos + 1);
    ssize_t send_bytes = net.send1(fd, buffers, buffers.size(), 0, "send1");
    std::cout << buffers << std::endl;
    std::string str;
    ssize_t recv_bytes = net.recv1(fd, str, 1024, 0, "recv1");
    std::cout << str << std::endl;
    retr(path, dfd);
}
void Cmd::retr(std::string args, int &dfd){
    std::string path = "/home/zgyx/Network/ftp/client/";
    size_t filepos = args.find_last_of('/');
    std::string filename = args.substr(filepos + 1);
    std::string ppath = args.substr(0, filepos);
    path += ppath;
    chdir(path.c_str());
    char filname[filename.size() + 1];
    strcpy(filname, filename.c_str());
    int file_fd = open(filname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd < 0)
    {
        perror("open");
        return;
    }
    net.recvfile(dfd, file_fd, 1024);
}