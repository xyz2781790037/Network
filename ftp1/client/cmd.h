#include <atomic>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include "net.h"
class Cmd {
public:
    void handle_input();
};