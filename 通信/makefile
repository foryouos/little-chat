# 获取当前搜索下的源文件
src = $(wildcard *.c)
# 将源文件的后缀替换为.o
obj = $(patsubst %.c,%.o,$(src))
calc:server.o client.o
	gcc server.o -pthread -o server
	gcc client.o -pthread -o client

server.o:server.c
	gcc -c server.c
client.o:client.c
	gcc -c client.c
# 生命clean
.PHONY:clean
clean:
	#shell命令前-表示强制执行
	- rm $(obj) $(calc)

