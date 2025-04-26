#include "secv.hpp"
#include <filesystem>
#include <fcntl.h>
#include <vector>
#include <dirent.h>
#include <string.h>
#include <functional>
#include <atomic>
#include <sys/sendfile.h>
#include "ispath.hpp"
class cmd
{
    secv refile;
    pathtask handp;
    void stor(int fd, int fdd, std::string args)
    {
        size_t filepos = args.find_last_of('/');
        std::string filename = args.substr(filepos + 1);
        std::string path = args.substr(0, filepos);
        chdir(path.c_str());
        char filname[filename.size()];
        strcpy(filname, filename.c_str());
        int file_fd = open(filname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (file_fd < 0)
        {
            perror("open");
            sendResponse(550, "Failed to open file for writing.", fd);
            return;
        }
        sendResponse(150, "The file can be transferred now", fd);
        refile.send1(fdd, file_fd, 1024);
    }
    void cwd(std::string args, int client_fd)
    {
        // sendResponse("")
        if (chdir(args.c_str()) == -1)
        {
            sendResponse(550, "Failed to change directory.", client_fd); // 切换失败
        }
        else
        {
            sendResponse(250, "Directory successfully changed.", client_fd); // 成功
        }
    }
    static int filesort(const struct dirent **a, const struct dirent **b)
    {
        char aw[256], bw[256];
        strncpy(aw, (*a)->d_name + 1, strlen((*a)->d_name) - 1);
        aw[strlen((*a)->d_name) - 1] = '\0';
        strncpy(bw, (*b)->d_name + 1, strlen((*b)->d_name) - 1);
        bw[strlen((*b)->d_name) - 1] = '\0';
        return strcasecmp(aw, bw);
    }
    void list(std::string path, int &fd,int &data_fd)
    {
        sendResponse(250, "this list is ready", fd);
        std::cout << data_fd << std::endl;
        struct stat st;
        char result[1024];
        struct dirent **file;
        int n = scandir(path.c_str(), &file, NULL, filesort);
        for (int i = 0; i < n; i++)
        {
            memset(result, '\0', strlen(result));
            sprintf(result, "%s/%s", path.c_str(), file[i]->d_name);
            if (lstat(result, &st) != -1)
            {
                if (S_ISDIR(st.st_mode)) // 目录
                    dprintf(data_fd, "d");
                else if (S_ISLNK(st.st_mode)) // 符号链接
                    dprintf(data_fd, "l");
                else
                    dprintf(data_fd, "-");
                dprintf(data_fd, (st.st_mode & S_IRUSR) ? "r" : "-");
                dprintf(data_fd, (st.st_mode & S_IWUSR) ? "w" : "-");
                dprintf(data_fd, (st.st_mode & S_IXUSR) ? "x" : "-");
                dprintf(data_fd, (st.st_mode & S_IRGRP) ? "r" : "-");
                dprintf(data_fd, (st.st_mode & S_IWGRP) ? "w" : "-");
                dprintf(data_fd, (st.st_mode & S_IXGRP) ? "x" : "-");
                dprintf(data_fd, (st.st_mode & S_IROTH) ? "r" : "-");
                dprintf(data_fd, (st.st_mode & S_IWOTH) ? "w" : "-");
                dprintf(data_fd, (st.st_mode & S_IXOTH) ? "x" : "-");
                dprintf(data_fd, " %*lu", 3, st.st_nlink);
                dprintf(data_fd, " %s", "usr"); //
                dprintf(data_fd, " %s", "group");
                dprintf(data_fd, " %*lu", 10, st.st_size);
                char time_str[20];
                struct tm *time = localtime(&st.st_mtime);
                strftime(time_str, sizeof(time_str), "%m月 %d %H:%M", time);
                dprintf(data_fd, " %s ", time_str);
            }
            dprintf(data_fd, "%s\n", file[i]->d_name);
        }
        // shutdown(data_fd, SHUT_WR);
        std::string buf;
        refile.recv1(data_fd, buf, 1024, 0);
        std::cout << buf << std::endl;
    }
    void retr(std::string path,int &client_fd,int &data_fd){
        
        char filname[path.size()];
        strcpy(filname, path.c_str());
        int file_fd = open(filname, O_RDONLY, 0644);
        if (file_fd < 0)
        {
            perror("open");
            return;
        }
        std::cout << "filename :" << filname << std::endl;
        sendResponse(150, "The file can be transferred now", client_fd);
        struct stat st;
        fstat(file_fd, &st);
        int bytes = sendfile(data_fd, file_fd, NULL, st.st_size);
    }

public:
    void handcmd(std::string orders, int client_fd, int data_fd,std::atomic<bool> &flag)
    {
        ssize_t cmdspace = orders.find_first_of(' ');
        std::string order = orders.substr(0, cmdspace);
        std::string args = orders.substr(cmdspace + 1);
        if (order == "PASV")
        {
        }
        else if (order == "LIST")
        {
            list(args, client_fd,data_fd);
        }
        else if (order == "STOR")
        {
            stor(client_fd, data_fd, args);
        }
        else if (order == "RETR")
        {
            retr(args,client_fd,data_fd);
        }
        else if (order == "MKD")
        {
            chdir("/home/zgyx/Network/ftp/ser");
            if (!handp.createDir(args))
            {
                perror("mkdir");
                sendResponse(550, "No such file or directory", client_fd);
            }
            sendResponse(250, "Directory created successfully", client_fd);
        }
        else
        {
            std::cout << "error input" << std::endl;
            sendResponse(500, "invalid commend", client_fd);
        }
    }
};