all:client mytcpepoll

client:client.cpp
	g++ -std=c++11 -g client.cpp -o client
mytcpepoll:mytcpepoll.cpp
	g++ -std=c++11 -g mytcpepoll.cpp -o mytcpepoll request.cpp threadpool.cpp util.cpp epoll.cpp -lpthread
clean:
	rm -rf client mytcpepoll