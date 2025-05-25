import numpy as np

def read_matrix(filename):
    """Считывает матрицу из файла."""
    with open(filename, 'r') as f:
        matrix = []
        for line in f:
            row = list(map(int, line.strip().split()))
            matrix.append(row)
    return np.array(matrix)

def write_matrix(matrix, filename):
    """Записывает матрицу в файл (для отладки)."""
    with open(filename, 'w') as f:
        for row in matrix:
            f.write(' '.join(map(str, row)) + '\n')

def matrices_are_equal(a, b, tolerance=1e-6):
    """Проверяет, равны ли две матрицы с учетом погрешности."""
    return np.allclose(a, b, atol=tolerance)

# Загрузка матриц
matrix_a = read_matrix('./multiplication-tests/left.1.txt')
matrix_b = read_matrix('./multiplication-tests/right.1.txt')
expected_result = read_matrix('./multiplication-tests/1.a.txt')

# Проверка размерностей
if matrix_a.shape != (35, 35) or matrix_b.shape != (35, 35):
    raise ValueError("Матрицы должны быть размером 35x35")

# Перемножение матриц
computed_result = np.dot(matrix_a, matrix_b)

# Проверка результата
if matrices_are_equal(computed_result, expected_result):
    print("Результат совпадает с матрицей в multiplication.txt!")
else:
    print("Результат НЕ совпадает с матрицей в multiplication.txt.")
    # Для отладки можно записать вычисленную матрицу в файл
    write_matrix(computed_result, './multiplication-tests/1.a.checked.txt')
    print("Вычисленный результат сохранен в ./multiplication-tests/1.a.checked.txt")