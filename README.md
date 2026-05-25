RungeKuttaMethods
=================

Программа реализует расчёты для курсовой работы по методу Мерсона:

- задание 2: решение тестовой системы с известным точным решением;
- задание 3: построение графиков максимальной погрешности `e(h)` и `e(h)/h^5`;
- задание 4: построение орбиты Аренсторфа и графика скорости `(x', y')`.

Что нужно установить
--------------------

Для Ubuntu/WSL:

```bash
sudo apt update
sudo apt install -y build-essential cmake python3 python3-dev python3-numpy python3-matplotlib
```

Для Windows нужны CMake, компилятор C++17 и Python 3 с пакетами `numpy` и `matplotlib`.

Сборка
------

Из папки проекта:

```bash
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Запуск
------

Выполнить все задания:

```bash
./build/Runge-Cuttas all
```

Запустить отдельные части:

```bash
./build/Runge-Cuttas test
./build/Runge-Cuttas errors
./build/Runge-Cuttas orbit
```

Режимы:

- `test` - решает тестовую систему из задания 2 и сравнивает численное решение с точным;
- `errors` - строит графики `e(h)` и `e(h)/h^5` для задания 3;
- `orbit` - строит орбиту Аренсторфа и график скорости для задания 4;
- `all` - последовательно выполняет все три режима.

Выходные файлы
--------------

После запуска создаются:

- `task2_test_solution.png` - сравнение численного и точного решения;
- `task2_test_solution.csv` - таблица значений для тестовой задачи;
- `task3_errors.png` - графики `e(h)` и `e(h)/h^5`;
- `task3_errors.csv` - таблица ошибок для разных шагов;
- `task4_orbit.png` - орбита Аренсторфа в координатах `(x, y)`;
- `task4_velocity.png` - график скорости в координатах `(x', y')`;
- `task4_arenstorf_orbit.csv` - таблица значений для орбиты.

Настройка графиков задания 4
----------------------------

Различные графики орбиты можно получать, меняя параметры в функции `run_task4()` в файле `main.cpp`:

```cpp
value_type start_t = 0.0;
value_type period = 11.124340337;
value_type h = 0.001;
vector_of_values start_y = {0.994, 0.0, 0.0, -2.031732629557337};
```

Здесь:

- `period` - конечное время интегрирования;
- `h` - шаг метода Мерсона;
- `start_y` - начальное состояние `{x0, y0, x'(0), y'(0)}`.

После изменения параметров программу нужно пересобрать:

```bash
cmake --build build
./build/Runge-Cuttas orbit
```
