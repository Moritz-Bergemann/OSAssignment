CC = gcc
CFLAGS = -Wall -Werror -pedantic -lpthread -lm -g
OBJ = Main.o Lift.o LiftR.o Request.o
EXEC = lift_sim_A

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(CFLAGS)

Main.o: Main.c Main.h LiftR.h Lift.h Request.h
	$(CC) -c Main.c $(CFLAGS)

Lift.o: Lift.c Lift.h Main.h Request.h
	$(CC) -c Lift.c $(CFLAGS)

LiftR.o: LiftR.c LiftR.h Main.h Request.h
	$(CC) -c LiftR.c $(CFLAGS)

Request.o: Request.c Request.h
	$(CC) -c Request.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)