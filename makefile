CXX := g++ -std=c++20
INC_FLAGS := -Idependencies/boost-1.88.0_build/include
LD_FLAGS := -Ldependencies/boost-1.88.0_build/lib -lpthread -lssl -lcrypto -Wl,-Bstatic -lboost_json -lboost_charconv -lboost_url -lquadmath -Wl,-Bdynamic
SRC_FILES := $(shell find ./handlers ./server ./db_client ./utils main.cpp -name '*.cpp')
OBJ_FILES := $(SRC_FILES:%.cpp=./build/%.o)

./build/main: $(OBJ_FILES)
	@echo "\n---Generating Executable---"
	$(CXX) $(OBJ_FILES) -o ./build/main $(INC_FLAGS) $(LD_FLAGS)

./build/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(INC_FLAGS)

.PHONY: clean
clean:
	rm -rf build