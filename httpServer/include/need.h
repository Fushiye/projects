#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include "logger.h"

#define SERVER_STRING "Server: httpServer/0.1.0\r\n" // 定义服务器类别
#define ISspace(x) isspace((int)(x))                // 判断x是否是空格，帮助解析

     
