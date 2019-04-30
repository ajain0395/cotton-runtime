BIN =../bin
SRC =../src
OBJ =../obj
MAKE =../make
INC =../inc

CC = g++ 
CFLAGS =  -std=c++11
IFLAGS = -I$(INC)

all : $(BIN)/NQueens.e $(BIN)/Matrix.e
  
matrix : $(BIN)/Matrix.e

nqueens : $(BIN)/NQueens.e

$(BIN)/NQueens.e : $(SRC)/nqueens.cpp $(SRC)/cotton-runtime.cpp
	$(CC) -g $(IFLAGS) $(SRC)/cotton-runtime.cpp $(SRC)/nqueens.cpp -o $(BIN)/NQueens.e -lpthread
	export COTTON_WORKERS=1

$(BIN)/Matrix.e : $(SRC)/matrix.cpp $(SRC)/cotton-runtime.cpp
	$(CC) -g $(IFLAGS)/ $(SRC)/cotton-runtime.cpp -lpthread $(SRC)/matrix.cpp -lOpenCL $(CFLAGS) -o $(BIN)/Matrix.e
	export COTTON_WORKERS=1
	
$(OBJ)/matrix.o : $(SRC)/matrix.cpp
	$(CC) $(CFLAGS) $(IFLAGS) $(SRC)/matrix.cpp -lOpenCL
	mv *.o $(OBJ)

$(OBJ)/cotton-runtime.o : $(SRC)/cotton-runtime.cpp
	$(CC) -lOpenCL  $(CFLAGS) $(IFLAGS) $(SRC)/cotton-runtime.cpp
	mv *.o $(OBJ)
	
$(OBJ)/nqueens.o : $(SRC)/nqueens.cpp 
	$(CC) $(CFLAGS) $(IFLAGS) $(SRC)/nqueens.cpp
	mv *.o $(OBJ)

clean : 
	rm -rf $(OBJ)/*.o $(BIN)/*.e
