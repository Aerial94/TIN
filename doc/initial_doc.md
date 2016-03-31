# TIN Projekt
## DNS Wrangler - cykliczny analizator dostępności domen
## Dokumentacja wstępna

#### Skład zespołu projektowego
- Radosław Załuska
- Konrad Czarnota
- Wiktor Kozakowski
- Aleksander Brodowicz

#### Treść zadania
> Celem zadania jest implementacja serwera, który cyklicznie będzie odpytywał
> systemy DNS dla dynamicznie zarządzanej listy domen. Odpytanie serwerów DNS
> będzie zrealizowane całkowicie iteracyjnie, tj. rozpoczynając od podanego
> "Root name server-a" poprzez wszystkie kolejne poziomy adresu domenowego.
> Zarządzanie listą domen oraz odbieranie wyników będzie zrealizowane z
> wykorzystaniem protokołu HTTP i notacji/składni JSON.

## Środowisko
Serwer będzie działał w środowsku systemu Linux. Do implementacji będzie użyty
język C++11. Obsługa wątków będzie zrealizowana za pomocą klasy std::thread.
Synchronizacja w dostępie do danych współdzielonych będzie odbywać się za pomocą
klasy std::mutex (obie klasy pochodzą ze standardu C++11).

##### Spis bibliotek pomocniczych
- [jsoncpp](https://github.com/open-source-parsers/jsoncpp) - parser json
- [Boost.Program\_options](http://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html) - obsługa parametrów
- [Catch](https://github.com/philsquared/Catch) - testowanie

### Budowanie
Projekt będzie budowany za pomocą narzędzia cmake zapewniającego sprawdzenie
dostępności wymaganych bibliotek w systemie i automatyczne tworzenie plików
Makefile w przypadku spełnienia zależności.

### Testowanie
Kluczowe elementy programu zostaną przetestowane za pomocą biblioteki
wspomagającej testy jednstkowe w C++ [Catch](https://github.com/philsquared/Catch)

## Zakładana funkcjonalność

## Architektura programu

### Lista podstawowych klas/obiektów


### Schemat działania
#### Zapytanie do serwera http
1. klient wysyła żądanie do serwera http. Umeszcza on zapytanie w kolejce do
   obsłużenia. Któryś z wątków obsługujących odbiera zapytanie i realizuje
   powierzone przez użytkownia zadania. Dostęp do kolejki zadań jest
   synchronizowany semaforem.
    ```
    Client *****> HttpServer ++> [o O o O o] ++> HttpHendler
    ```

2. wątek obsługujący odpytuje bazę danych domen w poszukiwaniu informacje o
   zadanej domenie.  Do klienta zostaje przesłana informacja o dostępności
   domeny lub jej braku.
    ```
    Client <***** HttpHendler <++ Database
    ```

#### Odpytanie serwerów DNS
```
DnsClient <++ Database - klient odczytuje z bazy danych domen informacje o
                         domenie którą ma w danej chwili zbadać.

RootDnsServer <---- DnsClient - klient zaczyna odpytywanie od jednego z root
                                serwerów
RootDnsServer ----> DnsClient
TopLevelDnsServer <---- DnsClient
TopLevelDnsServer ----> DnsClient
SecondLevelDnsServer <---- DnsClient
SecondLevelDnsServer ----> DnsClient
...
DomainDnsServer <---- DnsClient
DomainLevelDnsServer ----> DnsClient

DnsClient ++> Database
```

#### Legenda do oznaczeń
```
<***** - ruch sieciowy TCP
<----- - ruch sieciowy UDP
<+++++ - synchronizowany dostęp do zasobu współdzielonego przez wątki serwera
[o O o O o] - kolejka synchronizowana
```

## Sposób instalacji
### Budowanie ze źródeł
#### Ubuntu 14.04
```bash
sudo apt-get install cmake libjsoncpp git build-essential
git clone https://github.com/Aerial94/TIN
cd TIN
mkdir build
cmake ..
make
```

### Uruchomienie po zbudowaniu
```bash
cd TIN
cd build
./dns-wrangler [parametry]
```

## Zmiana parametrów programu
W programie będzie można skonfigurować następujące parametry:

- adres interfejsu sieciowego na którym będzie nasłuchiwał serwer http (lub *
oznaczająca wszystkie interfejsy INADDR\_ANY)
- port na którym będzie dostępna usługa http
- adres interfejsu sieciowego przez który będziemy wysyłać zapytania DNS
- domyślny interwał pomiędzy cyklicznymi odpytaniami serwerów DNS o domenę
- domyślny timeout na przysłanie danych od kilena do serwera http po rozpoczęciu
obsługi
- maksymalna ilość wątków HttpHendler obsługujących klientów serwera http
- maksymalna długość kolejki użytkowników oczekujących na obsługę przez jeden z
wątków HttpHendler

Parametry będą konfigurowalne poprzez plik cfg i parametry programu. Parametry
programu będą miały wyższy priorytet niż plik konfiguracyjny. Parsowanie
paramerów i pliku konfiguracyjnego zapewni biblioteka  Boost.Program\_options.

## Sytuacje wyjątkowe

- próba wykonania operacji bind dla uprzywilejowanego numeru portu bez uprawnień
użytkownika root
- brak dostępu do internetu dla interfejsu sieciowego wybranego do odpytywania
serwerów DNS
- nieprawidłowe zapytanie do serwera http (nieprawidłowe nagłówki http lub dane
w json
- przepełnienie kolejki podłączonych użytkowniów oczekujących na obsługę
