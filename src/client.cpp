#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "mysocket" // Путь к сокету

int main() {
    int sock = 0;
    struct sockaddr_un serv_addr;

    // Создание сокета
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Ошибка создания сокета" << std::endl;
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, SOCKET_PATH, sizeof(serv_addr.sun_path) - 1);

    // Подключение к серверу
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Ошибка подключения к серверу" << std::endl;
        close(sock);
        return -1;
    }

    std::string message;
    char buffer[1024] = {0}; // Буфер для получения ответа от сервера

    while (true) {
        std::cout << "Введите сообщение для отправки на сервер (или 'exit' для выхода): ";
        std::getline(std::cin, message);

        // Проверка на команду выхода
        if (message == "exit") {
            break; // Выход из цикла
        }

        // Отправка данных на сервер
        if(send(sock, message.c_str(), message.size(), 0) <= 0)
        {
            std::cerr << "Ошибка: сервер отключился" << std::endl;
            break;
        }
        
        // Ожидание ответа от сервера
        int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Завершение строки
            std::cout << "Ответ от сервера: " << buffer << std::endl;
        } else {
            std::cerr << "Ошибка при получении ответа от сервера" << std::endl;
            break; // Выход из цикла при ошибке
        }
    }

    // Закрытие сокета
    close(sock);
    return 0;
}
