CXX=g++
CFLAGS=-Wall -std=c++98 -O2
LDFLAGS=-lpthread -lrt

all: jacobi_serial jacobi_parallel

jacobi_serial: jacobi_test.o jacobi.o
	$(CXX) -o $@ $(LDFLAGS) $^

jacobi_parallel: jacobi_test_parallel.o ParallelJacobiSolver.o jacobi.o
	$(CXX) -o $@ $(LDFLAGS) $^

FileReaderTest: FileReaderTest.o FileReader.o
	$(CXX) -o $@ $(LDFLAGS) $^

%.o: %.cpp
	$(CXX) -c -o $@ $(CFLAGS) $<
