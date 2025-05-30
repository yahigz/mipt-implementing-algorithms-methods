***Scapegoat Tree***

На случайных тестах Scapegoat Tree показывает себя достаточно хорошо. На максимальном тесте производительность низкая - 20000 запросов обработалось за 1100 мс в лучшем случае (при $\alpha = 0.8$), то есть хуже аналогичных структур (КЧД) примерно в 5-15 раз, также $\alpha = 0.8$ является оптимальным значением по результатам макс теста и случайного теста. 

*График зависимости времени работы от alpha, случайные данные*

![График зависимости времени работы от alpha, случайные данные](./pictures/performance_plot_random.png)

*График зависимости времени работы от alpha, худший случай*

![График зависимости времени работы от alpha, худший случай](./pictures/performance_plot_max.png)

*График зависимости времени работы insert от alpha, худший случай*

![График зависимости времени работы insert от alpha, худший случай](./pictures/performance_plot_max_insert.png)

*График зависимости времени работы delete от alpha, худший случай*

![График зависимости времени работы delete от alpha, худший случай](./pictures/performance_plot_max_delete.png)

Видно, что delete очень сильно медленее insert в худшем случае. Стоит попытаться оптимизировать операцию удаления. 