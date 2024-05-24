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
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для подключения к серверу
    const char* sendBuffer1 = "Hello from Client - Message 1"; // Первое сообщение от клиента
    const char* sendBuffer2 = "Hello from Client - Message 2"; // Второе сообщение от клиента
    char recvBuffer[512]; // Буфер для приема данных от сервера

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
    result = getaddrinfo("localhost", "666", &hints, &addrResult); // Получаем информацию об адресе сервера
    if (result != 0) {
        cout << "getaddrinfo failed with error " << result << endl;
        WSACleanup();
        return 1;
    }
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); // Создаем сокет для подключения
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed with error " << WSAGetLastError() << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); // Подключаемся к серверу
    if (result == SOCKET_ERROR) {
        cout << "Error connecting to server " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Отправляем первое сообщение
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed with error " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "First message sent successfully" << endl;

    // Принимаем ответ от сервера
    ZeroMemory(recvBuffer, 512); // Обнуляем буфер приема
    result = recv(ConnectSocket, recvBuffer, 512, 0);
    if (result > 0) {
        cout << "Received " << result << " bytes: " << recvBuffer << endl;
    }
    else if (result == 0) {
        cout << "Connection closed by server" << endl;
    }
    else {
        cout << "Receive failed with error " << WSAGetLastError() << endl;
    }

    // Отправляем второе сообщение
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed with error " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Second message sent successfully" << endl;

    // Принимаем ответ от сервера
    ZeroMemory(recvBuffer, 512); // Обнуляем буфер приема
    result = recv(ConnectSocket, recvBuffer, 512, 0);
    if (result > 0) {
        cout << "Received " << result << " bytes: " << recvBuffer << endl;
    }
    else if (result == 0) {
        cout << "Connection closed by server" << endl;
    }
    else {
        cout << "Receive failed with error " << WSAGetLastError() << endl;
    }

    closesocket(ConnectSocket); // Закрываем сокет
    freeaddrinfo(addrResult); // Очищаем информацию об адресе
    WSACleanup(); // Очищаем Winsock

    return 0;
}
