CC = gcc
CFLAGS = -Wall -Werror -pedantic -lpthread -lm
OBJ = Main_B.o Lift_B.o LiftR_B.o Request_B.o
EXEC = lift_sim_B

# Condional compilation (for outputting debug print statements)
ifdef DEBUG
CFLAGS += -D DEBUG
CFLAGS += -g
DEBUG: clean $(EXEC)
endif

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(CFLAGS)

Main.o: Main_B.c Main_B.h LiftR_B.h Lift_B.h Request_B.h
	$(CC) -c Main_B.c $(CFLAGS)

Lift.o: Lift_B.h Lift_B.h Main_B.h Request_B.h
	$(CC) -c Lift_B.h $(CFLAGS)

LiftR.o: LiftR_B.c LiftR_B.h Main_B.h Request_B.h
	$(CC) -c LiftR_B.c $(CFLAGS)

Request.o: Request_B.c Request_B.h
	$(CC) -c Request_B.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)