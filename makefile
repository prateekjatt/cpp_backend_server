CXX := g++ -std=c++17
INC_FLAGS := -Idependencies/boost-1.88.0_build/include
LD_FLAGS := -Ldependencies/boost-1.88.0_build/lib -lpthread
OBJ_FILES := ./build/main.o ./build/server.o ./build/http_session.o ./build/root_handler.o


./build/main: $(OBJ_FILES)
	@echo "\n---Generating Executable---"
	$(CXX) $(OBJ_FILES) -o ./build/main $(INC_FLAGS) $(LD_FLAGS)
	rm $(OBJ_FILES)

./build/main.o: main.cpp ./build
	$(CXX) -c main.cpp -o ./build/main.o $(INC_FLAGS)

./build/server.o: ./server/server.cpp ./build
	$(CXX) -c ./server/server.cpp -o build/server.o $(INC_FLAGS)

./build/http_session.o: ./server/http_session.cpp ./build
	$(CXX) -c ./server/http_session.cpp -o build/http_session.o $(INC_FLAGS)

./build/root_handler.o: ./handlers/root_handler.cpp ./build
	$(CXX) -c ./handlers/root_handler.cpp -o build/root_handler.o $(INC_FLAGS)

./build:
	mkdir build

.PHONY: clean
clean:
	rm -rf build