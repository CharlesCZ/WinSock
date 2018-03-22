#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996) 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include<iostream>
#include <Ws2tcpip.h>
#include <string>
#include <bitset>
#include <random>
#include <string>
#include<vector>
using namespace std;
//-D_SCL_SECURE_NO_WARNINGS
int aktualne_id_sesji;

struct Protocol {
	bool operacja[4];
	bool odpowiedz[3];
	int64_t dlugosc;
	string dane;
	int id_sesji;
};
unsigned char ToByte(bool b[8])
{
	unsigned char c = 0;
	for (int i = 0; i < 8; ++i)
		if (b[i])
			c |= 1 << i;
	return c;
}
void odczytaj_wiad(bitset<8 * 1024> newbitset) {

	unsigned char dl[8];
	bitset<8> pomoc;
	int p = 0;
	int dl_licznik = 0;
	for (int j = 63; j >= 0; j--) {

		pomoc[p] = newbitset[j + 7];
		if (p != 7) {
			p++;
		}
		else if (p == 7) {

			p = 0;
			bool boole[8];
			for (int i = 0; i < 8; i++) {
				boole[i] = pomoc[i];
			}
			dl[dl_licznik] = ToByte(boole);
			dl_licznik++;
		}

	}
	bitset<64> xd;
	int licznik2 = 0;
	for (int i = 0; i < 8; i++) {
		bitset <8>lol = dl[i];
		for (int i = 0; i < 8; i++) {
			xd[licznik2] = lol[i];
			licznik2++;
		}
	}
	int64_t val = 0;

	for (int i = xd.size() - 1; i >= 0; --i) {
		val += xd[i] ? (1LL << i) : 0LL;
	}
	int licznik = 71;
	bool litera[8];
	for (int i = 0; i < val; i++) {
		for (int j = 0; j < 8; j++) {
			litera[j] = newbitset[licznik];
			licznik++;
		}
		cout << ToByte(litera);
	}
	cout << endl;

}

bool* Utworzwiadomosc(Protocol p1, bool tab[]) {
	vector<bool> prot;

	for (int i = 0; i < 4; ++i) {
		prot.push_back(p1.operacja[i]);

	}
	for (int i = 0; i < 3; ++i) {
		prot.push_back(p1.odpowiedz[i]);
	}

	bitset<64> b = p1.dlugosc;

	for (int i = 63; i >= 0; i--) {

		prot.push_back(b[i]);
	}


	for (int i = 0; i < p1.dane.size(); ++i) {
		bitset<8> byte = p1.dane.c_str()[i];
		for (int j = 0; j < 8; ++j) {
			prot.push_back(byte[j]);

		}
	}
	bitset<32> bsesji = p1.id_sesji;

	for (int i = 0; i<32; i++) {
		prot.push_back(bsesji[i]);
	}
	prot.push_back(0);
	copy(prot.begin(), prot.end(), tab);


	return tab;
}

