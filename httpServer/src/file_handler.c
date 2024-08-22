#include "file_handler.h"


/// @brief 处理客户端请求并将指定文件的内容发送到客户端。
/// @param client 客户端的 socket 描述符，用于通信。
/// @param filename 要发送给客户端的文件的路径。
void serve_file(int client, const char *filename)
{
  FILE *resource = NULL;
  int charsCount = 1;
  char buf[1024];

  // 读取并忽略掉这个 http 请求后面的所有内容
  do
  {
    charsCount = get_line(client, buf, sizeof(buf));
  } while ((charsCount > 0) && strcmp("\n", buf));

  // 尝试打开文件
  resource = fopen(filename, "r");
  if (resource == NULL)
  {
    not_found(client);
  }
  else
  {
    // 发送响应头部
    headers(client, filename);
    // 发送文件内容

    cat(client, resource);
  }
  // 关闭文件指针，避免资源泄漏
  fclose(resource);
}

/// @brief 读取文件内容并发送给客户端
/// @param client 客户端socket描述符
/// @param resource 文件指针，指向需要读取的文件
void cat(int client, FILE *resource)
{
  char buf[4096];                                   // 使用更大的缓冲区，减少系统调用此时以提高性能
  while (fgets(buf, sizeof(buf), resource) != NULL) // 读取文件内容
  {
    size_t len = strlen(buf);
    if (send(client, buf, len, 0) == -1) // 发送数据并检查错误
    {
      perror("Error sending data to client");
      break; // 发送失败则退出循环
    }
  }

  if (ferror(resource)) // 检查文件读取错误
  {
    perror("Error reading from file");
  }
}

/// @brief 用于从 socket 读取一行数据，用于解析 HTTP 请求的头部。
////       函数通过逐字节读取数据，直到遇到换行符或达到指定的缓冲区大小。
/// @param sock 套接字描述符
/// @param buf 缓冲区
/// @param size 缓冲区大小
/// @return 读取数据的大小
int get_line(int sock, char *buf, int size)
{
  int i = 0;
  char c = '\0';
  int n;

  // 确保but不为NULL且size为正
  if (buf == NULL || size <= 0)
  {
    return -1; // 无效参数
  }

  while ((i < size - 1) && (c != '\n'))
  {
    // 从套接字读取一个字节
    n = recv(sock, &c, 1, 0);
    if (n > 0)
    {
      // 检查是否有 CRLF(回车换行符)
      // 待补充CRLF注入漏洞防御
      if (c == '\r')
      {
        // 查看下一个字符是否为 '\n'
        n = recv(sock, &c, 1, MSG_PEEK);
        if ((n > 0) && (c == '\n'))
        {
          // 使用 '\n'
          recv(sock, &c, 1, 0);
        }
        else
        {
          c = '\n'; // 如果没有 '\n'，则将 '\r' 视为 '\n'
        }
      }
      buf[i] = c;
      i++;
    }
    else if (n == 0)
    {
      // 连接被客户端关闭
      break;
    }
    else
    {
      // 错误处理
      perror("recv");
      return -1; // 出错时返回-1
    }
  }
  buf[i] = '\0'; // 以空值终止字符串

  return i;
}

/// @brief 通过文件名确定文件的MIME类型以填充header
/// @param filename
/// @return 字符串，文件MIME类型
const char *get_mime_type(const char *filename)
{
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return "application/octet-stream"; // 默认MIME类型
  if (strcmp(dot, ".html") == 0 || strcmp(dot, ".html") == 0)
    return "text/html";
  if (strcmp(dot, ".css") == 0)
    return "text/css";
  if (strcmp(dot, ".js") == 0)
    return "application/javascript";
  if (strcmp(dot, ".png") == 0)
    return "image/png";
  if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
    return "image/jpeg";
  if (strcmp(dot, ".gif") == 0)
    return "image/gif";
  if (strcmp(dot, ".pdf") == 0)
    return "application/pdf";
  if (strcmp(dot, ".txt") == 0)
    return "text/plain";

  return "application/octet-stream"; // 不确定文件类型时的默认类型
}
