import random
from collections import defaultdict

def generate_stress_test(n=1000, rect_count=50):
    """Генерирует нагрузочный тест с интенсивными операциями"""
    # Создаем пул прямоугольников
    rect_pool = [(
        random.randint(0, 90),
        random.randint(10, 100),
        random.randint(0, 90),
        random.randint(10, 100)
    ) for _ in range(rect_count)]
    
    operations = []
    active_rects = set()
    op_stats = defaultdict(int)
    
    for _ in range(n):
        # Выбираем операцию с учетом текущего состояния
        if not active_rects or random.random() < 0.4:
            op = 'insert'
        else:
            op = random.choices(['search', 'delete', 'insert'], 
                               weights=[3, 2, 1])[0]
        
        # Выбираем прямоугольник - 70% из активных, 30% случайный
        if active_rects and random.random() < 0.7:
            rect = random.choice(list(active_rects))
        else:
            rect = random.choice(rect_pool)
        
        # Формируем операцию
        operations.append(f"{op} {' '.join(map(str, rect))}")
        
        # Обновляем статистику и состояние
        if op == 'insert':
            active_rects.add(rect)
        elif op == 'delete' and rect in active_rects:
            active_rects.remove(rect)
        
        op_stats[op] += 1
    
    print(f"Статистика операций: {dict(op_stats)}")
    return operations

def save_test_with_expected(filename, operations):
    """Сохраняет тест с ожидаемыми ответами"""
    active_rects = set()
    expected = []
    
    with open(filename, 'w') as f:
        f.write(f"{len(operations)}\n")
        for op in operations:
            f.write(f"{op}\n")
            parts = op.split()
            cmd = parts[0]
            rect = tuple(parts[1:5])
            
            if cmd == 'insert':
                active_rects.add(rect)
            elif cmd == 'delete':
                active_rects.discard(rect)
            elif cmd == 'search':
                expected.append('1' if rect in active_rects else '0')
    
    with open(f"{filename}.expected", 'w') as f:
        f.write("\n".join(expected))

# Генерация теста
stress_test = generate_stress_test(5000, 100)  # 5000 операций с 100 прямоугольниками
save_test_with_expected("./tests/1.txt", stress_test)