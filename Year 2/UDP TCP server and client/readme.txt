Nume: STIUCA Roxana-Elena
Grupa: 325CB
==========================================================================
						PROTOCOALE DE COMUNICATIE
			Tema 2 - Aplicatie client-server TCP si UDP pentru
						gestionarea mesajelor
==========================================================================
[1] DESCRIERE APLICATIE:

		In cadrul acestei teme, ofer o varianta de implementare a unei
	aplicatii client-server pentru gestionarea mesajelor. Serverul este
	legat prin socketi atat la clienti TCP, cat si la clienti UDP. El
	foloseste procedeul de multiplexare astfel incat sa poata raspunda
	cererilor primite de la un numar variabil de clienti si sa comunice.
	si cu utilizatorul uman (STDIN). Clientii TCP sunt legati prin socket
	la server si comunica atat cu serverul, cat si cu utilizatorul uman.
		Aplicatia este una de stiri, care administreaza subscriptiile
	unui client astfel incat acesta sa primeasca toate mesajele/stirile
	despre anumie topic-uri.
		Pentru implementare, am folosit structuri de mesaje (detaliate mai
	pe larg la punctele [3] si [4]), liste si cozi (toate se gasesc in
	fisierul lib.h).
==========================================================================
[2] DESCRIERE SERVER: {server.c}

		Serverul comunica cu utilizatorul uman (STDIN) si este conectat
	prin socketi la clienti UDP si clienti TCP.
--------------------------------------------------------------------------
	Comunicarea cu utilizatorul uman:

		Singura intructiune valida de la STDIN este: exit. Aceasta duce la
	oprirea executiei programului.
		Nicio alta comanda STDIN nu produce efecte in aplicatie, ci conduc
	doar la afisarea mesajului de eroare "Invalid command. Expected exit."
--------------------------------------------------------------------------
	Comunicarea cu clientii TCP:

		Serverul foloseste un socket TCP pasiv prin care se pot conecta
	clienti TCP. In momentul unei noi conexiuni, el asteapta ca urmatorul
	mesaj primit sa fie de la acelasi client, reprezentand un mesaj de
	tip TCP_CONNECT, cu continutul ID-ul noului client. In caz de succes,
	adauga noul client in vectorul dinamic de clienti, alaturi de
	informatiile sale (socket si ID) sau face update unui client vechi
	daca ID-ul exista deja si ii trimite mesajele in asteptare.

		Erorile tratate in cazul unei noi incercari de conexiune:
	 * Lipsa mesaj TCP_CONNECT sau format incorect => se trimite un mesaj
	 catre client, dupa care se intrerupe conexiunea cu acesta.
	 * Incercare de conectare cu un ID deja conectat => se trimite un
	 mesaj corespunzator catre client si se intrerupe conexiunea cu
	 acesta.

		Prin socketii activi TCP, clientii pot trimite mesaje de tipul
	TCP_SUBSCRIBE sau TCP_UNSUBSCRIBE, care duc la modificarea listelor
	subscriptiilor clientului. Serverul ii permite clientului sa se
	reaboneze la acelasi topic (fie ca parametrul SF este modificat sau
	nu), este permisiv in acest sens. Clientul poate da de oricate ori
	subscribe la un anume topic, serverul va tine in memorie doar
	parametrii ultimelei subscriptii la acel topic.

		Erorile tratate in acest caz:
	 * Clientul incearca sa se dezaboneze de la un topic la care nu este
	 abonat => se trimite un mesaj corespunzator clientului, iar lista
	 sa de subscriptii ramane aceeasi.
	 * Clientul trimite un mesaj cu format necunoscut => se trimite
	 un mesaj clientului si se ignora continutul mesajului.
--------------------------------------------------------------------------
	-> Comunicarea cu clientii UDP:

		Clientii UDP trimit pachete care respecta un anume format:
	=> 50 bytes (topic) - 1 byte (tip_date) - max 1500 bytes (continut) <=

		Serverul primeste mesaj UDP si il trimite mai departe clientilor
	abonati la acel topic sau il pune in coada pentru cei deconectati, dar
	care au ales optiunea SF (Store&Forward). Inainte de a il trimite mai
	departe, serverul adauga la finalul mesajului si IP-ul si portul
	clientului UDP.
==========================================================================
[3] PROTOCOL TCP:
	
		Clientii TCP vor primi de la server mesaje HELLO pentru conectare
	sau reconectare, mesaje de tip ERROR atunci cand mesajele trimise de
	ei la server erau eronate (ex.: incercare de dezabonare de la un
	topic la care nu sunt abonati) si mesaje ce contin stiri legate de
	topic-urile la care acestia sunt abonati.
--------------------------------------------------------------------------
	Descriere mesaje server -> client TCP:

		Structura se gaseste in lib.h sub denumirea msg_udp:
	==> topic - content_type - content - udp_ip - udp_port <==

	 * topic este un sir de maxim 10 caractere.
	 * content_type este un numar pe 1 byte, ce dicteaza felul in care
	 mesajul va fi tratat.
	 * content este un sir de maxim 1500 de caractere.
	 * udp_ip este un struct in_addr cu adresa IP a clientului UDP sursa.
	 * udp_port este un numar pe 2 bytes, reprezentand portul clientului
	 UDP sursa.

		Valori posibile pentru content_type:
	 * INT -> mesajul contine o stire, sub forma unui numar pe 4 bytes.
	 * SHORT_REAL -> mesajul contine o stire, sub forma unui numar real.
	 * FLOAT -> --//--.
	 * STRING -> mesajul contine o stire, sub forma unui sir de caractere.
	 * ERROR -> mesajul contine o eroare de la server, ce trebuie afisata
	 pentru utilizatorul uman.
	 * HELLO -> mesajul contine un text de conectare/reconectare, ce este
	 afisat utilizatorului uman.
