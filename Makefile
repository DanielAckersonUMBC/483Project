CXX=g++
CFLAGS=-Wall -std=c++98
LDFLAGS=-lpthread

all: jacobi_serial jacobi_parallel

jacobi_serial: jacobi.o jacobi_test.o
	$(CXX) -o $@ $(LDFLAGS) $^

jacobi_parallel: ParallelJacobiSolver.o jacobi.o jacobi_test_parallel.o
	$(CXX) -o $@ $(LDFLAGS) $^

FileReaderTest: FileReaderTest.o FileReader.o
	$(CXX) -o $@ $(LDFLAGS) $^

%.o: %.c
	$(CXX) -c -o $@ $(CFLAGS) $<
