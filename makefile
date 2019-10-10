target: server client peer

serversocekt.o: serversocket.cpp serversocket.h
	g++ -c serversocket.cpp serversocket.h -lws2_32

server: serversocket.o server.cpp
	g++ -o server serversocket.o server.cpp -lws2_32

client: client.cpp
	g++ -o client client.cpp -lws2_32

peer: peer.cpp
	g++ -o peer peer.cpp serversocket.o -lws2_32