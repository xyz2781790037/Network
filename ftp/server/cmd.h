#include <filesystem>
#include <fcntl.h>
#include <vector>
#include <dirent.h>
#include <string.h>
#include <functional>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include "net.h"
int filesort(const struct dirent **a, const struct dirent **b)
{
    char aw[256], bw[256];
    strncpy(aw, (*a)->d_name + 1, strlen((*a)->d_name) - 1);
    aw[strlen((*a)->d_name) - 1] = '\0';
    strncpy(bw, (*b)->d_name + 1, strlen((*b)->d_name) - 1);
    bw[strlen((*b)->d_name) - 1] = '\0';
    return strcasecmp(aw, bw);
}
class Path
{
    bool isdir(const std::string &path)
    {
        struct stat filestat;
        if (stat(path.c_str(), &filestat) == 0)
        {
            return S_ISDIR(filestat.st_mode);
        }
        return false;
    }

public:
    bool createDir(const std::string &dirpath)
    {
        if (isdir(dirpath))
        {
            return true;
        }
        struct stat filestat;
        if (stat(dirpath.c_str(), &filestat) == 0)
        {
            return false;
        }
        if (mkdir(dirpath.c_str(), 0777) == 0)
        {
            return true;
        }
        return false;
    }
};
class Cmd{
    Net net;
    Path path;
    void stor(int &fd, int &fdd, std::string args);

public:
    void handcmd(std::string orders, int &client_fd, int &data_fd, std::atomic<bool> &pasv);
    void retr(std::string path, int &client_fd, int &data_fd);
    void list(std::string path, int &fd, int &data_fd);
};
void Cmd::handcmd(std::string orders, int &client_fd, int &data_fd, std::atomic<bool> &pasv)
{
    ssize_t cmdspace = orders.find_first_of(' ');
    std::string order = orders.substr(0, cmdspace);
    std::string args = orders.substr(cmdspace + 1);
    if (order == "PASV")
    {
        pasv = true;
    }
    else if (order == "LIST")
    {
        list(args, client_fd, data_fd);
    }
    else if (order == "STOR")
    {
        stor(client_fd, data_fd, args);
    }
    else if (order == "RETR")
    {
        retr(args, client_fd, data_fd);
    }
    else if (order == "MKD")
    {
        chdir("/home/zgyx/Network/ftp/server");
        if (!path.createDir(args))
        {
            perror("mkdir");
            net.send_Response(550, "No such file or directory", client_fd);
            return;
        }
        net.send_Response(250, "Directory created successfully", client_fd);
    }
    else
    {
        std::cout << "error input" << std::endl;
        net.send_Response(500, "invalid commend", client_fd);
    }
}
void Cmd::stor(int &fd, int &fdd, std::string args) {
    size_t filepos = args.find_last_of('/');
    std::string filename = args.substr(filepos + 1);
    std::string path = args.substr(0, filepos);
    std::cout << path << std::endl;
    chdir(path.c_str());
    char filname[filename.size()];
    strcpy(filname, filename.c_str());
    int file_fd = open(filname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd < 0)
    {
        perror("open");
        net.send_Response(550, "Failed to open file for writing.", fd);
        return;
    }
    net.send_Response(150, "The file can be transferred now", fd);
    net.recvfile(fdd, file_fd, 1024);
}
void Cmd::retr(std::string path, int &client_fd, int &data_fd){
    char filname[path.size()];
    strcpy(filname, path.c_str());
    int file_fd = open(filname, O_RDONLY, 0644);
    if (file_fd < 0)
    {
        perror("open");
        return;
    }
    std::cout << "filename :" << filname << std::endl;
    net.send_Response(150, "The file can be transferred now", client_fd);
    struct stat st;
    fstat(file_fd, &st);
    off_t offset = 0;
    ssize_t bytes_sent = 0;
    while (offset < st.st_size){
        bytes_sent = sendfile(data_fd, file_fd, &offset, 1024);
        if(bytes_sent < 0){
            std::cerr << "Error send file" << std::endl;
            close(file_fd);
            return;
        }
        else if(bytes_sent == 0){
            break;
        }
    }
    close(file_fd);
    shutdown(data_fd, SHUT_WR);
}
void Cmd::list(std::string path, int &fd, int &data_fd){
    std::string ppath = "/home/zgyx/Network/ftp/server/";
    net.send_Response(250, "this list is ready", fd);
    path = path.substr(2);
    ppath += path;
    std::cout << "path:" << ppath << std::endl;
    struct stat st;
    char result[1024];
    struct dirent **file;
    int n = scandir(ppath.c_str(), &file, NULL, filesort);
    if(n == -1){
        perror("dir");
    }
    std::string list_one;
    for (int i = 0; i < n; i++)
    {
        memset(result, '\0', strlen(result));
        sprintf(result, "%s/%s", ppath.c_str(), file[i]->d_name);
        list_one.clear();
        if (lstat(result, &st) != -1)
        {
            if (S_ISDIR(st.st_mode)) // 目录
                list_one += "d";
            else if (S_ISLNK(st.st_mode)) // 符号链接
                list_one += "l";
            else
                list_one += "-";
            list_one += (st.st_mode & S_IRUSR) ? "r" : "-";
            list_one += (st.st_mode & S_IWUSR) ? "w" : "-";
            list_one += (st.st_mode & S_IXUSR) ? "x" : "-";
            list_one += (st.st_mode & S_IRGRP) ? "r" : "-";
            list_one += (st.st_mode & S_IWGRP) ? "w" : "-";
            list_one += (st.st_mode & S_IXGRP) ? "x" : "-";
            list_one += (st.st_mode & S_IROTH) ? "r" : "-";
            list_one += (st.st_mode & S_IWOTH) ? "w" : "-";
            list_one += (st.st_mode & S_IXOTH) ? "x" : "-";
            list_one += " ";
            list_one += std::to_string(st.st_nlink);
            list_one += " usr group ";
            list_one += std::to_string(st.st_size);
            char time_str[64];
            struct tm *timeinfo = localtime(&st.st_mtime);
            strftime(time_str, sizeof(time_str), "%m月 %d %H:%M", timeinfo);
            list_one += " ";
            list_one += time_str;
            memset(time_str, '\0', strlen(time_str));
        }
        list_one += " ";
        list_one += file[i]->d_name;
        list_one += "\n";
        char lists[1024];
        strcpy(lists, list_one.c_str());
        std::cout << list_one;
        send(data_fd, lists, list_one.size(), 0);
    }
    shutdown(data_fd, SHUT_WR);
    std::string buf;
    net.recv1(data_fd, buf, 1024, 0);
    std::cout << "send end: " << buf << std::endl;
}
void Net::handle_client(int &data_fd, int &sock_fd, std::string input, std::atomic<bool> &runflag, std::atomic<bool> &pasv)
{
    Cmd cmd;
    cmd.handcmd(input, sock_fd, data_fd, pasv);
    runflag = false;
}