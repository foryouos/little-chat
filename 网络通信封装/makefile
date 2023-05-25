# 获取当前搜索下的源文件
src = $(wildcard *.cpp)
# 将源文件的后缀替换为.o
obj = $(patsubst %.cpp,%.o,$(src))
calc:TcpSocket.o TcpServer.o server.o client.o
	g++ TcpSocket.o TcpServer.o server.o -pthread -o server
	g++ TcpSocket.o client.o -pthread -o client
TcpSocket.o:TcpSocket.cpp
	g++ -c TcpSocket.cpp
TcpServer.o:TcpServer.cpp
	g++ -c TcpServer.cpp
server.o:server.cpp
	g++ -c server.cpp
client.o:client.cpp
	g++ -c client.cpp
# 生命clean
.PHONY:clean
clean:
	#shell命令前-表示强制执行
	- rm $(obj) $(calc)

