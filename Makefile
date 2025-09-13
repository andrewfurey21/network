all:
	mkdir -p build
	gcc udp_server.c -o ./build/server
	gcc udp_client.c -o ./build/client
