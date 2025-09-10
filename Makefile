all:
	mkdir -p build
	gcc chat_client.c -o ./build/client
	gcc chat_server.c -o ./build/server
