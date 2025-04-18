import random
random.seed(47)
matrix1 = [[random.randint(0, 99) for _ in range(35)] for _ in range(35)]
for row in matrix1:
    print(' '.join(map(str, row)))