CXX=g++
CFLAGS=-Wall -std=c++98
LDFLAGS=-lpthread
INC=-Ideps/eigen

all: conjugate_gradient_serial conjugate_gradient_parallel

conjugate_gradient_serial: conjugate_gradient_serial.cpp
	$(CXX) $(CFLAGS) -o $@ $(LDFLAGS) $^ $(INC)

conjugate_gradient_parallel: conjugate_gradient_parallel.cpp
	$(CXX) $(CFLAGS) -o $@ $(LDFLAGS) $^ $(INC)
