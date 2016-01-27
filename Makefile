CXXFLAGS	= -g

.PHONY: all

all: vm

vm: vm.cpp vm.h instruction.h
	$(CXX) vm.cpp -o vm -Wall $(CXXFLAGS)
