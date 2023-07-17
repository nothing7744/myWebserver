all:mytcpepoll

mytcpepoll:mytcpepoll.cpp
	g++ -std=c++11 -g mytcpepoll.cpp -o mytcpepoll request.cpp threadpool.cpp util.cpp epoll.cpp -lpthread -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs
clean:
	rm -rf mytcpepoll
