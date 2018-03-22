#pragma comment(lib,"ws2_32.lib")
#include<iostream>
#include <Ws2tcpip.h>
#include<random>
#include<string>
#include <stdio.h>
#include <time.h>
#include<vector>
#include<array>
#include<bitset>
#include<math.h>
#include<sstream>
using namespace std;
struct polehistorii {
	int id_obl;
	float wartosc;

};
vector<vector<polehistorii>> Historia(4); //dodaj = 0 odejmij = 1,...
int sesja;

//100-ustalanie sesji
//111-odrzuceni poloczenia
//222-potwierdzenie poloczenia
//333-obliczenia
//444-brak obliczen
//555-niewalsciwy nr sesji
//777-historia
//666-zamykanie poloczenia
//999-poza zakresem


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


struct Proto {
	string  CZ;//czas
	string OP;//operacja
	int ST;//status
	int ID;//id sesji
	int OO;//identyfikator obliczen
	float P1;//parametr1
	float P2;
	float WY;//wynik
	int RZ;//rzad
	int KO;//kolumna
};
Proto Odbierz_potwierdzienie_wiadomosci_sesji(SOCKET newConnection) {
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
	cout << endl << endl << "potwierdzienie_wiadomosci_sesji" << endl;
	cout << "server otrzymal z sesji:CZ: " << p1.CZ << endl
		<< "server otrzymal z sesji:OP: " << p1.OP << endl
		<< "server otrzymal z sesji:ST: " << p1.ST << endl
		<< "server otrzymal z sesji:ID:" << p1.ID << endl
		<< "server otrzymal z sesji:OO: " << p1.OO << endl
		<< "server otrzymal z sesji:P1: " << p1.P1 << endl
		<< "server otrzymal z sesji:P2: " << p1.P2 << endl
		<< "server otrzymal z sesji:WY: " << p1.WY << endl
		<< "server otrzymal z sesji:RZ: " << p1.RZ << endl
		<< "server otrzymal z sesji:KO: " << p1.KO << endl << endl;


	return p1;

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

	cout << endl << endl << "pakiet" << endl;
	cout << "serwer otrzymal :CZ: " << p1.CZ << endl
		<< "serwer otrzymal :OP: " << p1.OP << endl
		<< "serwer otrzymal :ST: " << p1.ST << endl
		<< "serwer otrzymal :ID:" << p1.ID << endl
		<< "serwer otrzymal :OO: " << p1.OO << endl
		<< "serwer otrzymal :P1: " << p1.P1 << endl
		<< "serwer otrzymal :P2: " << p1.P2 << endl
		<< "serwer otrzymal :WY: " << p1.WY << endl
		<< "serwer otrzymal :RZ: " << p1.RZ << endl
		<< "serwer otrzymal :KO: " << p1.KO << endl << endl;


	return p1;

}
Proto obsluga_normalnej_wiadomosci(Proto p1) {
	if (p1.ID != sesja)
		p1.ST = 555;
	else {
		polehistorii var;


		if (p1.OP == "dodaj") {
			var.wartosc = p1.WY = p1.P1 + p1.P2;
			p1.OO = var.id_obl = 1 + rand() % 2000000;
			Historia[0].push_back(var);

		}
		if (p1.OP == "odejmij") {
			var.wartosc = p1.WY = p1.P1 - p1.P2;
			p1.OO = var.id_obl = 1 + rand() % 2000000;
			Historia[1].push_back(var);

		}
		if (p1.OP == "logarytmuj") {
			var.wartosc = p1.WY = log10(p1.P2) / log10(p1.P1);
			p1.OO = var.id_obl = 1 + rand() % 2000000;
			Historia[2].push_back(var);

		}
		if (p1.OP == "poteguj") {
			var.wartosc = p1.WY = pow(p1.P1, p1.P2);
			p1.OO = var.id_obl = 1 + rand() % 2000000;
			Historia[3].push_back(var);

		}
		if (p1.OP == "nieznane") {
			int flag = 0;
			for (int i = 0; i < Historia.size(); ++i)
				for (int j = 0; j < Historia[i].size(); ++j)
					if (Historia[i][j].id_obl == p1.OO) {
						p1.WY = Historia[i][j].wartosc;	flag = 1; p1.ST = 777;
					}
			if (flag == 0)
				p1.ST = 444;
		}
		if (p1.WY == std::numeric_limits<double>::infinity())
			p1.ST = 999;

	}

	return p1;
}

