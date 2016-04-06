# TIN Projekt
## Domain Checker - Cykliczny analizator dostępności domen
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
Serwer będzie działał w środowisku systemu Linux. Do implementacji będzie użyty
język C++11. Obsługa wątków będzie zrealizowana za pomocą klasy std::thread.
Synchronizacja w dostępie do danych współdzielonych będzie odbywać się za pomocą
klasy std::mutex (obie klasy pochodzą ze standardu C++11).

##### Spis bibliotek pomocniczych
- [jsoncpp](https://github.com/open-source-parsers/jsoncpp) - parser json
- [Catch](https://github.com/philsquared/Catch) - testowanie

### Budowanie
Projekt będzie budowany za pomocą narzędzia cmake zapewniającego sprawdzenie
dostępności wymaganych bibliotek w systemie i automatyczne tworzenie plików
Makefile w przypadku spełnienia zależności.

### Testowanie
Kluczowe elementy programu zostaną przetestowane za pomocą biblioteki
wspomagającej testy jednostkowe w C++ [Catch](https://github.com/philsquared/Catch).
Do testów integracyjnych zostanie użyty prosty klient http napisany w Pythonie.

## Zakładana funkcjonalność
- śledzenie dostępności wskazanych domen
- możliwość dodawania nowych domen do śledzenia za pomocą zatania HTTP
- możliwość pobierania statusu domen przez zapytania HTTP
- możliwość usuwania domen ze zbioru śledzonych za pomocą zapytania HTTP

## Architektura programu

### Lista podstawowych klas/obiektów
- TCPSocket
- UDPSocket
- HTTPServer
- HTTPHandler
- HTTPPacket
- Database
- DNSPacket
- DNSPooler
- Logger

\newpage
### Schemat działania

![](graph.pdf)
![](legend.pdf)

Dla każdego nowego połączenia uruchamiany jest wątek HTTPHandler obsługujący to
połączenie. Liczba wątków działających w danym momencie na rzecz klientów
serwera HTTP jest konfigurowalna w pliku cfg. Aplikacja reguluje możliwość
uruchomienia nowego wątku poprzez użycie semafora którego wartość jest ustawiona
początkowo na ilość wątków podaną w konfiguracji. Stworzenie nowego wątku
zmniejsza wartość semafora o 1, a zakończenie pracy wątku zwiększa tę wartość.

Synchronizacja poprzez mutex przy dostępie do bazy danych jest zrealizowana na
poziomie pojedynczej domeny. Gdy DNSPooler chce zapisać dane o domenie, blokuje
dostęp do odczytu.

### Przykłady zapytań JSON
#### Dodanie nowego serwera/serwerów
```js
{"command":"add", "domains": ["google.com", "elka.pw.edu.pl"]}
```
##### Odpowiedź
```js
{"task": {"command":"add", "domains": ["google.com", "elka.pw.edu.pl"]},
"result":
    [
        {"domain":"google.com", "status":"ok"},
        {"domain":"elka.pw.edu.pl", "staus": "ok"}
    ]
}
```
#### Dodanie usunięcie serwera/serwerów
```js
{"command":"remove", "domains": ["google.com", "elka.pw.edu.pl"]}
```
##### Odpowiedź
```js
{"task": {"command":"remove", "domains": ["google.com", "elka.pw.edu.pl"]},
"result":
    [
        {"domain":"google.com", "status":"ok"},
        {"domain":"elka.pw.edu.pl", "staus": "no_in_database"} //próba usunięcia
        //domeny która nie była na serwerze
    ]
}
```
#### Odpytanie o status serwerów
```js
{"command":"query", "domains": ["google.com", "elka.pw.edu.pl", "wp.pl", "github.com"]}
```
##### Odpowiedź
```js
{"task": {"command":"query", "domains": ["google.com", "elka.pw.edu.pl", "wp.pl"]},
"result":
    [
        {"domain":"google.com", "status":"ok"},  //domena jest znana na serwerze
        //i odpowiada na zapytania
        {"domain":"elka.pw.edu.pl", "staus": "no_in_database"} // domena jest
        //niezanana na serwerze
        {"domain":"wp.pl", "staus": "unreachable"} //domena jest znana na
        //serwerze ale nie odpowiada na zapytania
        {"domain":"github.com", "staus": "unknown"} //domena jest znana
        //na serwerze ale nie była jeszcze odpytywana lub konfiguracja siecowa
        //serwera uniemożliwia jej odpytanie
    ]
}
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
./domain-checker [parametry]
```

## Zmiana parametrów programu
W programie będzie można skonfigurować następujące parametry:

- adres interfejsu sieciowego na którym będzie nasłuchiwał serwer http (lub *
oznaczająca wszystkie interfejsy - INADDR\_ANY)
- port na którym będzie dostępna usługa http
- adres interfejsu sieciowego przez który będziemy wysyłać zapytania DNS
- domyślny interwał pomiędzy cyklicznymi odpytaniami serwerów DNS o domenę
- domyślny timeout na przysłanie danych od klienta do serwera http po rozpoczęciu
obsługi (aby nie zabierać zasobów przez "wiszące" połączenia)
- maksymalna ilość wątków HttpHandler obsługujących klientów serwera http

Program będzie konfigurowany poprzez plik cfg w formacie json. Podczas
uruchamiania plik ten będzie musiał być umieszczony w tym samym katalogu co
aplikacja.

### Przykładowy plik konfiguracyjny
```js
{
    "httpServer": {
        "address" : "127.0.0.1",
        "port" : "8080"
        "readTimeout" : "60", //timeout 1 minuta
        "maxThreads" : "10"
    }
    "dnsPooler" : {
        "address" : "127.0.0.1",
        "interval" : "600" //odpytanie co 10 minut
    }
}
```

## Sytuacje wyjątkowe

- próba wykonania operacji bind dla uprzywilejowanego numeru portu bez uprawnień
użytkownika root, program kończy działanie w wyniku tego błędu
- brak dostępu do internetu dla interfejsu sieciowego wybranego do odpytywania
serwerów DNS - następuje dodanie wpisu do logu, program kontynuuje swoje
działanie w oczekiwaniu na pojawienie się połączenia
- nieprawidłowe zapytanie do serwera http (nieprawidłowe nagłówki http lub dane
w json - sytuacja zostaje zalogowana, a nieprawidłowy pakiet odrzucony
- przepełnienie kolejki podłączonych użytkowników oczekujących na obsługę -
informacja o tym zostaje dodana do logów
- nieodnalezienie plik konfiguracyjnego w katalogu w którym jest program
- nieprawidłowy format pliku konfiguracyjnego - błąd składniowy lub plik
niekompletny - program kończy działanie wypisując komunikat o błędzie

## Format logów
```
DATA CZAS WAŻNOŚĆ_LOGU NAZWA_MODUŁU NAZWA_BŁĘDU OPIS
WAŻNOŚĆ_LOGU => INFO | DEBUG | WARNING
```
