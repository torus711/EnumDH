CXX	:= g++
CXXFLAGS := -std=c++17 -Wall -Wextra

SRCS := main.cpp dh.cpp ./../common/abstruct_enumerator.cpp
OBJECTS := $(dir SRCS)/$(SRCS:%.cpp=%.o)
DEPS := $(dir OBJECTS)/$(OBJECTS:%.o=%.d)

main.exe: $(OBJECTS)
	$(CXX) -o $@ $^

-include $(DEPS)

%.o: %.cpp
	$(CXX) -c -MMD -MP $(CXXFLAGS) $< -o $(dir <)$(<:%.cpp=%.o)

clean:
	rm $(OBJECTS)
	rm $(DEPS)
