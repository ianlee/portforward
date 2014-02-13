CC = g++
CFLAGS = -Wall -g
LDFLAGS = -lpthread

all: myprogram
client: main_client
echo_client.o : echo_client.cpp echo_client.h
	${CC} ${CFLAGS} -c echo_client.cpp
main_client: echo_client.o main_client.cpp
	${CC} ${CFLAGS} main_client.cpp echo_client.o ${LDFLAGS} -o ../client

multi_thread_server.o : multi_thread_server.cpp multi_thread_server.h
	${CC} ${CFLAGS} -c multi_thread_server.cpp ${LDFLAGS}

main_server.o : main_server.cpp multi_thread_server.h
	${CC} ${CFLAGS} -c main_server.cpp 



myprogram : main_server.o multi_thread_server.o 
	${CC} ${CFLAGS} main_server.o multi_thread_server.o ${LDFLAGS} -o ../server

clean:
	rm -rf *.o a *.cpp~ *.h~
