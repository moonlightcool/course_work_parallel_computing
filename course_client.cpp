#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <time.h>
#pragma warning(disable: 4996)

using namespace std;
int main(int argc, char* argv[])
{
	srand(time(NULL));
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		cout << "Error" << endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		std::cout << "Error: failed connect to server.\n";
		return 1;
	}
	cout << "Connected!\n";
	char n[100];
	cout<<"How much threads do u want?(1-10)"<<endl;
	cin >> n;
	send(Connection, n, sizeof(n), NULL);
	char n2[100];
	cout << "Enter the key word" << endl;
	cin >> n2;
	send(Connection, n2, sizeof(n2), NULL);
	char s[100];
	for(int i=0;i<20;i++)
	{
		recv(Connection, s, sizeof(s), NULL);
		cout << s << endl;
	}
	return 0;
}