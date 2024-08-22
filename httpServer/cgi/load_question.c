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
    MYSQL_RES *res;
    MYSQL_ROW row;

    printf("Content-Type: text/html\n\n");

    conn = mysql_init(NULL);
    if (conn == NULL) {
        printf("MySQL initialization failed");
        exit(1);
    }

    if (mysql_real_connect(conn, "localhost", "root", "123456", "Data", 0, NULL, 0) == NULL) {
        print_error(conn);
    }

    if (mysql_query(conn, "SELECT id, question, answer FROM Questions WHERE skilled=0 LIMIT 1")) {
        print_error(conn);
    }

    res = mysql_store_result(conn);
    if (res == NULL) {
        print_error(conn);
    }

    row = mysql_fetch_row(res);
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
