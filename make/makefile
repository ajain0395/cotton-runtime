BIN =../bin
SRC =../src
OBJ =../obj
MAKE =../make
INC =../inc

CC = g++
CFLAGS = -Wall -g -c 
IFLAGS = -I$(INC)

all : $(BIN)/NQueens.e
  
$(BIN)/NQueens.e : $(OBJ)/nqueens.o $(OBJ)/cotton-runtime.o
	$(CC) $(OBJ)/cotton-runtime.o $(OBJ)/nqueens.o -o $(BIN)/NQueens.e -lpthread
	export COTTON_WORKERS=1
	
$(OBJ)/cotton-runtime.o : $(SRC)/cotton-runtime.cpp
	$(CC) $(CFLAGS) $(IFLAGS) $(SRC)/cotton-runtime.cpp
	mv *.o $(OBJ)
	
$(OBJ)/nqueens.o : $(SRC)/nqueens.cpp 
	$(CC) $(CFLAGS) $(IFLAGS) $(SRC)/nqueens.cpp
	mv *.o $(OBJ)

clean : 
	rm -rf $(OBJ)/*.o $(BIN)/*.e
