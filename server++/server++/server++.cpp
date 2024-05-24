#define WIN32_LEAN_AND_MEAN // Отключает редко используемые части Windows-заголовков для ускорения компиляции

#include <iostream> // Библиотека для ввода и вывода данных
#include <Windows.h> // Основная библиотека Windows API
#include <WinSock2.h> // Библиотека для работы с сокетами
#include <WS2tcpip.h> // Библиотека для IP-адресов
using namespace std;

int main()
{
    WSADATA wsaData; // Структура для хранения информации о реализации Windows Sockets
    ADDRINFO* addrResult; // Указатель на структуру с адресной информацией
    ADDRINFO hints; // Структура для задания критериев поиска адресной информации
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для подключения к клиенту
    SOCKET ListenSocket = INVALID_SOCKET; // Сокет для прослушивания входящих подключений

    const char* sendBuffer = "Hello from Server"; // Сообщение, которое сервер будет отправлять клиенту

    char recvBuffer[512]; // Буфер для приема данных от клиента

    // Инициализация Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed with result " << result << endl;
        return 1;
    }
    ZeroMemory(&hints, sizeof(hints)); // Обнуляем структуру hints

    hints.ai_family = AF_INET; // Указываем, что используем IPv4
    hints.ai_socktype = SOCK_STREAM; // Указываем, что используем потоковый сокет (TCP)
    hints.ai_protocol = IPPROTO_TCP; // Указываем протокол TCP
    hints.ai_flags = AI_PASSIVE; // Сокет будет использоваться для прослушивания

    // Получаем информацию об адресе
    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed with error " << result << endl;
        WSACleanup();
        return 1;
    }

    // Создаем сокет для прослушивания
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed with error " << WSAGetLastError() << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Привязываем сокет к адресу и порту
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Binding failed with error " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Начинаем прослушивание входящих подключений
    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        cout << "Listening failed with error " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Принимаем входящее подключение
    ConnectSocket = accept(ListenSocket, NULL, NULL);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Accepting connection failed with error " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    closesocket(ListenSocket); // Закрываем сокет прослушивания, так как он больше не нужен

    // Основной цикл обработки сообщений
    do {
        ZeroMemory(recvBuffer, 512); // Обнуляем буфер приема
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Принимаем данные от клиента
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;

            // Отправляем ответ клиенту
            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) {
                cout << "Send failed with error " << WSAGetLastError() << endl;
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }
        }
        else if (result == 0) {
            cout << "Connection closing" << endl;
            break;
        }
        else {
            cout << "Receive failed with error " << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    // Завершаем соединение
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed with error " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ConnectSocket); // Закрываем сокет
    WSACleanup(); // Очищаем Winsock

    return 0;
}
