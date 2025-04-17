#pragma once
#include "secv.hpp"
#include <filesystem>
#include <fcntl.h>
#include <vector>
#include <dirent.h>
#include <string.h>
#include <functional>
#include "ispath.hpp"
class cmd
{
    secv refile;
    pathtask handp;
    void stor(int fd,int fdd,std::string args){
        size_t filepos = args.find_last_of('/');
        std::string filename = args.substr(filepos + 1);
        std::string path = args.substr(0, filepos);
        cwd(path, fd);
        int file_fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (file_fd < 0)
        {
            perror("open");
            sendResponse(550, "Failed to open file for writing.",fd);
            return;
        }
        sendResponse(150, "The file can be transferred now", fd);
        refile.send1(fdd, file_fd, 1024);
    }
    void cwd(std::string args,int client_fd){
        send(client_fd, "uping...", 8, 0);
        if (chdir(args.c_str()) == -1)
        {
            sendResponse(550, "Failed to change directory.", client_fd); // 切换失败
        }
        else
        {
            sendResponse(250, "Directory successfully changed.", client_fd); // 成功
        }
    }
    static int filesort(const struct dirent **a, const struct dirent **b){
        char aw[256], bw[256];
        strncpy(aw, (*a)->d_name + 1, strlen((*a)->d_name) - 1);
        aw[strlen((*a)->d_name) - 1] = '\0';
        strncpy(bw, (*b)->d_name + 1, strlen((*b)->d_name) - 1);
        bw[strlen((*b)->d_name) - 1] = '\0';
        return strcasecmp(aw,bw);
    }
    void list(std::string path, int fd)
    {
        sendResponse(250, "this list is ready", fd);
        struct stat st;
        char result[1024];
        struct dirent **file;
        int n = scandir(path.c_str(), &file, NULL, filesort);
        for (int i = 0;i < n;i++){
            memset(result, '\0', strlen(result));
            sprintf(result, "%s/%s", path.c_str(), file[i]->d_name);
            if (lstat(result,&st) != -1){
                if (S_ISDIR(st.st_mode)) // 目录
                    dprintf(fd,"d");
                else if (S_ISLNK(st.st_mode)) // 符号链接
                    dprintf(fd,"l");
                else
                    dprintf(fd,"-");
                dprintf(fd,(st.st_mode & S_IRUSR) ? "r" : "-");
                dprintf(fd,(st.st_mode & S_IWUSR) ? "w" : "-");
                dprintf(fd,(st.st_mode & S_IXUSR) ? "x" : "-");
                dprintf(fd,(st.st_mode & S_IRGRP) ? "r" : "-");
                dprintf(fd,(st.st_mode & S_IWGRP) ? "w" : "-");
                dprintf(fd,(st.st_mode & S_IXGRP) ? "x" : "-");
                dprintf(fd,(st.st_mode & S_IROTH) ? "r" : "-");
                dprintf(fd,(st.st_mode & S_IWOTH) ? "w" : "-");
                dprintf(fd,(st.st_mode & S_IXOTH) ? "x" : "-");
                dprintf(fd," %*lu", 3, st.st_nlink);
                dprintf(fd," %s", "usr"); //
                dprintf(fd," %s", "group");
                dprintf(fd," %*lu", 10, st.st_size);
                char time_str[20];
                struct tm *time = localtime(&st.st_mtime);
                strftime(time_str, sizeof(time_str), "%m月 %d %H:%M", time);
                dprintf(fd," %s ", time_str);
            }
            dprintf(fd, "%s\n", file[i]->d_name);
        }
    }

public:
    void handcmd(std::string orders,int client_fd,int data_fd){
        ssize_t cmdspace = orders.find_first_of(' ');
        std::string order = orders.substr(0, cmdspace);
        std::string args = orders.substr(cmdspace + 1);
        if (order == "PASV"){
        }
        else if(order == "ACTION"){
            
        }
        else if (order == "LIST"){
            list(args,client_fd);
        }
        else if (order == "STOR"){
            stor(client_fd, data_fd, args);
        }
        else if(order == "RETR"){

        }
        else if(order == "MKD"){
            if(!handp.createDir(args)){
                perror("mkdir");
            }
        }
        else if (order == "CWD")
        {
            cwd(args,client_fd);
        }
        else{
            std::cout << "error input" << std::endl;
            sendResponse(500, "invalid commend", client_fd);
        }
    }
};