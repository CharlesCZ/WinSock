#pragma comment(lib,"ws2_32.lib")
//#include<winsock.h>
#include<iostream>
#include <Ws2tcpip.h>
#include <string>
#include <stdio.h>
#include <time.h>
#include<string>
#include <sstream>
using namespace std;
//100-ustalanie sesji

//111-odrzuceni poloczenia
//222-potwierdzenie poloczenia
//333-zwykle poloczenie
//777-historia
//666-zamykanie poloczenia
struct Proto {

	string  CZ;//czas
	string OP;//operacja
	int ST;//status
	int ID;//id sesji
	int OO;//id obliczen
	float P1;//parametr 1
	float P2;//parametr 2
	float WY;//wynik
	int RZ;//rzad
	int KO;//kolumna

};
Proto p2;






//rozpakowanie
template < typename T >
T get_var(string &s);

template <>
string get_var(string &s)
{
	const size_t pos = s.find('$');
	const string result = s.substr(0, pos);
	s = s.substr(pos + 1);
	return result.substr(result.find('=') + 1);
}

template <>
int get_var(string &s)
{
	return atoi(get_var<string>(s).c_str());
}

template <>
float get_var(string &s)
{
	return static_cast<float>(atof(get_var<string>(s).c_str()));
}
Proto Odbierz_wiadomosc_sesji(SOCKET newConnection) {
	Proto p1;

	char* message = new char[1024];
	recv(newConnection, message, 1024, NULL);
	string s = std::string(message);

	p1.CZ = get_var<string>(s);
	p1.OP = get_var<string>(s);
	p1.ST = get_var<int>(s);
	p1.ID = get_var<int>(s);
	p1.OO = get_var<int>(s);
	p1.P1 = get_var<float>(s);
	p1.P2 = get_var<float>(s);
	p1.WY = get_var<float>(s);
	p1.RZ = get_var<int>(s);
	p1.KO = get_var<int>(s);

	cout << "klient otrzymal z sesji:CZ: " << p1.CZ << endl
		<< "klient otrzymal z sesji:OP: " << p1.OP << endl
		<< "klient otrzymal z sesji:ST: " << p1.ST << endl
		<< "klient otrzymal z sesji:ID:" << p1.ID << endl
		<< "klient otrzymal z sesji:OO: " << p1.OO << endl
		<< "klient otrzymal z sesji:P1: " << p1.P1 << endl
		<< "klient otrzymal z sesji:P2: " << p1.P2 << endl
		<< "klient otrzymal z sesji:WY: " << p1.WY << endl
		<< "klient otrzymal z sesji:RZ: " << p1.RZ << endl
		<< "klient otrzymal z sesji:KO: " << p1.KO << endl;


	return p1;

}





void Odpowiedz_na_sesje(SOCKET newConnection) {//CZ
	string pakiet;

	//CZ
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[21];
	tstruct = *localtime(&now);
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y %m %d %X", &tstruct);
	std::ostringstream oss;
	oss << "CZ=" << buf << '$';

	//operacja
	oss << "OP=" << "brak" << '$';
	//status
	int ST = 100;
	cout << endl << "polaczyc sie z serverem?0/1";
	int s = 0;
	cin >> s;
	switch (s) {
	case 0:
		ST = 111;
		break;
	case 1:
		ST = 222;
		break;
	}
	oss << "ST=" << ST << '$';
	//sesja

	oss << "ID=" << p2.ID << '$';

	//OO

	oss << "OO=" << p2.OO << '$';

	//P1
	oss << "P1=" << p2.P1 << '$';

	//P2
	oss << "P1=" << p2.P1 << '$';

	//WY
	oss << "WY=" << p2.WY << '$';

	//RZ
	int RZ = -1;
	oss << "RZ=" << p2.RZ << '$';

	//KO
	int KO = -1;
	oss << "KO=" << p2.KO << '$';

	pakiet = oss.str();


	char tab2[1024];
	strncpy_s(tab2, pakiet.c_str(), sizeof(tab2));
	//tab2[pakiet.size()] = 0;
	send(newConnection, tab2, pakiet.size(), NULL);//send the chat message to this client*/
												   //status
												   //111-odrzuceni poloczenia
												   //222-potwierdzenie poloczenia	

}



