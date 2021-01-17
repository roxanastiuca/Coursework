
build:
	gcc -Wall subscriber.c utils.c topic_list.c queue.c -lm -o subscriber
	gcc -Wall server.c utils.c topic_list.c queue.c -o server

run_server:
	./server

run_subscriber:
	./subscriber

clean:
	rm -f server subscriber
