serverStart:timer.o connectDataBase.o server.o util.o udp.o lock.o condition.o threadpool.o data.o processEvent.o
	g++ -g timer.o connectDataBase.o server.o util.o udp.o lock.o condition.o threadpool.o data.o processEvent.o `mysql_config --cflags --libs` -o serverStart
timer.o:timer.cpp
	g++ -g -c timer.cpp -o timer.o
processEvent.o:processEvent.cpp
	g++ -g -c processEvent.cpp -o processEvent.o
data.o:data.cpp
	g++ -g -c data.cpp -o data.o
lock.o:lock.cpp
	g++ -g -c lock.cpp -o lock.o
condition.o:condition.cpp
	g++ -g -c condition.cpp -o condition.o
connectDataBase.o:connectDataBase.cpp
	g++ -g -c connectDataBase.cpp `mysql_config --cflags --libs` -o connectDataBase.o
threadpool.o:threadpool.cpp
	g++ -g -c threadpool.cpp `mysql_config --cflags --libs` -o threadpool.o
udp.o:udp.cpp
	g++ -g -c udp.cpp -o udp.o
util.o:util.cpp
	g++ -g -c util.cpp -o util.o
server.o:epollserver.cpp
	g++ -g -c epollserver.cpp `mysql_config --cflags --libs` -o server.o
clean:
	rm -rf *.o serverStart
