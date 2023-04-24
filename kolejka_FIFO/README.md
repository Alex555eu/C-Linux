# Kolejka FIFO (łącza nazwane)

Wykorzystując potoki nazwane (fifo) należy zaimplementować zadanie typu klient-serwer z możliwością obsługi wielu klientów jednocześnie.

Projekt składa się z dwóch programów uruchamianych niezależnie: serwera i klienta:

• [s] serwer tworzy swoje FIFO, otwiera je w trybie tylko do odczytu i blokuje się;

• [s] pozostaje w tym stanie do momentu aż połączy się z nim klient, otwierając to samo FIFO do zapisu;

• [s] serwer odblokuje się i wykona funkcję sleep();

• [k] po otwarciu FIFO serwera każdy klient tworzy własne FIFO o unikatowej nazwie, przeznaczone
do odczytywania danych zwracanych przez serwer;

• [k] klient przesyła dane do serwera (blokując się jeżeli potok jest pełny albo serwer nadal uśpiony),
dane umieszczane np. w strukturze o składowych jak w przykładzie poniżej;

• [k] klient blokuje się na odczycie własnego potoku, oczekując na odpowiedź serwera;

• [s] po otrzymaniu danych od klienta serwer przetwarza je (zamienia wszystkie litery w wiadomości
na duże), otwiera FIFO klienta do zapisu (odblokowując w ten sposób klienta);

• [s] zapisuje przetworzone dane;

• [k] po odblokowaniu klient może odczytać ze swojego potoku dane zapisane przez serwer;

• cały proces powtarza się dopóki ostatni klient nie zamknie potoku serwera, wówczas funkcja read()
w serwerze zwróci 0;
