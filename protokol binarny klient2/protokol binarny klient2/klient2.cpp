#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996) 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<string>
#include <Ws2tcpip.h>
#include<bitset>
#include<vector>
using namespace std;
int aktualne_id_sesji;
SOCKET Connection;//This client's connection to the server
bool flaga_niezmienna = 0;
unsigned char ToByte(bool b[8])
{
	unsigned char c = 0;
	for (int i = 0; i < 8; ++i)
		if (b[i])
			c |= 1 << i;
	return c;
}
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


	if (flaga_niezmienna == 0) {
		for (int i = 1; i < 5; i++) {
			for (int j = 0; j < 8; j++) {
				litera[j] = newbitset[licznik];
				licznik++;
			}
			aktualne_id_sesji = (aktualne_id_sesji << 8) | ToByte(litera);
		}
		flaga_niezmienna = 1;
	}
	cout << endl;
}
struct Protocol {


	bool operacja[4];
	bool odpowiedz[3];
	int64_t dlugosc;
	string dane;
	int id_sesji;

};

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

	for (int i = 0; i < 32; ++i) {
		prot.push_back(bsesji[i]);
	}
	prot.push_back(0);
	copy(prot.begin(), prot.end(), tab);
	return tab;
}
void wyslij_wiadomosc(std::string wiadomosc) {
	Protocol p1;
	bitset<4>op;

	p1.operacja[0] = 0;
	p1.operacja[1] = 0;
	p1.operacja[2] = 1;
	p1.operacja[3] = 0;

	p1.odpowiedz[0] = 0;
	p1.odpowiedz[1] = 0;
	p1.odpowiedz[2] = 0;



	p1.dlugosc = wiadomosc.size();
	p1.dane = wiadomosc;
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

	send(Connection, tabelka, bajty.size(), NULL);
	delete[] tabelka;
}
void ClientThread()
{
	int bufferlength = 1024;
	while (true)
	{

		char * buffer = new char[bufferlength];
		recv(Connection, buffer, bufferlength, NULL);
		bitset<8 * 1024> bbb = konwersja(buffer);
		odczytaj_wiad(bbb);
		delete[] buffer;
	}
}
void odbierz_wiadomosc_sesji() {

	char * buffer = new char[1024];

	recv(Connection, buffer, 1024, NULL);

	bitset<8 * 1024> bbb = konwersja(buffer);
	odczytaj_wiad(bbb);

	Protocol p2;
	p2.dane = "zgadzam sie na polaczenie";
	p2.dlugosc = p2.dane.length();
	p2.id_sesji = aktualne_id_sesji;
	p2.operacja[0] = 0;
	p2.operacja[1] = 0;
	p2.operacja[2] = 0;
	p2.operacja[3] = 1;

	p2.odpowiedz[0] = 0;
	p2.odpowiedz[1] = 0;
	p2.odpowiedz[2] = 1;
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
	send(Connection, tabelka, bajty.size(), NULL);
}
int main() {
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) //If WSAStartup returns anything other than 0, then that means an error has occured in the WinSock Startup.
	{
		MessageBoxA(NULL, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	SOCKADDR_IN addr; //Address that we will bind our listening socket to
	inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr)); // IPv4
	int addrlen = sizeof(addr); //length of the address (required for accept call)
	addr.sin_addr = addr.sin_addr; //Broadcast locally//do kasacji
	addr.sin_port = htons(1111); //Port
	addr.sin_family = AF_INET; //IPv4 Socket

	Connection = socket(AF_INET, SOCK_STREAM, NULL); //Set Connection socket
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) //If we are unable to connect...
	{
		MessageBoxA(NULL, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
		return 0; //Failed to Connect
	}

	std::cout << "Connected!" << std::endl;

	odbierz_wiadomosc_sesji();
	char * buffer = new char[1024];
	recv(Connection, buffer, 1024, NULL);
	bitset<8 * 1024> bbb = konwersja(buffer);
	odczytaj_wiad(bbb);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);
	std::string userinput;
	while (true)
	{
		//cout << endl;
		std::getline(std::cin, userinput);
		wyslij_wiadomosc(userinput.c_str());
		//cout << endl;
		Sleep(10);
	}
	return 0;
}