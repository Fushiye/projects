# httpServer

httpServer是一个简单的 HTTP 服务器实现，支持基本的静态文件服务和 CGI 脚本执行。

## 目录结构说明

- **`src/`**: 存放所有源代码文件。
  - **`main.c`**: 服务器的主程序，启动服务器，协调各个模块的调用。
  - **`network.c`**: 网络处理模块，负责创建、绑定、监听和接受客户端连接。
  - **`http_request.c`**: HTTP请求处理模块，解析请求，处理GET和POST请求。
  - **`file_handler.c`**: 文件处理模块，处理静态文件请求，读取文件，检查文件类型。
  - **`cgi_handler.c`**: CGI处理模块，执行CGI脚本，处理动态内容。
  - **`logger.c`**: 日志模块，记录访问日志、错误日志和调试信息。

- **`include/`**: 存放所有头文件。
  - **`network.h`**: 网络处理模块的接口定义。
  - **`http_request.h`**: HTTP请求处理模块的接口定义。
  - **`file_handler.h`**: 文件处理模块的接口定义。
  - **`cgi_handler.h`**: CGI处理模块的接口定义。
  - **`logger.h`**: 日志模块的接口定义。

- **`Makefile`**: 用于编译项目的Makefile，定义如何编译和链接项目的各个部分。
- **`README.md`**: 项目的说明文档，包括项目概述、编译和运行指南等。

## 编译和运行

1. 进入项目目录。
2. 执行 `make` 编译项目。
3. 使用 `./tinyhttpd` 启动服务器。

## 模块说明

- `network.c` - 处理网络相关操作。
- `http_request.c` - 处理 HTTP 请求。
- `file_handler.c` - 处理静态文件请求。
- `cgi_handler.c` - 处理 CGI 脚本。
- `logger.c` - 记录日志信息。
