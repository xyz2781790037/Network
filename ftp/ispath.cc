#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
class pathtask
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
    std::string segstrspace(std::string &order,int count = 0){
        while(count <= order.size()){
            if ((order[count] == ' ' && count == 0) || (order[count] == ' ' && count == order.size() - 1)|| (count + 1 < order.size() && order[count] == ' ' && order[count + 1] == ' ')){
                order.erase(count, 1);
            }
            else{
                count++;
            }
        }
        return order;
    }
    std::string handlefilename(std::string &filename){
        size_t STpos = filename.find("STOR");
        if (STpos < 0)
        {
            return;
        }
    }
};
