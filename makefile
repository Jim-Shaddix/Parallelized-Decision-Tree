
# <-- VARIABLES -->
#------------------------------------------------------------------------------

# Compiler
CC = g++

# FLAGS
FLAGS = -O3
OMPFLAGS = -fopenmp
MATHFLAG = -lm

# global directory refference
G_DIR = global_files/

# FILES
OBJS   = timer.o useful-func.o read_csv.o #Decision-Node.o
H_FILE = $(G_DIR)timer.h $(G_DIR)useful-func.h $(G_DIR)read_csv.h DecisionTree1.h 

# EXECUTABLES
EXEC = decTree_SEQ decTree_SEQ_B decTree_PAR

all: $(EXEC)

# <-- COMPILATION -->
#------------------------------------------------------------------------------

# <-- GLOBAL FILES -->
#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
timer.o: $(G_DIR)timer.cpp $(G_DIR)timer.h
	$(CC) -o $@ -c $(G_DIR)timer.cpp $(FLAGS)

useful-func.o: $(G_DIR)useful-func.cpp $(G_DIR)useful-func.h
	$(CC) -o $@ -c $(G_DIR)useful-func.cpp $(FLAGS)

read_csv.o: $(G_DIR)read_csv.cpp $(G_DIR)read_csv.h
	$(CC) -o $@ -c $(G_DIR)read_csv.cpp $(FLAGS) 

#Decision-Node.o: $(G_DIR)Decision-Node.cpp $(G_DIR)Decision-Node.h
#	$(CC) -o $@ -c $(G_DIR)Decision-Node.cpp $(FLAGS) 
#
#Decision-Node.o: $(G_DIR)Decision-Node.cpp $(G_DIR)Decision-Node.h
#	$(CC) -o $@ -c $(G_DIR)Decision-Node.cpp $(FLAGS) 
#
#Decision-Node.o: $(G_DIR)Decision-Node.cpp $(G_DIR)Decision-Node.h
#	$(CC) -o $@ -c $(G_DIR)Decision-Node.cpp $(FLAGS) 
#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

# Original Version
decTree_SEQ: DecisionTree1.cpp $(H_FILE) $(OBJS) 
	$(CC) -o $@ DecisionTree1.cpp $(OBJS) $(FLAGS) $(MATHFLAG) 

decTree_PAR: DecisionTree1p.cpp $(H_FILE) $(OBJS) 
	$(CC) -o $@ DecisionTree1p.cpp $(OBJS) $(FLAGS) $(MATHFLAG) $(OMPFLAGS)

# Better version
decTree_SEQ_B: DecisionTree2.cpp $(H_FILE) $(OBJS) 
	$(CC) -o $@ DecisionTree2.cpp $(OBJS) $(FLAGS) $(MATHFLAG) 
#------------------------------------------------------------------------------


# <-- EXTRA -->
#------------------------------------------------------------------------------
clean:
	rm -f $(EXEC) *.o

tar:
	tar cvf Shaddix-Hussain-project.tar *
#------------------------------------------------------------------------------
