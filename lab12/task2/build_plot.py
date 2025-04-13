import matplotlib.pyplot as plt
import numpy as np

def load_data(filename):
    """Загружает данные из файла в формате 'alpha time'"""
    alphas = []
    times = []
    
    with open(filename, 'r') as f:
        for line in f:
            # Пропускаем пустые строки и строки с разделителями
            if line.strip() and not line.startswith('-'):
                try:
                    alpha, time = map(float, line.strip().split())
                    alphas.append(alpha)
                    times.append(time)
                except ValueError:
                    print(f"Пропуск строки с некорректными данными: {line.strip()}")
                    continue
    
    return np.array(alphas), np.array(times)

def plot_performance(alphas, times):
    """Строит и сохраняет график производительности"""
    plt.figure(figsize=(10, 6))
    
    # Создаём график с маркерами и линией
    plt.plot(alphas, times, 'o-', color='royalblue', linewidth=2, markersize=8,
             markerfacecolor='gold', markeredgewidth=1, markeredgecolor='navy')
    
    # Настраиваем заголовки и подписи
    plt.title('Зависимость времени работы от параметра alpha', fontsize=14, pad=20)
    plt.xlabel('Значение параметра alpha', fontsize=12, labelpad=10)
    plt.ylabel('Время работы (мс)', fontsize=12, labelpad=10)
    
    # Настраиваем сетку и пределы осей
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.xlim(min(alphas)-0.02, max(alphas)+0.02)
    plt.ylim(min(times)-5, max(times)+5)
    
    # Добавляем аннотации для точек
    for alpha, time in zip(alphas, times):
        plt.annotate(f'{time} мс', xy=(alpha, time), xytext=(0, 10),
                    textcoords='offset points', ha='center', fontsize=9)
    
    # Сохраняем график
    plt.savefig('performance_plot_random.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("График успешно сохранён в файл 'performance_plot_random.png'")

def main():
    input_file = 'testing_data_random.txt'
    
    try:
        alphas, times = load_data(input_file)
        
        if len(alphas) == 0:
            print("Ошибка: не удалось загрузить данные из файла")
            return
        
        print("Загруженные данные:")
        for alpha, time in zip(alphas, times):
            print(f"alpha={alpha:.2f}, время={time} мс")
        
        plot_performance(alphas, times)
        
    except FileNotFoundError:
        print(f"Ошибка: файл {input_file} не найден")
    except Exception as e:
        print(f"Произошла ошибка: {str(e)}")

if __name__ == '__main__':
    main()