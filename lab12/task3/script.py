import random

a = []

for i in range(10000):
    a.append(i)

a.reverse()

with open("./tests/3.txt", 'w') as file:
    for elem in a:
        file.write(str(elem) + "\n")