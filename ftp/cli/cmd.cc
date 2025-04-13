// #pragma once
// #include <iostream>
// #include "handstr.cc"
// class cmd{
//     handstr hs;
//     void stor(std::string &input, std::string &path)
//     {
//         hs.inputseg(path);
//     }

// public:
//     void handcmd(std::string orders,int client_fd){
//         ssize_t cmdspace = orders.find_first_of(' ');
//         std::string order = orders.substr(0, cmdspace);
//         std::string args = orders.substr(cmdspace + 1);
//         if (order == "PASV")
//         {
//         }
//         else if (order == "ACTION")
//         {
//         }
//         else if (order == "LIST")
//         {
//         }
//         else if (order == "STOR")
//         {
//             // stor(client_fd, args);
//         }
//         else if (order == "RETR")
//         {
//         }
//         else if (order == "MKD")
//         {
//             // if (!handp.createDir(args))
//             // {
//             //     perror("mkdir");
//             // }
//         }
//         else if (order == "CWD")
//         {
//             // cwd(args, client_fd);
//         }
//         else
//         {
//             // std::cout << "error input" << std::endl;
//             // sendResponse(500, "invalid commend", client_fd);
//         }
//     }
// };