void wyswietl_historia() {

	cout << "\***************HISTORIA    OBLICZEN**************/" << endl << endl;
	for (int i = 0; i < Historia.size(); ++i)
		for (int j = 0; j < Historia[i].size(); ++j)
			cout << ":OO: " << Historia[i][j].id_obl << " WY: " << Historia[i][j].wartosc << endl;


}
void wyslij_zwykla_wiadomosc(SOCKET newConnection, Proto p) {

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
	oss << "OP=" << p.OP << '$';
	//status
	oss << "ST=" << p.ST << '$';
	//sesja
	oss << "ID=" << sesja << '$';
	//OO

	oss << "OO=" << p.OO << '$';

	//P1

	oss << "P1=" << p.P1 << '$';

	//P2

	oss << "P2=" << p.P2 << '$';

	//WY

	oss << "WY=" << p.WY << '$';

	//RZ
	int RZ = -1;
	oss << "RZ=" << RZ << '$';

	//KO
	int KO = -1;
	oss << "KO=" << KO << '$';

	pakiet = oss.str();

	//	cout << oss.str();

	char tab2[1024];
	strncpy_s(tab2, pakiet.c_str(), sizeof(tab2));
	//tab2[pakiet.size()] = 0;
	send(newConnection, tab2, pakiet.size(), NULL);//send the chat message to this client

}

void wyslij_wiadomosc_sesji(SOCKET newConnection) {
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
	int st = 100;
	oss << "ST=" << st << '$';
	//sesja
	sesja = 1000 + rand() % 2000000;
	oss << "ID=" << sesja << '$';
	//OO
	//1+rand() % 2000000;
	oss << "OO=" << 0 << '$';

	//P1
	int P1 = 0;
	oss << "P1=" << P1 << '$';

	//P2
	int P2 = 0;
	oss << "P2=" << P2 << '$';

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

	//	cout << oss.str();

	char tab2[1024];
	strncpy_s(tab2, pakiet.c_str(), sizeof(tab2));
	//tab2[pakiet.size()] = 0;
	send(newConnection, tab2, pakiet.size(), NULL);//send the chat message to this client
}
int main()
{
	srand(time(NULL));
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

	addr.sin_port = htons(7777); //Port
	addr.sin_family = AF_INET; //IPv4 Socket


							   //SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //Create socket to listen for new connections
	SOCKET sListen;
	listen, sListen = INVALID_SOCKET;
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); //Bind the address to the socket
	listen(sListen, SOMAXCONN); //Places sListen socket in a state in which it is listening for an incoming connection. Note:SOMAXCONN = Socket Oustanding Max Connections

	SOCKET newConnection; //Socket to hold the client's connection
	int ConnectionCounter = 0; //# of client connections

	newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //Accept a new connection
	if (newConnection == 0) //If accepting the client connection failed
	{
		std::cout << "Failed to accept the client's connection." << std::endl;
	}
	else //If client connection properly accepted
	{

		wyslij_wiadomosc_sesji(newConnection);

		Proto p2 = Odbierz_potwierdzienie_wiadomosci_sesji(newConnection);
		if (p2.ST == 0)
			cout << "klient siê nie po³¹czy³" << endl;
		else cout << " " << "Client Connected!" << endl;

		int koniec = 1;

		while (koniec == 1)
		{

			//p2 = Odbierz_zwykla_wiadomosc(newConnection);

			p2 = Odbierz_wiadomosc(newConnection);
			if (p2.ST == 666)
			{
				shutdown(newConnection, 2);
				closesocket(newConnection);
				return 0;
			}
			p2 = obsluga_normalnej_wiadomosci(p2);
			wyslij_zwykla_wiadomosc(newConnection, p2);


			/*cout << "Historia obliczen?0/1" << endl;
			int s = 0;
			cin >> s;
			if (s == 1)*/
			wyswietl_historia();
		}
	}



	return 0;
}