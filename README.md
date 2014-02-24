scalable-server
===============

Comp 8006 assignment 2.  

Instructions:

To compile the programs first navigate to the src folder
	cd src
Then run make
	make
navigate out one directory level and make a directory called test
	cd ..
	mkdir test
Now you can run the servers and clients

	./server [-t servertype] [-p port] [-f filename] [-n numberOfWorkers] [-b buflength]
	./client [-a hostname] [-p port] [-t timesToSend] [-c maxConnect] [-b buflength]
	
		server options
		-t -- server type	1 = multi-thread, 2=select, 3=epoll, default:epoll
		-p -- server port	default: 7000
		-f -- file output	default: test/tests.txt
		-n -- number of threads	default: 10
		-b -- buffer length	default: 255
		
		client options
		-a -- serverhostname
		-p -- server port	default: 7000
		-t -- timestosend str	default: 5000
		-c -- numberConnections	default: 1000
		-b -- buffer length	default: 255