SOCKET Connection;//This client's connection to the server

void wyslij_zwykla_wiadomosc(SOCKET newConnection, string rodzajobliczen, float a, float b) {

	string pakiet;
	Proto p1;
	//CZ
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[21];
	tstruct = *localtime(&now);
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y %m %d %X", &tstruct);
	std::ostringstream oss;
	oss << "CZ=" << buf << '$';

	//operacja
	p2.OP = rodzajobliczen;
	oss << "OP=" << p2.OP << '$';

	//status
	p2.ST = 333;
	oss << "ST=" << p2.ST << '$';

	//sesja
	oss << "ID=" << p2.ID << '$';

	//OO
	p2.OO = 0;
	oss << "OO=" << p2.OO << '$';


	//P1	
	oss << "P1=" << a << '$';

	//P2
	oss << "P2=" << b << '$';


	//WY
	int WY = 0;
	oss << "WY=" << WY << '$';

	//RZ
	int RZ = -1;
	oss << "RZ=" << RZ << '$';

	//KO
	int KO = -1;
	oss << "KO=" << KO << '$';

	pakiet = oss.str();

	char tab2[1024];
	strncpy_s(tab2, pakiet.c_str(), sizeof(tab2));
	send(newConnection, tab2, pakiet.size(), NULL);//send the chat message to this client
}

Proto Odbierz_wiadomosc(SOCKET newConnection) {
	Proto p1;

	char* message = new char[1024];
	recv(newConnection, message, 1024, NULL);
	string s = std::string(message);

	p1.CZ = get_var<string>(s);
	p1.OP = get_var<string>(s);
	p1.ST = get_var<int>(s);
	p1.ID = get_var<int>(s);
	p1.OO = get_var<int>(s);
	p1.P1 = get_var<float>(s);
	p1.P2 = get_var<float>(s);
	p1.WY = get_var<float>(s);
	p1.RZ = get_var<int>(s);
	p1.KO = get_var<int>(s);

	cout << "klient otrzymal z sesji:CZ: " << p1.CZ << endl
		<< "klient otrzymal z sesji:OP: " << p1.OP << endl
		<< "klient otrzymal z sesji:ST: " << p1.ST << endl
		<< "klient otrzymal z sesji:ID:" << p1.ID << endl
		<< "klient otrzymal z sesji:OO: " << p1.OO << endl
		<< "klient otrzymal z sesji:P1: " << p1.P1 << endl
		<< "klient otrzymal z sesji:P2: " << p1.P2 << endl
		<< "klient otrzymal z sesji:WY: " << p1.WY << endl
		<< "klient otrzymal z sesji:RZ: " << p1.RZ << endl
		<< "klient otrzymal z sesji:KO: " << p1.KO << endl;


	return p1;

}
void ClientThread()
{
	p2 = Odbierz_wiadomosc_sesji(Connection);


	int s;

	while (true)
	{
		p2 = Odbierz_wiadomosc(Connection);


	}
}

