CXX := g++ -std=c++20 -fcoroutines
INC_FLAGS := -Idependencies/boost-1.88.0_build/include -Idependencies/libsodium-stable/src/libsodium/include
LD_FLAGS := -Ldependencies/boost-1.88.0_build/lib -Ldependencies/libsodium-stable/src/libsodium/.libs -lpthread -lssl -lcrypto -Wl,-Bstatic -lboost_json -lboost_charconv -lboost_url -lquadmath -lsodium -Wl,-Bdynamic
SRC_FILES := $(shell find ./handlers ./server ./db_client ./utils ./models main.cpp -name '*.cpp')
OBJ_FILES := $(SRC_FILES:%.cpp=./build/%.o)

./build/backend_server: $(OBJ_FILES)
	@echo "\n---Generating Executable---"
	$(CXX) $(OBJ_FILES) -o ./build/backend_server $(INC_FLAGS) $(LD_FLAGS)

./build/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(INC_FLAGS)

.PHONY: clean
clean:
	rm -rf build