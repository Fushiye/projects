#include "network.h" 
#include "http_request.h"
#include "file_handler.h"
#include "cgi_handler.h"    
#include "need.h"
/// @brief 定义套接字描述符、调用startup函数初始化服务器socket并开始监听指定端口
///        进入无限循环，不断监听并使用调用accept_request函数处理客户端的连接请求，出错时关闭服务器。
/// @param
/// @return
int main(void)
{
  int server_sock = -1;    // 定义服务器socket描述符
  int client_sock = -1;    // 定义客户端socket描述符
  server_sock = startup(); // 服务器初始化
  

  // 准备accept()函数需要的参数，此函数用于从已完成连接队列的头部返回下一个已完成连接。
  // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
  // 参数包括套接字描述符、指向用于存储接受连接的客户端的地址信息的sockaddr结构体的指针，sockaddr结构体的长度
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  // 不断监听和处理请求，直到运行错误error_die退出
  while (1)
  {
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
    // 返回值为-1时表示套接字描述符获取失败，报错，退出循环并结束全部程序运行。
    if (client_sock == -1)
      error_die("main","Accept function failed to execute");

    // 处理请求
    accept_request(client_sock);
  }
  shutdown_loggin();
}