--------------------------------------------------------------------------
	Descriere mesaje client TCP -> server:

		Structura se gaseste in lib.h sub denumirea msg_tcp:
	==> type - id - topic - SF <==

	 * type este tipul mesajului, ce dicteaza felul in care mesajul va fi
	 tratat.
	 * id este folosit in cazul unui mesaj de tip TCP_CONNECT si reprezinta
	 ID-ul clientului TCP ce vrea sa se conecteze la server.
	 * topic este folosit in cazul mesajelor de tip TCP_SUBSCRIBE si
	 TCP_UNSUBSCRIBE si reprezinta topic-ul la care clientul doreste sa
	 se aboneze/de la care doreste sa se dezaboneze.
	 * SF este folosit in cazul mesajul TCP_SUBSCRIBE si reprezinta
	 optiunea de Store&Forward pentru noua abonare.

		Valori posibile pentru type:
	 * TCP_CONNECT -> mesajul contine ID-ul clientului ce vrea sa se
	 conecteze la server;
	 * TCP_SUBSCRIBE -> mesajul contine topic-ul la care clientul vrea sa
	 se aboneze;
	 * TCP_UNSUBSCRIBE -> mesajul contine topic-ul de la care clientul
	 vrea sa se dezaboneze.
--------------------------------------------------------------------------
	Implementare client TCP: {subscriber.c}

		Se deschide conexiunea cu serverul si se adauga in multimea
	descriptorilor de citire socket-ul si 0 pentru STDIN. Se trimite
	un mesaj TCP_CONNECT cu ID-ul primit ca parametru. Dupa se asteapta
	repetat mesaj fie de la utilizatorul uman, fie de la server si se
	trateaza ca atare. 
==========================================================================
[4] PROTOCOL UDP:
	
		Serverul primeste mesaje prin socket-ul UDP cu formatul fix:
	=> 50 bytes (topic) - 1 byte (tip_date) - max 1500 bytes (continut) <=

		Serverul va citi datele primite prin acest socket intr-o
	structura msg_udp, populand insa doar primele 3 campuri ale acestei
	structuri:
	 * topic;
	 * content_type;
	 * content.

		De asemenea, campul content_type poate avea doar urmatoarele
	valori: INT, SHORT_REAL, FLOAT, STRING, care determina felul in care
	continutul mesajului va fi afisat.
==========================================================================
[5] DETALII DE IMPLEMENTARE:
		
		Clientii TCP sunt tinuti intr-un vector alocat dinamic, incapsulat
	in structura clients_array. Serverul tine in acest vector toti
	clientii ce s-au conectat, chiar daca acestia pot fi deconectati.

		Toate informatiile de care serverul are nevoie despre clientii
	TCP sunt tinute intr-o structura de tip client_info:
	 * id -> ID-ul unic al clientului;
	 * sockfd -> socket-ul prin care acest client este conectat daca
	 este conectat
	 * connected -> 1 daca este conectat, 0 daca nu;
	 * topics_SF -> lista simplu inlantuita cu topic-urile la care
	 clientul este abonat folosind optiunea SF (Store&Forward); pot fi
	 privite ca topic-urile cu prioritate, cele pt care se stocheaza
	 mesajele in coada atunci cand clientul este deconectat;
	 * topics -> lista simplu inlantuita cu topic-urile la care este
	 abonat fara sa fi folosit optiunea SF;
	 * msg_queue -> coada de asteptare pentru mesajele ce urmeaza sa fie
	 trimise la urmatoarea conectare a clientului.
	 	Functiile de prelucrare a vectorului de clienti se gasesc in
	utils.c.

	 	Coada de asteptare pentru mesajele ce trebuie trimise la
	reconectare este implementata cu o lista simplu inlantuita care
	respecta principiul FIFO (First In, First Out). Deci clientul va
	primi mesajele in ordine cronologica atunci cand se reconecteaza.
	Implementarea acestei cozi implica structurile list si queue din
	lib.h si metodele din queue.c.

		Topic-urile la care un client este abonat sunt tinute in cele 2
	liste inlanuite precizate mai sus, topics si topics_SF. Implementarea
	acestei liste consta in structura TList din lib.h si in functiile de
	prelucrare din topic_list.c.

		In afara erorile tratate precizate la [1], aplicatie verifica
	orice apel de sistem si foloseste DIE pe rezultat. Toate problemele de
	memorie sau de parametrii gresiti ai programului sunt tratate, prin
	apelul lui PROG_ERROR. Toate acestea duc la inchiderea fortata a
	aplicatiei.
==========================================================================
[6] OBSERVATII:

		Afisarile in server sunt cele specificate, plus un mesaj pentru
	comanda invalida primita de la STDIN:
	 * New client (CLIENT_ID) connected from IP:PORT.
	 * Client (CLIENT_ID) disconnected.
	 * Invalid command. Expected exit.

		In subscriber, pe langa afisarea mesajelor primite despre
	topic-uri, se mai afiseaza si alte mesaje primite de la server, cum
	ar fi mesaje HELLO sau ERROR.

		Programul nu are memory leak-uri.