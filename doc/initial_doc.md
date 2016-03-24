# TIN Projekt - dokumentacja wstępna

#### Skład zespołu
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
- [jsoncpp](https://github.com/open-source-parsers/jsoncpp)

## Zakładana funkcjonalność

## Architektura programu

### Lista podstawowych klas/obiektów
- asd


### Schemat działania
#### Zapytanie do serwera http
1. klient wysyła żądanie do serwera http. Zostaje dla niego utworzany nowy wątek
   (HttpHendler), który będze go obsługiwał.
    ```
    Client *****> HttpServer ++> HttpHendler
    ```

2. wątek obsługujący odpytuje bazę danych domen w poszukiwaniu informacje o
   zadanej domenie.  Do klienta zostaje przesłana informacja o dostępności
   domeny.
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
```

## Sposób instalacji
#### Ubuntu 14.04
```bash
sudo apt-get install cmake libjsoncpp git build-essential
git clone https://github.com/Aerial94/TIN
cd TIN
mkdir build
cmake ..
make
```

### Uruchomienie

## Zmiana parametrów programu

### Plik konfiguracyjny

## Sytuacje wyjątkowe
