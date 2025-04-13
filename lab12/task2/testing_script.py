import subprocess
import os

def compile_cpp():
    """Компилируем C++ программу"""
    result = subprocess.run(
        ['g++', 'algo.cpp', '-o', 'algo'],
        stderr=subprocess.PIPE
    )
    if result.returncode != 0:
        print("Ошибка компиляции:")
        print(result.stderr.decode())
        return False
    return True

def run_test(alpha, test_file):
    """Запускаем программу с указанным alpha и тестовым файлом"""
    with open(test_file, 'r') as f:
        input_data = f.read()
    
    result = subprocess.run(
        ['./algo', str(alpha)],
        input=input_data,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    
    if result.returncode != 0:
        print(f"Ошибка выполнения для alpha={alpha}:")
        print(result.stderr)
        return None
    
    return result.stdout.strip()

def main():
    if not compile_cpp():
        return
    
    test_file = 'random_test.txt'
    output_file = 'testing_data_random.txt'
    
    with open(output_file, 'w') as out_f:
        alpha = 0.5
        while alpha <= 1.0 + 1e-9:  # Добавляем небольшой эпсилон для учёта погрешности float
            result = run_test(alpha, test_file)
            if result is not None:
                out_f.write(result + '\n')
                print(f"Успешно выполнен тест для alpha={alpha:.2f}")
            alpha = round(alpha + 0.05, 2)  # Округляем для избежания погрешности float

    print(f"\nВсе результаты сохранены в {output_file}")

if __name__ == '__main__':
    main()