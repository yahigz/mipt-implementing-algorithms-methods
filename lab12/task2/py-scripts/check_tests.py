import os
import subprocess

def compile_cpp():
    """Компилируем algo.cpp в исполняемый файл"""
    if not os.path.exists('./algo.cpp'):
        print("Ошибка: файл ./algo.cpp не найден")
        return False
    
    result = subprocess.run(
        ['g++', './algo.cpp', '-o', './algo'],
        stderr=subprocess.PIPE
    )
    
    if result.returncode != 0:
        print("Ошибка компиляции:")
        print(result.stderr.decode())
        return False
    
    return True

def run_test(test_file, expected_file):
    """Запускаем тест и сравниваем результат с ожидаемым"""
    with open(test_file, 'r') as tf, open(expected_file, 'r') as ef:
        expected = ef.read().strip()
        
        result = subprocess.run(
            ['././algo'],
            stdin=tf,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        
        if result.returncode != 0:
            print(f"Ошибка при выполнении для {test_file}:")
            print(result.stderr)
            return False
        
        output = result.stdout.strip()
        return output == expected

def main():
    if not compile_cpp():
        return
    
    test_dir = './tests'
    passed = 0
    total = 0
    
    for i in range(50):
        test_file = os.path.join(test_dir, f'random{i:02d}.txt')
        expected_file = os.path.join(test_dir, f'random{i:02d}.a.txt')
        
        if not os.path.exists(test_file) or not os.path.exists(expected_file):
            continue
        
        total += 1
        if run_test(test_file, expected_file):
            print(f"Тест random{i:02d}.txt: PASSED")
            passed += 1
        else:
            print(f"Тест random{i:02d}.txt: FAILED")
    
    print(f"\nИтого: {passed} из {total} тестов пройдено")
    if passed == total:
        print("Все тесты пройдены успешно!")
    else:
        print(f"Не пройдено тестов: {total - passed}")

if __name__ == '__main__':
    main()