import sys
import os
import subprocess
from tempfile import NamedTemporaryFile

def compile_cpp_program(cpp_file, output_name="r-tree"):
    """Компилирует C++ программу с помощью g++"""
    try:
        subprocess.run(["g++", "-std=c++17", "-O2", "-Wall", cpp_file, "-o", output_name], 
                      check=True)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Ошибка компиляции: {e}")
        return False

def run_cpp_program(program_path, input_file):
    """Запускает скомпилированную C++ программу с входным файлом"""
    try:
        with open(input_file, 'r') as f:
            result = subprocess.run([f"./{program_path}"], stdin=f, 
                                  stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                  text=True, check=True)
        return result.stdout.strip().split('\n')
    except subprocess.CalledProcessError as e:
        print(f"Ошибка выполнения программы: {e.stderr}")
        return None

def generate_expected_output(test_file):
    """Генерирует ожидаемый вывод для теста"""
    rectangles = set()
    expected = []
    
    with open(test_file, 'r') as f:
        n = int(f.readline())
        for _ in range(n):
            parts = f.readline().strip().split()
            if not parts:
                continue
                
            cmd = parts[0]
            rect = tuple(parts[1:5])  # Сохраняем как строки для точного сравнения
            
            if cmd == 'insert':
                rectangles.add(rect)
            elif cmd == 'delete':
                rectangles.discard(rect)
            elif cmd == 'search':
                expected.append('1' if rect in rectangles else '0')
    
    return expected

def compare_outputs(actual, expected):
    """Сравнивает фактический и ожидаемый вывод"""
    if len(actual) != len(expected):
        print(f"Несоответствие количества ответов: ожидалось {len(expected)}, получено {len(actual)}")
        return False
    
    all_correct = True
    for i, (act, exp) in enumerate(zip(actual, expected)):
        if act != exp:
            print(f"Ошибка в ответе #{i+1}: ожидалось '{exp}', получено '{act}'")
            all_correct = False
    
    return all_correct

def main():
    
    test_file = "manual-tests/test.txt"
    executable = "r-tree"
    
    # 2. Генерируем ожидаемый вывод
    expected_output = generate_expected_output(test_file)
    
    # 3. Запускаем программу с тестовыми данными
    actual_output = run_cpp_program(executable, test_file)
    if actual_output is None:
        sys.exit(1)
    
    # 4. Сравниваем результаты
    if compare_outputs(actual_output, expected_output):
        print("Все тесты пройдены успешно!")
        sys.exit(0)
    else:
        sys.exit(1)

if __name__ == "__main__":
    main()