#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
void sendResponse(int code, const std::string &message, int client_fd)
{
    std::string response = std::to_string(code) + " " + message + "\r\n";
    int a = send(client_fd, response.c_str(), response.size(), 0);
    if( a <= 0){
        perror("send _res");
        exit(1);
    }
    std::cout << "send :" << response << std::endl;
}
class secv
{
public:
    int recv1(int &fd, char *buf, size_t n, int flags)
    {
        int recv_cmd = recv(fd, buf, n, flags);
        if (recv_cmd <= 0)
        {
            perror("recvcmd");
            sendResponse(421, "Control connection error", fd);
            close(fd);
        }
        buf[recv_cmd] = '\0';
        return recv_cmd;
    }
    int send1(int &read_fd, int &write_fd, size_t n)
    {
        char buffer[1024];
        int read_bytes = 0;
        std::cout << "reading " << std::endl;
        while ((read_bytes = read(read_fd, buffer, n)) > 0)
        {
            if (write(write_fd, buffer, read_bytes) != read_bytes)
            {
                perror("write");
                close(write_fd);
                sendResponse(550, "Write error during file transfer.", read_fd);
                return -1;
            }
        }
        if (read_bytes == 0)
        {
            // 文件读完了
            sendResponse(226, "Transfer complete.", read_fd);
            return 0;
        }
        else
        {
            // 读出错
            sendResponse(451, "Read error during file transfer.", read_fd);
            return -1;
        }
    }
};