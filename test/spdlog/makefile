all : sync async
sync : sync.cc
	g++ -std=c++17 -o $@ $^ -lspdlog -lfmt
async : async.cc
	g++ -std=c++17 -o $@ $^ -lspdlog -lfmt