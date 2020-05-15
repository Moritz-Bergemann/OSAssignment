# Python script for randomly generating a set of requests

import random

NUM_FLOORS = 20
NUM_REQUESTS = 50

file = open("sim_input.txt", "w")
for i in range(NUM_REQUESTS):
    num1 = random.randint(1, NUM_FLOORS)
    num2 = random.randint(1, NUM_FLOORS)
    file.write(f"{num1} {num2}\n")

file.close()