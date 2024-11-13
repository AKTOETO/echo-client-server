#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <thread>
#include <vector>
#include <unistd.h>

#define SOCKET_PATH "mysocket" // Путь к сокету

void handleClient(int clientSocket) {
    char buffer[1024];
    int bytesReceived;

    while (true) {
        // Ожидание сообщения от клиента
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cerr << "Клиент отключился." << std::endl;
            break; // Выход из цикла при отключении клиента
        }

        buffer[bytesReceived] = '\0'; // Завершение строки
        std::cout << "Получено сообщение от клиента: " << buffer << std::endl;

        // Отправка ответа клиенту
        const char* response = "Привет от сервера";
        send(clientSocket, response, strlen(response), 0);
    }

    // Закрытие сокета клиента
    close(clientSocket);
}

void listenForExit() {
    std::string command;
    while (true) {
        std::getline(std::cin, command);
        if (command == "exit") {
            std::cout << "Сервер завершает работу..." << std::endl;
            exit(0); // Завершение работы сервера
        }
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_un address;
    int addrlen = sizeof(address);
    std::vector<std::thread> threads; // Вектор для хранения потоков

    // Создание сокета
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    // Удаление старого сокета, если он существует
    unlink(SOCKET_PATH);

    // Настройка адреса сокета
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

    // Привязка сокета к адресу
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Ошибка привязки сокета");
        exit(EXIT_FAILURE);
    }

    // Начало прослушивания
    if (listen(server_fd, 5) < 0) {
        perror("Ошибка прослушивания");
        exit(EXIT_FAILURE);
    }

    std::cout << "Сервер запущен и ожидает подключения..." << std::endl;

    // Запуск потока для прослушивания команды завершения
    std::thread exitThread(listenForExit);

    while (true) {
        // Принятие входящего соединения
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("Ошибка принятия соединения");
            continue;
        }
        
        std::cout << "Новый клиент подключен." << std::endl;

        // Создание нового потока для обработки клиента
        threads.emplace_back(handleClient, new_socket);
    }

    // Закрытие серверного сокета (достигнуто только в случае завершения работы сервера)
    close(server_fd);

    // Дождаться завершения всех потоков
    for (auto &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    return 0;
}
