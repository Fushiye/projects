/* 网络模块 
 * 处理所有与网络相关的操作，包括服务器 socket 的创建、绑定端口、监听连接、接受客户端连接等。
 * 核心功能:
 * - 创建服务器 socket (`create_server_socket`)
 * - 监听和接受客户端连接 (`accept_client_connection`) 
 */

#include "network.h"


void error_die(const char *func,const char *sc)
{

  perror(sc); // 使用当前 errno 值打印错误消息   
  exit(EXIT_FAILURE); // 使用 EXIT_FAILURE 宏来表示程序失败退出  

}


/// @brief 服务器启动函数，包括创建套接字、绑定地址和端口，开启端口监听
/// @param port 监听的端口，连接的端口
/// @return
int startup(void)
{
  u_short port = 44231;        // 定义服务端监听端口，原实现采用自动分配的端口
  struct sockaddr_in hostName; // IPv4套接字地址结构，内含地址族、端口号和地址

  // 1. 创建套接字
  // 参数分别是协议族、套接字类型、协议，返回值-1表示失败
  // AF_INET表示IPv4，SOCK_STREAM表示基于TCP的流式套接字，当前二者可以确定协议时第三个参数可以为0
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  // 错误时退出
  if (server_socket == -1)
    error_die("startup","Socket creation failed");

  // 设置 SO_REUSEADDR 选项，允许在TIME_WAIT状态下快速重用端口
  int optval = 1;
  if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    error_die("startup","setsockopt failed");

  // 2. 绑定地址和端口
  // 初始化地址结构体
  memset(&hostName, 0, sizeof(hostName));       // 确保结构体没有未初始化的值
  hostName.sin_family = AF_INET;                // 使用IPv4地址
  hostName.sin_port = htons(port);              // 使用固定端口，htons函数将主机字节序的端口号转换为网络字节序
  hostName.sin_addr.s_addr = htonl(INADDR_ANY); // 设置INADDR_ANY（通常表示 0.0.0.0）服务器将监听所有可用的网络接口同样转换为网络字节序

  // 绑定地址与 socket
  if (bind(server_socket, (struct sockaddr *)&hostName, sizeof(hostName)) < 0)
    error_die("startup","bind failed");

  // 3. 设置监听
  if (listen(server_socket, 5) < 0)
    error_die("startup","listen  failed");
  printf("httpServer running on port %d\n", port);
  return server_socket;
}
