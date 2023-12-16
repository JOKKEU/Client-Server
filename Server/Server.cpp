#define _WIN32_LEAN_AND_MEAN // ���������� MS


#include <iostream>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>




#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4005)

using namespace std;

void check(SOCKET socket, ADDRINFO* addrResult)
{
    closesocket(socket); // ��������� ������
    socket = INVALID_SOCKET; // ��������
    freeaddrinfo(addrResult);
}

int main()
{
    WSADATA wsaData;
    ADDRINFO hints; // ���������� ��� ������� � �������
    ADDRINFO* addrResult = NULL; // ���� ������� ������ � �������
    SOCKET ClientSocket = INVALID_SOCKET; // ���������� ������
    SOCKET ListenSocket = INVALID_SOCKET; // ��������� ������

    const char* sendBuffer = "Your enter messange to server!"; // ��� �������
    char recvBuffer[1024];

    int result; // ��� ��������
        
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        cout << "WSAStartup failed, resilt " << result << endl;
        return 1;
    }

    cout << "----Server Activated----" << endl;

    ZeroMemory(&hints, sizeof(hints)); // �������� ��� ���� hints (��� ��������� hints ����� 0)
    hints.ai_family = AF_INET; // ethernet
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP; // �������� TCP/IP
    hints.ai_flags = AI_PASSIVE; // ���������� ��� ��������� ������� (�.� ������ ��������� �������, � ������ ��������)

    result = getaddrinfo(NULL, "666", &hints, &addrResult);  // ���������� � �������
    if (result != 0)
    {
        cout << "getaddrinfo failed with error: " << result << endl;
        WSACleanup();
        return 1;
    }

    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult); // ����������� addrResult
        WSACleanup();
        return 1;
    }

    result = bind(ListenSocket, addrResult->ai_addr, static_cast<int>(addrResult->ai_addrlen));

    if (result == SOCKET_ERROR)
    {
        cout << "Binding socket failed" << endl;
        check(ListenSocket, addrResult);
        return 1;
    }

    result = listen(ListenSocket, SOMAXCONN); // ������ �������� - ������� �������� ����� ������� ������

    if (result != 0)
    {
        cout << "Listen socket failed" << endl;
        check(ListenSocket, addrResult);
        return 1;
    }

    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        cout << "Accepting socket failed" << endl;
        check(ListenSocket, addrResult);
        return 1;
    }

    closesocket(ListenSocket); // �.� ������ �� ����� ������� ���������� �������
    int count_messange = 1;
    do {

        ZeroMemory(recvBuffer, sizeof(recvBuffer));
        result = recv(ClientSocket, recvBuffer, sizeof(recvBuffer), 0); // �������� ���������� �� ����������� �������

        if (result > 0)
        {
            cout << "(" << count_messange << ") Received data: " << recvBuffer << endl;
            result = send(ClientSocket, sendBuffer, static_cast<int>(strlen(sendBuffer)), 0); // ���������� ������� ��������� �� �������
            if (result == SOCKET_ERROR)
            {
                cout << "Failed to send data back" << endl;
                check(ClientSocket, addrResult);
                return 1;
            }
        }
        else if (result == 0)
        {
            cout << "Connection closing..." << endl;
        }
        else
        {
            cout << "Recv failed with error" << endl;
            check(ClientSocket, addrResult);
            return 1;
        }
        ++count_messange;
    } while (result > 0);

    result = shutdown(ClientSocket, SD_SEND);
    if (result == SOCKET_ERROR)
    {
        cout << "shutdown client socket failed" << endl;
        check(ClientSocket, addrResult);
        return 1;
    }

    check(ClientSocket, addrResult);

    return 0;

}
