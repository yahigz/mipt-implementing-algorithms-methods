import random

a = []

for i in range(500):
    a.append(i)

random.shuffle(a)

for elem in a:
    print(elem)