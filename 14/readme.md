# Модуль 3. Задание 14 (UDP сокеты, 2 балла)
    Реализовать чат для двух клиентов с использованием UDP-сокетов.

# Реализация
    Программа разделена на две основные части udp-сервер и udp-клиент.
## `udp_server.c`
    Это простейший UDP-сервер, который принимает сообщения от клиентов и отправляет их обратно.
- Программа на вход принимает аргументы командной строки. Ожидает один аргумент — номер порта. Если аргумент отсутствует, программа выдает сообщение о неправильном использовании и завершает работу.
- Здесь создается несколько переменных:
    `sockfd` - дескриптор сокета;
    `len` - переменная для хранения длины адреса клиента;
    `n` - еременная для хранения количества полученных байтов;
    `line` - массив для хранения сообщения от клиента;
    `servaddr` и `cliaddr` - структуры для хранения адреса сервера и клиента соответственно.
- Далее заполняется структура для адреса сервера
    `bzero(&servaddr, sizeof(servaddr));` — обнуление структуры `servaddr`
    `sin_family` устанавливается как `AF_INET`, что указывает, что используется IPv4
    `port` получает значение из аргументов командной строки после преобразования из строки в целое число
    Если номер порта равен 0 (что недопустимо), программа завершает работу
    `htons(port)` преобразует номер порта в сетевой порядок байтов (big-endian).
    `htons(INADDR_ANY)` устанавливает адрес сервера на всех доступных интерфейсах
- Создается сокет с использованием протокола `PF_INET` (IPv4) и типа `SOCK_DGRAM` (UDP). Если создание не удалось, выводится ошибка и программа завершается. Сокет связывается с указанным адресом и портом. Если операция не удалась, выводится ошибка, сокет закрывается и программа завершается.
- В основной части программа входит в бесконечный цикл, в котором она слушает входящие сообщения от клиентов. Размер адреса клиента сохраняется в `len`. С помощью `recvfrom` программа принимает сообщение, а также информацию о клиенте. Если происходит ошибка, выводится сообщение об ошибке, сокет закрывается, и программа завершает работу.
- После успешного получения сообщения:
    Программа выводит полученное сообщение на экран.
    Затем она отправляет это сообщение обратно клиенту с помощью `sendto`, передавая адрес клиента и длину адреса. Если возникает ошибка во время отправки, выводится сообщение об ошибке, сокет закрывается, и программа завершает работу.

## `udp_client.c`
    Эта программа является клиентом для UDP-сервера, который отправляет сообщения на сервер и получает ответы.
- Программа начинается с проверки количества аргументов командной строки. Ожидается, что пользователь введет два аргумента: IP-адрес сервера и номер порта. Если аргументы не соответствуют ожиданиям, программа выдает сообщение о неправильном использовании и завершает свою работу.
- Создаются несколько переменных:
    `sockfd`: дескриптор сокета. Изначально инициализируется значением -1.
    `line`: массив для хранения сообщения, которое будет отправлено на сервер и получено от него.
    `servaddr`: структура, используемая для хранения адреса сервера.
- Создается сокет с использованием протокола `PF_INET` (IPv4) и типа `SOCK_DGRAM` (UDP). Если создание сокета не удалось, программа завершает работу с выводом сообщения об ошибке.
- Заполняется структура для адреса сервера
    `bzero(&servaddr, sizeof(servaddr));`: обнуляет структуру `servaddr`.
    `sin_family` устанавливается как `AF_INET`, что указывает, что используется IPv4.
    Номер порта извлекается из аргументов командной строки и преобразуется в целое число с использованием `atoi()`.
    `htons(port)`: преобразует номер порта в сетевой порядок байтов (big-endian).
    `inet_pton(AF_INET, argv[1], &servaddr.sin_addr);`: преобразует строковое представление IP-адреса в сетевой формат и сохраняет его в поле `sin_addr` структуры `servaddr`.
- В основной части программа входит в бесконечный цикл, в котором пользователю предлагается ввести сообщение. После ввода сообщения программа отправляет его на сервер с использованием функции `sendto()`. Здесь `strlen(line) + 1` используется для отправки длины сообщения вместе с завершающим нулем (для корректного завершения строки).
- Программа ожидает ответа от сервера с помощью функции `recvfrom()`. Если получение сообщения не удалось, выводится сообщение об ошибке, сокет закрывается, и программа завершает работу.
- Наконец, полученный ответ от сервера выводится на экран.
- После выхода из цикла (который в данном случае бесконечный, поэтому ctrl+C), сокет закрывается, и программа завершается.
