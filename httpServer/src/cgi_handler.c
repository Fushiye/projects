#include "cgi_handler.h"

/// @brief
/// @param client
/// @param path
/// @param method
/// @param query_string
void execute_cgi(int client, const char *path, const char *method, const char *query_string)
{
  char buf[1024];
  int cgi_output[2];
  int cgi_input[2];
  pid_t pid;
  int status;
  int charsCount = 1;
  int content_length = -1;

  // 用于确保能够进入循环
  buf[0] = 'A';
  buf[1] = '\0';

  // 检查请求方法
  if (strcasecmp(method, "GET") == 0)
  {
    // 忽略 GET 请求的剩余部分
    while ((charsCount > 0) && strcmp("\n", buf))
    {
      charsCount = get_line(client, buf, sizeof(buf));
    }
  }
  else
  {
    // 处理 POST 请求，只有 POST 方法才继续读内容
    charsCount = get_line(client, buf, sizeof(buf));
    // 读出指示 body 长度大小的参数，并记录 body 的长度大小。其余的 header 里面的参数一律忽略
    // 注意这里只读完 header 的内容，body 的内容没有读
    while ((charsCount > 0) && strcmp("\n", buf))
    {
      buf[15] = '\0';
      if (strcasecmp(buf, "Content-Length:") == 0)
      {
        // 记录 body 的长度大小
        content_length = atoi(&(buf[16]));
      }
      charsCount = get_line(client, buf, sizeof(buf));
    }
    // 如果没有 Content-Length 字段，返回错误
    if (content_length == -1)
    {
      bad_request(client);
      return;
    }
  }

  // 返回 HTTP 响应头
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  send(client, buf, strlen(buf), 0);

  // 创建管道 用于两个进程间通信
  if (pipe(cgi_output) < 0)
  {
    cannot_execute(client);
    return;
  }
  if (pipe(cgi_input) < 0)
  {
    cannot_execute(client);
    return;
  }

  // 创建子进程
  if ((pid = fork()) < 0)
  {
    cannot_execute(client);
    return;
  }

  if (pid == 0)
  { /* 子进程 执行 cgi 脚本*/
    char meth_env[255];
    char query_env[255];
    char length_env[255];

    // 重定向输入输出
    // 将子进程的输出由标准输出重定向到 cgi_ouput 的管道写端上
    // 将子进程的输出由标准输入重定向到 cgi_ouput 的管道读端上
    dup2(cgi_output[1], STDOUT_FILENO);
    dup2(cgi_input[0], STDIN_FILENO);

    // 关闭未使用的管道端
    close(cgi_output[0]);
    close(cgi_input[1]);

    // 设置环境变量
    setenv("REQUEST_METHOD", method, 1);

    // 根据http请求的不同方法，设置不同的环境变量
    if (strcasecmp(method, "GET") == 0)
    {
      setenv("QUERY_STRING", query_string, 1);
    }
    else
    { /* POST */
      sprintf(length_env, "%d", content_length);
      setenv("CONTENT_LENGTH", length_env, 1);
    }

    // 执行 CGI 脚本
    execl(path, path, NULL);
    exit(0); // 如果 exec 失败，确保子进程终止
  }
  else
  { /* 父进程 */
    // 关闭未使用的管道端
    close(cgi_output[1]);
    close(cgi_input[0]);

    // 如果是 POST 方法，写入请求体到子进程的输入
    if (strcasecmp(method, "POST") == 0)
    {
      for (int i = 0; i < content_length; i++)
      {
        char c;
        recv(client, &c, 1, 0);
        write(cgi_input[1], &c, 1);
      }
    }

    // 从子进程读取输出并发送给客户端
    char buffer[4096];
    int bytesRead;
    while ((bytesRead = read(cgi_output[0], buffer, sizeof(buffer))) > 0)
    {
      send(client, buffer, bytesRead, 0);
    }

    // 关闭管道并等待子进程结束
    close(cgi_output[0]);
    close(cgi_input[1]);
    waitpid(pid, &status, 0);
  }
}