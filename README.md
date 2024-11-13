# Простой эхо сервер и клиент

## Сборка и компиляция


```bash
cmake -B build . && cmake --build build
```

## Запуск

После сборки получим два исполняемых файла:
- `./build/server` - исполняемый файл сервера
- `./build/client` - исполняемый файл клиента

## Использование
Клиент и сервер оба считывают данных с терминала. При вводе `exit` происходит завершение работы.

При написании сообщений в консоль, они передадутся на сервер и там отобразятся в терминале, после чего придет ответ от сервера.

Сервер поддерживает соединение сразу с несколькими клиентами.
