# Модуль 3. Задание 01 (Процессы, 2 балла).
   Написать программу, вычисляющую площади квадратов с заданной длиной стороны.
   Длины сторон передаются как аргументы запуска.
   Расчеты делают родительский и дочерний процессы, разделяя задачи примерно поровну.

# Реализация
1. Программа принимает аргументы командной строки, которые представляют собой длины сторон квадратов. 
   Если аргументы не переданы, выводится сообщение о неправильном запуске программы.
2. Аргументы преобразуются из строк в целые числа и хранятся в массиве 'sides'.
3. Для создания дочернего процесса используется `fork()`. 
   Если `fork()` возвращает 0, это означает, что мы находимся в дочернем процессе.
4. В дочернем процессе вычисляются площади квадратов, начиная с половины массива (чтобы поделить задачу примерно пополам).
   В родительском процессе вычисляются площади для первой половины массива.
5. Родительский процесс ждет завершения дочернего процесса с помощью `wait()`.