import sys
import os
import subprocess
from tempfile import NamedTemporaryFile
import time

def compile_and_test(cpp_file, test_file, expected_file=None):
    """Полный цикл компиляции и тестирования"""
    
    print("\nЗапуск теста...")
    start_time = time.time()
    
    with open(test_file) as f:
        input_data = f.read()
    
    try:
        result = subprocess.run(["./r-tree"], 
                              input=input_data,
                              stdout=subprocess.PIPE, 
                              stderr=subprocess.PIPE,
                              text=True,
                              timeout=10)  # Лимит 10 секунд
    except subprocess.TimeoutExpired:
        print("Тест не завершился за 10 секунд!")
        return False
    
    elapsed = time.time() - start_time
    print(f"Время выполнения: {elapsed:.2f} сек")
    
    if result.returncode != 0:
        print("Программа завершилась с ошибкой:")
        print(result.stderr)
        return False
    
    # 3. Проверка результатов
    actual = result.stdout.strip().split('\n')
    
    if expected_file:
        with open(expected_file) as f:
            expected = f.read().strip().split('\n')
    else:
        expected = generate_expected_output(test_file)
    
    print("\nПроверка результатов...")
    errors = 0
    for i, (act, exp) in enumerate(zip(actual, expected)):
        if act != exp:
            if errors < 5:  # Покажем первые 5 ошибок
                print(f"Ошибка #{i+1}: ожидалось '{exp}', получено '{act}'")
            errors += 1
    
    total = len(expected)
    print(f"\nИтого: {total - errors} из {total} верно, ошибок: {errors}")
    
    if errors > 0:
        print("\nПервые неправильные ответы:")
        for i, (act, exp) in enumerate(zip(actual, expected)):
            if act != exp and i < 10:
                print(f"[{i+1}] Ожидалось: {exp}, получено: {act}")
    
    return errors == 0

def generate_expected_output(test_file):
    """Генерирует ожидаемые результаты на лету"""
    active = set()
    expected = []
    
    with open(test_file) as f:
        n = int(f.readline())
        for _ in range(n):
            parts = f.readline().strip().split()
            if not parts:
                continue
                
            cmd = parts[0]
            rect = tuple(parts[1:5])
            
            if cmd == 'insert':
                active.add(rect)
            elif cmd == 'delete':
                active.discard(rect)
            elif cmd == 'search':
                expected.append('1' if rect in active else '0')
    
    return expected

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Использование: python tester.py test_file.txt [expected_file.txt]")
        print("Пример: python tester.py stress_test.txt stress_test.txt.expected")
        sys.exit(1)
    
    test_file = sys.argv[1]
    expected_file = sys.argv[2] if len(sys.argv) > 2 else None
    
    cpp_program = "r-tree.cpp"
    
    success = compile_and_test(cpp_program, test_file, expected_file)
    
    if success:
        print("\n✅ Все тесты пройдены успешно!")
    else:
        print("\n❌ Обнаружены ошибки")
    
    sys.exit(0 if success else 1)