void wyslij_historia_zapytanie(SOCKET newConnection) {
	string pakiet;
	//CZ
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[21];
	tstruct = *localtime(&now);
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y %m %d %X", &tstruct);
	std::ostringstream oss;
	oss << "CZ=" << buf << '$';

	//operacja
	p2.OP = "nieznane";
	oss << "OP=" << p2.OP << '$';

	//status
	p2.ST = 777;
	oss << "ST=" << p2.ST << '$';

	cout << "zmienic nr sesji?0/1";
	int zm = 0;
	cin >> zm;
	if (zm == 1)
		cin >> p2.ID;
	//sesja
	oss << "ID=" << p2.ID << '$';

	//OO
	cout << "jakie ID obliczen";
	cin >> p2.OO;
	oss << "OO=" << p2.OO << '$';


	//P1	
	oss << "P1=" << 0 << '$';

	//P2
	oss << "P2=" << 0 << '$';


	//WY
	int WY = 0;
	oss << "WY=" << WY << '$';
	//RZ
	int RZ = -1;
	oss << "RZ=" << RZ << '$';

	//KO
	int KO = -1;
	oss << "KO=" << KO << '$';

	pakiet = oss.str();

	char tab2[1024];
	strncpy_s(tab2, pakiet.c_str(), sizeof(tab2));
	send(newConnection, tab2, pakiet.size(), NULL);//send the chat message to this client
}

void wyslij_zamykanie_poloczenia(SOCKET newConnection) {
	string pakiet;
	//CZ
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[21];
	tstruct = *localtime(&now);
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y %m %d %X", &tstruct);
	std::ostringstream oss;
	oss << "CZ=" << buf << '$';

	//operacja
	p2.OP = "nieznane";
	oss << "OP=" << p2.OP << '$';

	//status
	p2.ST = 666;
	oss << "ST=" << p2.ST << '$';


	//sesja
	oss << "ID=" << p2.ID << '$';

	//OO
	p2.OO = 0;
	oss << "OO=" << p2.OO << '$';


	//P1	
	oss << "P1=" << 0 << '$';

	//P2
	oss << "P2=" << 0 << '$';


	//WY
	int WY = 0;
	oss << "WY=" << WY << '$';
	//RZ
	int RZ = -1;
	oss << "RZ=" << RZ << '$';

	//KO
	int KO = -1;
	oss << "KO=" << KO << '$';

	pakiet = oss.str();

	char tab2[1024];
	strncpy_s(tab2, pakiet.c_str(), sizeof(tab2));
	send(newConnection, tab2, pakiet.size(), NULL);//send the chat message to this client
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

	SOCKADDR_IN addr; //Address that we will bind our listening socket to
	inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr)); // IPv4
	int addrlen = sizeof(addr); //length of the address (required for accept call)
	addr.sin_addr = addr.sin_addr; //Broadcast locally//do kasacji
	addr.sin_port = htons(7777); //Port
	addr.sin_family = AF_INET; //IPv4 Socket

	Connection = socket(AF_INET, SOCK_STREAM, NULL); //Set Connection socket
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) //If we are unable to connect...
	{
		MessageBoxA(NULL, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
		return 0; //Failed to Connect
	}

	std::cout << "Connected!" << std::endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL); //Create the client thread that will receive any data that the server sends.
	Sleep(500);

	Odpowiedz_na_sesje(Connection);
	char buffer[256]; //256 char buffer to send message
	while (true)
	{
		Sleep(1000);
		cout << endl << "dodaj, odejmij, logarytmuj, poteguj, historia , zamknij?0/1/2/3/4/5" << endl;
		int s = 0;
		cin >> s;
		cout << "wpisz parametry" << endl;
		cin >> p2.P1;
		cin >> p2.P2;
		switch (s) {
		case 0:
			wyslij_zwykla_wiadomosc(Connection, "dodaj", p2.P1, p2.P2);
			break;
		case 1:
			wyslij_zwykla_wiadomosc(Connection, "odejmij", p2.P1, p2.P2);
			break;
		case 2:
			wyslij_zwykla_wiadomosc(Connection, "logarytmuj", p2.P1, p2.P2);
			break;
		case 3:
			wyslij_zwykla_wiadomosc(Connection, "poteguj", p2.P1, p2.P2);
			break;
		case 4:
			wyslij_historia_zapytanie(Connection);
			break;
		case 5:
			wyslij_zamykanie_poloczenia(Connection);
			shutdown(Connection, 2);
			closesocket(Connection);
			return 0;
		}
		Sleep(10);
	}
	return 0;
}
