#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>
#include <vector>

#define SOCKET_PATH "mysocket" // Путь к сокету

int g_server_fd;                    // сокет сервера
std::vector<std::thread> g_threads; // Вектор для хранения потоков

void handleClient(int client_socket)
{
    char buffer[1024];
    int bytes_received;

    while (true)
    {
        // Ожидание сообщения от клиента
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0)
        {
            std::cerr << "Клиент отключился." << std::endl;
            break; // Выход из цикла при отключении клиента
        }

        buffer[bytes_received] = '\0'; // Завершение строки
        std::cout << client_socket << ": " << buffer << std::endl;

        // Отправка ответа клиенту
        std::string response ("server: ");
		response += buffer;
        send(client_socket, response.c_str(), response.length(), 0);
    }

    // Закрытие сокета клиента
    close(client_socket);
}

void exiting()
{
    // Закрытие серверного сокета (достигнуто только в случае завершения работы сервера)
    close(g_server_fd);

    // Дождаться завершения всех потоков
    for (auto &t : g_threads)
    {
        t.detach();
    }

	// удаление файла сокета
	unlink(SOCKET_PATH);

    exit(0);
}

void listenForExit()
{
    std::string command;
    while (true)
    {
        std::getline(std::cin, command);
        if (command == "exit")
        {
            std::cout << "Сервер завершает работу..." << std::endl;
            exiting();
        }
    }
}

int main()
{
    int new_socket;
    struct sockaddr_un address;
    int addrlen = sizeof(address);

    // Создание сокета
    if ((g_server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
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
    if (bind(g_server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Ошибка привязки сокета");
        exit(EXIT_FAILURE);
    }

    // Начало прослушивания
    if (listen(g_server_fd, 5) < 0)
    {
        perror("Ошибка прослушивания");
        exit(EXIT_FAILURE);
    }

    std::cout << "Сервер запущен и ожидает подключения..." << std::endl;

    // Запуск потока для прослушивания команды завершения
    std::thread exit_thread(listenForExit);

    while (true)
    {
        // Принятие входящего соединения
        new_socket = accept(g_server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0)
        {
            perror("Ошибка принятия соединения");
            break;
        }

        std::cout << "Новый клиент подключен." << std::endl;

        // Создание нового потока для обработки клиента
        g_threads.emplace_back(handleClient, new_socket);
    }

    return 0;
}
