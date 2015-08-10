all:
	gcc server1.c -o server1
	gcc server2.c -o server2

clean:
	rm -f server1 server2