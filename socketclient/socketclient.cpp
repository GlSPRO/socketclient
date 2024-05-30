#define WIN32_LEAN_AND_MEAN // Макрос WIN32_LEAN_AND_MEAN уменьшает объем включаемых заголовочных файлов Windows.h для оптимизации сборки
#include <iostream> // Включаем заголовочный файл для ввода-вывода
#include <Windows.h> // Заголовочный файл для функций Windows API
#include <WinSock2.h> // Заголовочный файл для Winsock API
#include <WS2tcpip.h> // Заголовочный файл для структур и функций TCP/IP

using namespace std;

int main() {
    SetConsoleCP(1251); // Устанавливаем кодировку консоли для поддержки кириллицы
    setlocale(LC_ALL, "Russian"); // Устанавливаем русскую локаль для корректного отображения русских символов

    WSADATA wsaData; // Структура для хранения информации о реализации Winsock
    ADDRINFO hints; // Структура для задания параметров соединения
    ADDRINFO* addrResult; // Указатель на структуру для хранения результата getaddrinfo
    SOCKET ClientSocket = INVALID_SOCKET; // Сокет для клиента

    const char* sendBuffer = "Hello from Client\n"; // Буфер для отправляемых данных
    const char* sendBufferTwo = "Howdy, folks?"; // Второй буфер для отправляемых данных
    char recvBuffer[512]; // Буфер для принимаемых данных

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Инициализация Winsock

    if (result != 0) {
        cout << "WSAStartup failed" << endl; // Если инициализация не удалась, выводим сообщение об ошибке
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); // Обнуляем структуру hints
    hints.ai_family = AF_INET; // Указываем семейство адресов (IPv4)
    hints.ai_socktype = SOCK_STREAM; // Указываем тип сокета (потоковый)
    hints.ai_protocol = IPPROTO_TCP; // Указываем протокол (TCP)

    result = getaddrinfo("localhost", "788", &hints, &addrResult); // Получаем информацию об адресе

    if (result != 0) {
        cout << "getaddrinfo failed" << endl; // Если получение информации об адресе не удалось, выводим сообщение об ошибке
        return 1;
    }

    ClientSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); // Создаем сокет для клиента

    if (ClientSocket == INVALID_SOCKET) {
        cout << "socket creation with error" << endl; // Если создание сокета не удалось, выводим сообщение об ошибке
        freeaddrinfo(addrResult); // Освобождаем ресурсы
        return 1;
    }

    result = connect(ClientSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); // Устанавливаем соединение с сервером

    if (result == SOCKET_ERROR) {
        cout << "Unable to connect server" << endl; // Если соединение с сервером не удалось, выводим сообщение об ошибке
        freeaddrinfo(addrResult); // Освобождаем ресурсы
        WSACleanup(); // Завершаем использование Winsock
        return 1;
    }

    result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), MSG_DONTROUTE); // Отправляем данные серверу

    if (result == SOCKET_ERROR) {
        cout << "send Failed" << endl; // Если отправка не удалась, выводим сообщение об ошибке
        freeaddrinfo(addrResult); // Освобождаем ресурсы
        WSACleanup(); // Завершаем использование Winsock
        return 1;
    }

    result = send(ClientSocket, sendBufferTwo, (int)strlen(sendBufferTwo), MSG_DONTROUTE); // Отправляем второй набор данных серверу

    if (result == SOCKET_ERROR) {
        cout << "send Failed" << endl; // Если отправка не удалась, выводим сообщение об ошибке
        freeaddrinfo(addrResult); // Освобождаем ресурсы
        WSACleanup(); // Завершаем использование Winsock
        return 1;
    }

    result = shutdown(ClientSocket, SD_SEND); // Закрываем отправку данных

    if (result == SOCKET_ERROR) {
        cout << "shutdown error" << endl; // Если ошибка при закрытии отправки данных, выводим сообщение об ошибке
        freeaddrinfo(addrResult); // Освобождаем ресурсы
        WSACleanup(); // Завершаем использование Winsock
        return 1;
    }

    do {
        ZeroMemory(recvBuffer, 512); // Обнуляем буфер приема
        result = recv(ClientSocket, recvBuffer, 512, 0); // Принимаем данные от сервера
        if (result > 0) {
            cout << "Получено " << result << " байт" << endl; // Выводим количество принятых байт
            cout << "Получено " << recvBuffer << endl; // Выводим принятые данные
        }
        else if (result == 0) {
            cout << "Соединение прервано" << endl; // Если соединение завершено, выводим соответствующее сообщение
        }
        else {
            cout << "Сообщение не получено" << endl; // Если прием данных не удался, выводим сообщение об ошибке
        }
    } while (result > 0); // Повторяем до тех пор, пока есть данные для приема

    closesocket(ClientSocket); // Закрываем сокет клиента
    freeaddrinfo(addrResult); // Освобождаем ресурсы
    WSACleanup(); // Завершаем использование Winsock
}