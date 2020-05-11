CC = gcc
CFLAGS = -Wall -Werror -pedantic -lpthread -lm
OBJ = Main_A.o Lift_A.o LiftR_A.o Request_A.o
EXEC = lift_sim_A

# Condional compilation (for outputting debug print statements)
ifdef DEBUG
CFLAGS += -D DEBUG
CFLAGS += -g
DEBUG: clean $(EXEC)
endif

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(CFLAGS)

Main.o: Main_A.c Main_A.h LiftR_A.h Lift_A.h Request_A.h
	$(CC) -c Main_A.c $(CFLAGS)

Lift.o: Lift_A.c Lift_A.h Main_A.h Request_A.h
	$(CC) -c Lift_A.c $(CFLAGS)

LiftR.o: LiftR_A.c LiftR_A.h Main_A.h Request_A.h
	$(CC) -c LiftR_A.c $(CFLAGS)

Request.o: Request_A.c Request_A.h
	$(CC) -c Request_A.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)