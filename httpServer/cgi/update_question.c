#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

void print_error(MYSQL *conn) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    mysql_close(conn);
    exit(1);
}

int main(void) {
    MYSQL *conn;
    char post_data[1024] = {0};
    char id[10] = {0};
    char action[2] = {0};
    int len;

    printf("Content-Type: text/html\n\n");

    conn = mysql_init(NULL);
    if (conn == NULL) {
        printf("MySQL initialization failed");
        exit(1);
    }

    if (mysql_real_connect(conn, "localhost", "root", "123456", "Data", 0, NULL, 0) == NULL) {
        print_error(conn);
    }

    // 获取 POST 数据
    len = atoi(getenv("CONTENT_LENGTH"));
    fread(post_data, 1, len, stdin);

    // 解析 id 和 action
    sscanf(post_data, "id=%[^&]&action=%s", id, action);

    // 如果按下的是按键 A，则更新 skilled 字段
    if (strcmp(action, "A") == 0) {
        char query[256];
        sprintf(query, "UPDATE Questions SET skilled=1 WHERE id=%s", id);
        if (mysql_query(conn, query)) {
            print_error(conn);
        }
    }
    char query_select[256];
    sprintf(query_select,"SELECT id, question, answer FROM Questions WHERE skilled=0 and id != %s LIMIT 1", id);
    // 加载下一个 skilled 为 0 的问题
    if (mysql_query(conn, query_select)) {
        print_error(conn);
    }

    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (row == NULL) {
        printf("<html><body><h1>No more questions to review!</h1></body></html>");
    } else {
        printf("<html><head><meta charset=\"UTF-8\"><title>面试速刷题</title></head>");
        printf("<body style=\"align-items: center; justify-content: center;  background: #f4f4f4;\">");
        printf("<h1 style=\"text-align: center;  margin-bottom: 10px;\">面试速刷题</h1>");
        printf("<form action=\"/update_question.cgi\" method=\"POST\" style=\"background: #fff;padding: 10px;\">");
        printf("<p style=\"margin: 5px 0 5px 20px; color: rgb(42, 0, 251);\">问题：");
        printf("<span id=\"text1\" style=\"display: block; word-wrap: break-word; color: rgb(0, 0, 0);\">%s", row[1]);
        printf("</span></p>");
        printf("<p style=\"margin: 5px 0 5px 20px; color: rgb(42, 0, 251); \">答案：");
        printf("<span id=\"text2\" style=\" display: block; word-wrap: break-word;color: rgb(0, 0, 0);\">%s",row[2]);    
        printf("</span></p>");
        printf("<div style=\"border: none; padding: 5px 10px; margin: 5px; border-radius: 3px; cursor: pointer;\">");
        printf("<input type=\"hidden\" name=\"id\" id=\"question_id\" value=\"%s\">",row[0]);
        printf("<button type=\"submit\" name=\"action\" value=\"A\" style=\"background: #4CAF50; color: white; \">熟悉</button>");
        printf("<button type=\"submit\" name=\"action\" value=\"B\" style=\"background: #f44336; color: white; \">陌生</button>");
        printf("</div></form></body></html>");
    }

    mysql_free_result(res);
    mysql_close(conn);

    return 0;
}


