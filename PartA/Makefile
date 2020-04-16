CC = gcc
CFLAGS = -Wall -Werror -ansi -pedantic -std=c89 -lpthread
OBJ = 
EXEC = lift_sim_A


$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(CFLAGS)

Main.o: Main.c Main.h

MainMenu.o: MainMenu.c MainMenu.h LinkedList.h FileManager.h GameManager.h
	$(CC) -c MainMenu.c $(CFLAGS)

GameManager.o: GameManager.c LinkedList.h MainMenu.h FileManager.h GameManager.h
	$(CC) -c GameManager.c $(CFLAGS)

FileManager.o: FileManager.c LinkedList.h MainMenu.h GameManager.h FileManager.h
	$(CC) -c FileManager.c $(CFLAGS)

LinkedList.o: LinkedList.c LinkedList.h
	$(CC) -c LinkedList.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)
