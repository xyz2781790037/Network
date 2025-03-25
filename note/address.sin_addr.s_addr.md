### **1. 监听所有可用地址（0.0.0.0）**

如果你要让服务器监听 **所有可用的网络接口**，可以写：

```c
address.sin_addr.s_addr = INADDR_ANY;
```

`INADDR_ANY` 的值是 `0.0.0.0`，表示**所有可用的 IP 地址**（本机所有网卡）。

这样，服务器就能接受来自任何网卡的连接。

### **2. 监听本地回环地址（127.0.0.1）**

如果服务器只需要监听 **本地回环地址**（即本机访问，外部无法连接），可以写：

```c
address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
```

`INADDR_LOOPBACK` 代表 `127.0.0.1`（仅限本机访问）。

`htonl()` 用于转换为网络字节序。

### **3. 指定某个具体的 IP 地址**

如果服务器只想监听**特定的 IP 地址**（如 `192.168.1.100`），可以使用 `inet_pton()`：

```c
inet_pton(AF_INET, "192.168.1.100", &address.sin_addr);
```

`inet_pton()` 将 **点分十进制的 IP 地址** 转换为 **二进制格式** 并存入 `sin_addr` 结构体中。

适用于监听 **指定网卡**，或者客户端连接 **指定服务器**。

### **4. 连接广播地址**

如果程序需要向**网络中的所有设备**发送广播数据，可以使用：

```c
address.sin_addr.s_addr = htonl(INADDR_BROADCAST);
```

`INADDR_BROADCAST` 代表 `255.255.255.255`，表示广播地址。

需要 **启用 socket 广播选项**：

```c
int broadcastEnable = 1;
setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
```

### **5. 连接多播地址**

如果要加入某个 **多播组**（比如 `224.0.0.1`），可以使用：

```c
inet_pton(AF_INET, "224.0.0.1", &address.sin_addr);
```

**多播地址范围**：`224.0.0.0` ~ `239.255.255.255`

需要设置 `IP_ADD_MEMBERSHIP` 选项来加入多播组。