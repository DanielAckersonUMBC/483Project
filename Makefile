CXX=g++
CFLAGS=-Wall -g -std=c++98 -O2
LDFLAGS=-pthread -lrt

all: jacobi_serial jacobi_parallel matrix_writer matrix_reader

jacobi_serial: jacobi_test.o jacobi.o
	$(CXX) -o $@ $(LDFLAGS) $^

jacobi_parallel: jacobi_test_parallel.o ParallelJacobiSolver.o jacobi.o
	$(CXX) -o $@ $(LDFLAGS) $^

matrix_writer: MatrixWriter.o SparseMatrixGen.o mmio.o
	$(CXX) -o $@ $(LDFLAGS) $^

matrix_reader: MatrixReaderTest.o MatrixReader.o mmio.o ParallelJacobiSolver.o jacobi.o
	$(CXX) -o $@ $(LDFLAGS) $^

%.o: %.cpp
	$(CXX) -c -o $@ $(CFLAGS) $<
