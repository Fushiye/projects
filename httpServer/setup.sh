#!/bin/bash

# 更新系统包索引
# sudo apt update

# 安装编译器和库
# sudo apt install -y build-essential libmysqlclient-dev liblog4c-dev


# 授予 sources 文件夹下所有 HTML 文件 666 权限
find sources -type f -name '*.html' -exec chmod 666 {} \;

# 授予所有 CGI 文件可执行权限
find htdocs -type f -name '*.cgi' -exec chmod +x {} \;

echo "Setup complete."
