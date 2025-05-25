
count = 10_000

with open("max_test.txt", 'w') as f:
    f.write(str(count * 2) + "\n")
    for i in range(count):
        f.write("insert " + str(i) + "\n")
    i = count - 1
    while (i >= 0):
        f.write("delete " + str(i) + "\n")
        i -= 1

    