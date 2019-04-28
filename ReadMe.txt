Assignment 2 
Ashish Jain & Sarosh Hasan
(MT18052 & MT18084)


Step to execute the program 1 & 2 -


1. Go to "make" directory and use below cmd 
                --> make clean (cmd to delete the object files and create bin and obj directory)
                --> make all (to make executable of nqueen in “bin” directory)


2. Go to "bin" directory and use below cmd  to execute the nqueen -
        
---> export COTTON_WORKERS=1         (one time to set environment variable)
        
--->  COTTON_WORKERS=4; ./NQueens 12 (To execute the code)
        
3. In the "src" directory files present are -


---> cotton-runtime.cpp
---> nqueens.cpp 


4. In the "inc" directory files present are -


---> cotton.h
---> cotton-runtime.h