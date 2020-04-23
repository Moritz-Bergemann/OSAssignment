import random

NUM_FLOORS = 20

file = open("sim_input.txt", "w")
for i in range(50):
    num1 = random.randint(1, NUM_FLOORS)
    num2 = random.randint(1, NUM_FLOORS)
    file.write(f"{num1} {num2}\n")

file.close()