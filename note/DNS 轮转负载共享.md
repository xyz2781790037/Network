# DNS 轮转负载共享
### 1. 什么是 DNS 轮转负载共享？

DNS 轮转（Round-Robin）是一种利用 **DNS 服务器** 来均衡流量的负载均衡策略。它的基本原理是：

- 一个 **域名**（比如 `example.com`）可以对应多个 **IP 地址**（即多台服务器）。
- 当用户访问 `example.com` 时，DNS 服务器会返回其中的一个 IP 地址，并且每次返回的 IP **按顺序轮换**，这样不同的用户会被引导到不同的服务器上。

> DNS（Domain Name System，域名系统）服务器是一种用于解析域名的服务器。它的主要作用是 将人类可读的域名（如 www.example.com）转换为计算机可识别的 IP 地址（如 192.168.1.1 或 2001:db8::ff00:42:8329），从而使计算机能够相互通信。

### **2. 作用**

- **负载均衡**：因为不同用户可能会被引导到不同的服务器，从而实现一定程度的负载均衡，防止某一台服务器过载。
- **提高可用性**：即使其中一台服务器宕机，仍然有其他服务器可以提供服务。

### **3. 限制**

虽然 DNS 轮转是一种简单的方法，但它也有一些缺点：

- **不考虑服务器健康状态**：如果某台服务器宕机了，DNS 仍然可能会返回它的 IP，导致部分用户访问失败。
- **缓存问题**：DNS 解析结果可能会被缓存，导致负载均衡效果受影响。
- **无法精确控制流量**：无法基于服务器的负载情况来动态调整请求分配。