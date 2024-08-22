#include "http_request.h"



/// @brief 函数能够处理以下几种 HTTP 请求报文：
///        GET 请求：用于获取静态资源或执行带查询参数的 CGI 程序。
///        POST 请求：用于提交表单数据，并执行相应的 CGI 程序。
///        请求目录：默认返回目录下的 start.html 文件。
///        不支持的方法：返回 501 Not Implemented 响应。
/* http请求报文示例
请求行
GET /index.html HTTP/1.1        // GET 请求（静态文件）
GET /search?q=example HTTP/1.1  // GET 请求（带查询参数，需执行CGI）
GET / HTTP/1.1                  // GET 请求（请求目录）
POST /submit HTTP/1.1           // POST 请求（需执行CGI），请求头需包含Content-Length:
*/
/// @param client_sock
void accept_request(int client)
{
  char buf[1024];  // 存储读取的数据
  int charsCount;  // 读取数据的字节数
  int cgiFlag = 0; // 是否需要执行cgi
  char url[255];   // 存储请求行可能包含的url
  char path[512];  // 存储文件路径，url+可能添加的后缀

  // 1.读取请求行
  charsCount = get_line(client, buf, sizeof(buf));
  if (charsCount <= 0)
  {
   //// fprintf(stderr, "Error reading from client socket\n");
    return;
  }

  // 2.解析请求方法
  int i = 0;
  int j = 0;
  char method[255]; // 存储请求方法

  while (!ISspace(buf[j]) && (i < sizeof(method) - 1))
  {
    method[i] = buf[j];
    i++;
    j++;
  }
  method[i] = '\0'; // 结束请求方法字符串

  // 不支持的请求方法，直接发送 response 告诉客户端没实现该方法
  if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
  {

    unimplemented(client);
    return;
  }

  // 如果是 POST 方法就将 cgi 标志变量置一(true)
  if (strcasecmp(method, "POST") == 0)
  {
    ////fprintf(stderr, "post\n");
    cgiFlag = 1;
  }

  // 3.解析 URL
  i = 0;

  // 跳过所有的空白字符(空格)
  while (ISspace(buf[j]) && (j < sizeof(buf)))
  {
    j++;
  }
  // 读取url

  while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf)))
  {
    url[i] = buf[j];
    i++;
    j++;
  }
  url[i] = '\0';

  // 4.处理 GET 请求中的查询字符串
  char *query_string = NULL;
  if (strcasecmp(method, "GET") == 0)
  {
    // 用一个指针指向 url
    query_string = url;
    // 去遍历这个 url，查找字符 ? ，其后跟着查询参数
    // 如果遍历完毕也没找到字符 ？则退出循环
    while ((*query_string != '?') && (*query_string != '\0'))
    {
      query_string++;
    }
    // url中带有?则有查询参数，需要执行cgi
    if (*query_string == '?')
    {
      cgiFlag = 1;
    ////  fprintf(stderr, "in query\n");
      // 从字符 ？ 处把字符串 url 给分隔会两份
      *query_string = '\0';
      // 使指针指向字符 ？后面的那个字符，即需要执行的文件
      query_string++;
    }
  }

  // 拼接路径，所有访问的资源均在sources目录下
  snprintf(path, sizeof(path), "sources%s", url);
  // 如果 path 数组中字符串的最后一个字符是以字符 / 结尾的话
  // 如GET / HTTP/1.1     // GET 请求（请求目录）
  // 就拼接上一个"start.html"的字符串，进入首页
  if (path[strlen(path) - 1] == '/')
  {
    ////fprintf(stderr, "strcat\n");
    strcat(path, "start.html");
  }
 
  //fprintf(stderr, "%s\n",path);
  // 检查文件状态
  struct stat st;
  if (stat(path, &st) == -1)
  {
    // 文件不存在，则将此次 http 的请求后续的内容(head 和 body)全部读完并忽略
    while ((charsCount > 0) && strcmp("\n", buf))
    {
      charsCount = get_line(client, buf, sizeof(buf));
    }
    fprintf(stderr, "%s\n",path);
    // 返回找不到文件的respose
    not_found(client);
  }
  else
  {
    // 文件存在
    fprintf(stderr, "%s\n",path);
    // 如果是一个目录，那就需要再在 path 后面拼接一个"/start.html"的字符串
    if ((st.st_mode & S_IFMT) == S_IFDIR)
    {
      strcat(path, "/start.html");
    }
    // 如果文件的权限是可执行的（是一个可执行文件，无论用户/组/其他可执行）
    // 表示需要执行cgi
    if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
    {
      ////fprintf(stderr, "st_mode\n");
      cgiFlag = 1;
    }

    if (!cgiFlag)
    {
      // 不需要cgi机制，只返回静态文件
      serve_file(client, path);
    }
    else
    {
      // 需要执行cgi，调用
      execute_cgi(client, path, method, query_string);
    }
  }

  close(client);
}

/// @brief 在HTTP响应中发送 HTTP的头部信息，包括状态行、响应头和空行
/// @param client 客户端套接字描述符
/// @param filename 打开的文件名称 用于确定header中的MIME类型，tinyhttpd没有使用
void headers(int client, const char *filename)
{
  char buf[1024];

  // 发送HTTP状态行
  strcpy(buf, "HTTP/1.0 200 OK\r\n");
  send(client, buf, strlen(buf), 0);
  // 发送服务器类型，SERVER_STRING是自定义的宏定义
  strcpy(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  // 确定并发送内容类别
  const char *mime_type = get_mime_type(filename);
  sprintf(buf, "Content-Type: %s\r\n", mime_type);
  send(client, buf, strlen(buf), 0);
  // 发送空行
  strcpy(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
}

/// @brief 发送400状态码，客户端请求的语法错误,没有Content-Length，服务器无法理解
/// @param client 客户端套接字描述符
void bad_request(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "Content-type: text/html\r\n");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, sizeof(buf), 0);

  // http报文响应体
  sprintf(buf, "<P>Your browser sent a bad request, ");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "such as a POST without a Content-Length.\r\n");
  send(client, buf, sizeof(buf), 0);
}
/// @brief 发送404状态码，请求的文件不存在或无法访问
void not_found(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-type: text/html\r\n");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, sizeof(buf), 0);

  // http报文响应体
  sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "your request because the resource specified\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "is unavailable or nonexistent.\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</BODY></HTML>\r\n");
  send(client, buf, strlen(buf), 0);
}
/// @brief 发500状态码，请求的文件不存在或无法访问
void cannot_execute(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
  send(client, buf, strlen(buf), 0);
}
/// @brief 发501状态码，服务器不支持的请求方法，无法完成请求
void unimplemented(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-Type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);

  sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</TITLE></HEAD>\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</BODY></HTML>\r\n");
  send(client, buf, strlen(buf), 0);
}
