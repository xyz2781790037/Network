#pragma once
#include <iostream>
#include <sys/socket.h>
void sendResponse(int code, const std::string &message, int client_fd)
{
    std::string response = std::to_string(code) + " " + message + "\r";
    send(client_fd, response.c_str(), response.size(), 0);
}