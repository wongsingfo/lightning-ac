CXXFLAGS	= -g

.PHONY: all

all: vm

vm: vm.cpp vm.h
	$(CXX) vm.cpp -o vm -Wall $(CXXFLAGS)
