CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
INCLUDES = -I/usr/include -I/usr/local/include -I./src/core -I./src/scenes
LDFLAGS = -L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system

# Ensure directory exists
ensure_dir = @mkdir -p $(@D)


test/engineTest: ./test/engineTest.cpp ./src/core/GameEngine.cpp ./src/core/GameEngine.h ./src/scenes/Scene.h ./src/core/Actions.h
	$(ensure_dir)
	$(CXX) $(CXXFLAGS) $(INCLUDES) ./test/engineTest.cpp ./src/core/GameEngine.cpp $(LDFLAGS) -o $@
