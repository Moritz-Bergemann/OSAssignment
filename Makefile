CC = gcc
CFLAGS = -Wall -Werror -pedantic -lpthread -lm

OBJ_A = Main_A.o Lift_A.o LiftR_A.o Request_A.o
OBJ_B = Main_B.o Lift_B.o LiftR_B.o Request_B.o

EXEC_A = lift_sim_A
EXEC_B = lift_sim_B


# Condional compilation (for outputting debug print statements)
ifdef DEBUG
CFLAGS += -D DEBUG
CFLAGS += -g
DEBUG: clean $(EXEC_A) $(EXEC_B) #cleans and then performs target
endif

#COMPILE ALL
all: $(EXEC_A) $(EXEC_B)

# PART A REQUIREMENTS
$(EXEC_A): $(OBJ_A)
	$(CC) $(OBJ_A) -o $(EXEC_A) $(CFLAGS)
	
Main_A.o: Main_A.c Main_A.h LiftR_A.h Lift_A.h Request_A.h
	$(CC) -c Main_A.c $(CFLAGS)

Lift_A.o: Lift_A.c Lift_A.h Main_A.h Request_A.h
	$(CC) -c Lift_A.c $(CFLAGS)

LiftR_A.o: LiftR_A.c LiftR_A.h Main_A.h Request_A.h
	$(CC) -c LiftR_A.c $(CFLAGS)

Request_A.o: Request_A.c Request_A.h
	$(CC) -c Request_A.c $(CFLAGS)

# PART B REQUIREMENTS
$(EXEC_B): $(OBJ_B)
	$(CC) $(OBJ_B) -o $(EXEC_B) $(CFLAGS)

Main_B.o: Main_B.c Main_B.h LiftR_B.h Lift_B.h Request_B.h
	$(CC) -c Main_B.c $(CFLAGS)

Lift_B.o: Lift_B.h Lift_B.h Main_B.h Request_B.h
	$(CC) -c Lift_B.c $(CFLAGS)

LiftR_B.o: LiftR_B.c LiftR_B.h Main_B.h Request_B.h
	$(CC) -c LiftR_B.c $(CFLAGS)

Request_B.o: Request_B.c Request_B.h
	$(CC) -c Request_B.c $(CFLAGS)

clean:
	rm -f $(EXEC_A) $(OBJ_A) $(EXEC_B) $(OBJ_B) 