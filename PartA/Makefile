CC = gcc
CFLAGS = -Wall -Werror -pedantic -lpthread
OBJ = Main.o Lift.o LiftR.o
EXEC = lift_sim_A

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(CFLAGS)

Main.o: Main.c Main.h LiftR.h Lift.h
	$(CC) -c Main.c $(CFLAGS)

Lift.o: Lift.c Lift.h Main.h
	$(CC) -c Lift.c $(CFLAGS)

LiftR.o: LiftR.c LiftR.h Main.h
	$(CC) -c LiftR.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)