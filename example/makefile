all : registry discovery
registry : registry.cc main.pb.cc
	g++ -std=c++17 -o $@ $^ -lspdlog -lfmt -lgflags -letcd-cpp-api -lcpprest -lbrpc -lssl -lcrypto -lprotobuf -lleveldb
discovery : discovery.cc main.pb.cc
	g++ -std=c++17 -o $@ $^ -lspdlog -lfmt -lgflags -letcd-cpp-api -lcpprest -lbrpc -lssl -lcrypto -lprotobuf -lleveldb

.PHONY:clean
clean:
	rm -f discovery registry