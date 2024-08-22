
1. 网络模块 (`network.c`)
   + 职责: 处理所有与网络相关的操作，包括服务器 socket 的创建、绑定端口、监听连接、接受客户端连接等。
   + 核心功能:
     - 创建服务器 socket (`create_server_socket`)
     - 监听和接受客户端连接 (`accept_client_connection`)
2. HTTP 请求处理模块 (`http_request.c`)
   + 职责: 解析 HTTP 请求，处理请求方法（GET、POST 等），根据请求生成适当的响应。
   + 核心功能:
     - 解析 HTTP 请求 (`parse_http_request`)
     - 处理 GET 请求 (`handle_get_request`)
     - 处理 POST 请求 (`handle_post_request`)
3. 文件处理模块 (`file_handler.c`)
   + 职责: 处理静态文件请求，负责读取文件、检查文件类型、处理目录索引等。
   + 核心功能:
     - 提供静态文件 (`serve_file`)
     - 检查文件类型 (`get_file_type`)
4. CGI 处理模块 (`cgi_handler.c`)
   + 职责: 处理 CGI 脚本的执行和与脚本的通信，解析 CGI 请求，传递环境变量等。
   + 核心功能:
     - 执行 CGI 脚本 (`execute_cgi`)
     - 处理 CGI 响应 (`process_cgi_response`)
5. 日志模块 (`logger.c`)
   + 职责: 记录日志信息，如访问日志、错误日志、调试信息，方便对服务器进行监控和问题排查。
   + 核心功能:
     - 记录访问日志 (`log_access`)
     - 记录错误日志 (`log_error`)
6. 主程序（`main.c`）
   + 主程序负责启动服务器，调用各模块的函数，协调服务器的整体运行。