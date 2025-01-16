all : registry discovery
registry : registry.cc
	g++ -std=c++17 -o $@ $^ -lspdlog -lfmt -lgflags -letcd-cpp-api -lcpprest
discovery : discovery.cc
	g++ -std=c++17 -o $@ $^ -lspdlog -lfmt -lgflags -letcd-cpp-api -lcpprest

.PHONY:clean
clean:
	rm -f discovery registry