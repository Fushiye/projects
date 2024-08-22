# 默认目标
all: setup httpd sources/load_question.cgi sources/update_question.cgi

# 运行 setup.sh 脚本
setup:
	./setup.sh
# 编译 httpd
httpd: src/main.o src/network.o src/http_request.o src/file_handler.o src/cgi_handler.o
	gcc -o httpd src/main.o src/network.o src/http_request.o src/file_handler.o src/cgi_handler.o -lpthread

# 编译 load_question.cgi 并将其放在 cgi 目录中
sources/load_question.cgi: cgi/load_question.o
	gcc -o sources/load_question.cgi cgi/load_question.o -lmysqlclient

# 编译 update_question.cgi 并将其放在 cgi 目录中
sources/update_question.cgi: cgi/update_question.o
	gcc -o sources/update_question.cgi cgi/update_question.o -lmysqlclient

# 编译所有源文件
src/main.o: src/main.c include/network.h include/http_request.h include/file_handler.h include/cgi_handler.h
	gcc -Iinclude -c -o src/main.o src/main.c

src/network.o: src/network.c include/network.h
	gcc -Iinclude -c -o src/network.o src/network.c

src/http_request.o: src/http_request.c include/http_request.h include/file_handler.h include/cgi_handler.h
	gcc -Iinclude -c -o src/http_request.o src/http_request.c

src/file_handler.o: src/file_handler.c include/file_handler.h
	gcc -Iinclude -c -o src/file_handler.o src/file_handler.c

src/cgi_handler.o: src/cgi_handler.c include/cgi_handler.h
	gcc -Iinclude -c -o src/cgi_handler.o src/cgi_handler.c

cgi/load_question.o: cgi/load_question.c
	gcc -Iinclude -c -o cgi/load_question.o cgi/load_question.c -lmysqlclient

cgi/update_question.o: cgi/update_question.c
	gcc -Iinclude -c -o cgi/update_question.o cgi/update_question.c -lmysqlclient

# 清理目标
clean:
	rm -f httpd src/*.o cgi/*.o sources/load_question.cgi sources/update_question.cgi
