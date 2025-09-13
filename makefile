CXX := g++ -std=c++17
INC_FLAGS := -Idependencies/boost-1.88.0_build/include
LD_FLAGS := -Ldependencies/boost-1.88.0_build/lib -lpthread
SRC_FILES := $(shell find ./handlers ./server main.cpp -name '*.cpp')
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