SOCKET Connections[2];
int TotalConnections = 0;
bitset<8 * 1024> konwersja(char buffer[1024]) {
	bitset<8 * 1024> bbb;
	int licznik = 0;
	for (int i = 0; i < 1024; i++) {
		bitset<8> bb;
		bb = buffer[i];
		for (int j = 0; j < 8; j++) {
			bbb[licznik] = bb[j];
			licznik++;
		}

	}
	return bbb;
}
void otrzymaj_odpowiedz_sesji(int i) {
	char * buffer = new char[1024];
	recv(Connections[i], buffer, 1024, NULL);
	bitset<8 * 1024> bbb = konwersja(buffer);
	odczytaj_wiad(bbb);
	bitset<8> bs = buffer[0];
	if (bs[0] == 0 && bs[1] == 0 && bs[2] == 0 && bs[3] == 1) {
		if (bs[4] == 0 && bs[5] == 0 && bs[6] == 0) {
			Protocol p2;

			p2.dane = "Inny klient chce sie polaczyc";
			p2.dlugosc = p2.dane.length();
			p2.id_sesji = aktualne_id_sesji;
			p2.operacja[0] = 0;
			p2.operacja[1] = 0;
			p2.operacja[2] = 0;
			p2.operacja[3] = 1;

			p2.odpowiedz[0] = 0;
			p2.odpowiedz[1] = 0;
			p2.odpowiedz[2] = 0;
			bool *pomoc = new bool[(p2.dlugosc + 1 + 8 + 4) * 8];
			bool *tab = Utworzwiadomosc(p2, pomoc);
			int l = 0;
			vector<char> bajty;
			for (int i = 0; i < (p2.dlugosc + 1 + 8 + 4); i++) {
				bool tablica[8];
				for (int j = 0; j < 8; j++) {
					tablica[j] = tab[l];
					l++;
				}
				bajty.push_back(ToByte(tablica));
			}
			char *tabelka = new char[bajty.size()];

			copy(bajty.begin(), bajty.end(), tabelka);

			//for (int i = 0; i < bajty.size(); i++) {
			//	cout << tabelka[i];
			//}
			send(Connections[1], tabelka, bajty.size(), NULL);
			delete[] tabelka;
		}
	}
	delete[] buffer;

}
void wyslij_wiadomosc_o_rozpoczeciu(std::string wiadomosc) {
	Protocol p1;
	bitset<4>op;

	p1.operacja[0] = 0;
	p1.operacja[1] = 0;
	p1.operacja[2] = 1;
	p1.operacja[3] = 1;

	p1.odpowiedz[0] = 0;
	p1.odpowiedz[1] = 0;
	p1.odpowiedz[2] = 0;

	p1.id_sesji = aktualne_id_sesji;
	std::string buftest = wiadomosc;
	p1.dlugosc = buftest.size();
	p1.dane = buftest;
	bool *pomoc = new bool[(p1.dlugosc + 1 + 8 + 4) * 8];
	bool *tab = Utworzwiadomosc(p1, pomoc);
	int l = 0;
	vector<char> bajty;
	for (int i = 0; i < (p1.dlugosc + 1 + 8 + 4); i++) {
		bool tablica[8];
		for (int j = 0; j < 8; j++) {
			tablica[j] = tab[l];
			l++;
		}
		bajty.push_back(ToByte(tablica));
	}
	char *tabelka = new char[bajty.size()];
	copy(bajty.begin(), bajty.end(), tabelka);
	send(Connections[0], tabelka, bajty.size(), NULL); //send Message
	send(Connections[1], tabelka, bajty.size(), NULL);
}
void otrzymaj_wiadomosc_od_drugiego() {
	char * buffer = new char[1024];
	recv(Connections[1], buffer, 1024, NULL);
	bitset<8 * 1024>bbb = konwersja(buffer);
	odczytaj_wiad(bbb);
	bitset<8> bs = buffer[0];
	if (bs[0] == 0 && bs[1] == 0 && bs[2] == 0 && bs[3] == 1) {
		if (bs[4] == 0 && bs[5] == 0 && bs[6] == 1) {
			wyslij_wiadomosc_o_rozpoczeciu("mozna zaczac pisac");

		}
	}
}
void wyslij_wiadomosc_sesji(int i, std::string wiadomosc) {
	Protocol p1;
	bitset<4>op;

	p1.operacja[0] = 0;
	p1.operacja[1] = 0;
	p1.operacja[2] = 0;
	p1.operacja[3] = 0;

	p1.odpowiedz[0] = 0;
	p1.odpowiedz[1] = 0;
	p1.odpowiedz[2] = 0;

	aktualne_id_sesji = p1.id_sesji = rand() % 256;
	cout << "id sesji to " << aktualne_id_sesji << endl;
	std::string s = std::to_string(aktualne_id_sesji);
	wiadomosc += s;
	std::string buftest = wiadomosc;
	p1.dlugosc = buftest.size();
	p1.dane = buftest;
	bool *pomoc = new bool[(p1.dlugosc + 1 + 8 + 4) * 8];
	bool *tab = Utworzwiadomosc(p1, pomoc);
	int l = 0;
	vector<char> bajty;
	for (int i = 0; i < (p1.dlugosc + 1 + 8 + 4); i++) {
		bool tablica[8];
		for (int j = 0; j < 8; j++) {
			tablica[j] = tab[l];
			l++;
		}
		bajty.push_back(ToByte(tablica));
	}
	char *tabelka = new char[bajty.size()];
	copy(bajty.begin(), bajty.end(), tabelka);

	send(Connections[i], tabelka, bajty.size(), NULL); //send Message

}

void ClientHandlerThread(int index) //index = the index in the SOCKET Connections array
{

	int bufferlength = 1024; //Holds the length of the message a client sent
	while (true)
	{

		char * buffer = new char[bufferlength]; //Allocate buffer
		recv(Connections[index], buffer, bufferlength, NULL); //get buffer message from client
		for (int i = 0; i < TotalConnections; i++) //For each client connection
		{
			if (i == index) //Don't send the chat message to the same user who sent it
				continue; //Skip user
			send(Connections[i], buffer, bufferlength, NULL);//send the chat message to client at index i
		}
		delete[] buffer; //Deallocate buffer to stop from leaking memory
	}
}
int main() {
	//WinSock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) //If WSAStartup returns anything other than 0, then that means an error has occured in the WinSock Startup.
	{
		MessageBoxA(NULL, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	SOCKADDR_IN addr;
	inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr));
	int addrlen = sizeof(addr);
	addr.sin_addr = addr.sin_addr;
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;


	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	int ConnectionCounter = 0;
	newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen);
	std::cout << "Client Connected!" << std::endl;
	Connections[0] = newConnection;
	TotalConnections += 1;
	newConnection = NULL;
	newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen);
	std::cout << "Client Connected!" << std::endl;
	Connections[1] = newConnection;
	TotalConnections += 1;
	wyslij_wiadomosc_sesji(0, "twoje id to ");
	otrzymaj_odpowiedz_sesji(0);
	otrzymaj_wiadomosc_od_drugiego();

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(0), NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(1), NULL, NULL);
	system("pause");
	return 0;
}