# Izveštaj o analizi projekta Mafija

## 1. Podaci o seminarskom radu

* **Autor:** Milica Mihailović
* **Broj indeksa:** 1023/2022
* **Predmet:** Verifikacija softvera
* **Analizirani projekat:** Mafija
* **Grana:** `main`
* **Commit:** `31565d605391de059c56128639cb4a7ba7111ae2`

## 2. Cilj rada

Cilj rada je primena različitih tehnika verifikacije softvera na projektu Mafija radi ispitivanja kvaliteta i ispravnosti implementacije. Analiza obuhvata statičku i dinamičku analizu koda, jedinično testiranje i analizu pokrivenosti testovima, fuzz testiranje i analizu kompleksnosti izvornog koda.

Korišćenjem više različitih tehnika cilj je da se ispitaju različiti aspekti projekta i identifikuju potencijalni problemi koji se ne mogu nužno uočiti primenom samo jednog pristupa. Posebna pažnja posvećena je pronalaženju grešaka u programskoj logici, potencijalnih problema u izvornom kodu, problema pri upravljanju memorijom, ponašanja programa za različite ulaze i složenosti pojedinih delova implementacije.

Pored pronalaženja potencijalnih problema, cilj rada je analiza i tumačenje dobijenih rezultata, kao i obezbeđivanje ponovljivosti sprovedenih analiza pomoću priloženih skripti i sačuvanih rezultata.


## 3. Opis analiziranog projekta

Mafija je studentski projekat razvijen u programskom jeziku C++ uz korišćenje Qt okvira, u okviru kursa Razvoj softvera na Matematičkom fakultetu, Univerzitetu u Beogradu. Projekat predstavlja implementaciju društvene igre Mafija namenjene za više igrača.

Na početku igre igračima se dodeljuju različite uloge, od kojih zavise njihove mogućnosti i akcije tokom igre. Igra se odvija kroz više faza. Tokom noći pojedini igrači mogu koristiti moći svojih uloga, na primer da provere ulogu drugog igrača, zaštite odnosno izleče nekog igrača ili, u slučaju mafije, odaberu igrača kojeg žele da eliminišu. Tokom dana igrači prikupljaju i razmenjuju informacije o drugim igračima i međusobno komuniciraju kako bi pokušali da utvrde ko pripada mafiji. Glasanje predstavlja posebnu fazu igre, tokom koje igrači glasaju za učesnika kojeg žele da izbace iz igre.

Aplikacija koristi klijentsko-serversku arhitekturu za komunikaciju između učesnika u igri. Projekat sadrži komponente zadužene za komunikaciju između klijenta i servera, zajedničku logiku i protokol razmene poruka, upravljanje stanjem i pravilima igre, kao i grafički korisnički interfejs implementiran korišćenjem Qt biblioteke.

Projekat koristi CMake sistem za izgradnju. Izvorni kod analizirane verzije projekta dodat je u repozitorijum ovog rada kao Git podmodul `Mafija`, vezan za tačno određeni commit, čime je omogućena reprodukcija analiza nad istom verzijom projekta.


## 4. Početna provera projekta

Pre primene alata za verifikaciju provereno je da se analizirana verzija projekta može uspešno konfigurisati, izgraditi i pokrenuti bez izmena izvornog koda. Projekat koristi CMake sistem za izgradnju.

Konfiguracija projekta izvršena je iz korenog direktorijuma projekta komandama:

```bash
cd Mafija
cmake -S . -B build
```

Nakon uspešne konfiguracije, projekat je izgrađen komandom:

```bash
cmake --build build
```

Izgradnja analizirane verzije projekta završena je uspešno. Grafička aplikacija zatim je pokrenuta pomoću generisanog izvršnog fajla:

```bash
./build/GUI/GUI
```

Provereno je da se aplikacija uspešno pokreće i da je moguće koristiti njen grafički korisnički interfejs.

## 5. Postojeći test program

Projekat Mafija sadrži postojeći skup testova napisan pomoću Catch2 okvira. Nakon izgradnje projekta, testovi su pokrenuti komandom:

```bash id="uxzj4m"
./build/tests/tests
```

Izvršavanje postojećeg skupa testova završeno je uspešno:

```text id="3mrj0m"
All tests passed (252 assertions in 40 test cases)
```

Dakle, svih 40 postojećih Catch2 test slučajeva, koji sadrže ukupno 252 provere, uspešno prolazi nad analiziranom verzijom projekta.

Analizom početnog skupa testova utvrđeno je koje su funkcionalnosti projekta već obuhvaćene testovima. Na osnovu te analize kasnije su odabrane funkcionalnosti i testni scenariji koji će biti dodatno ispitani pomoću QTest okvira.

Detaljniji pregled postojećih testova i izbor funkcionalnosti za koje su dodati novi QTest testovi dati su u narednim sekcijama.


## 6. Obim analize

Analiza projekta Mafija obuhvata više aspekata kvaliteta i ispravnosti softvera. Posmatrani su izvorni kod projekta, postojeći testovi, ponašanje programa tokom izvršavanja i obrada različitih ulaza.

U okviru rada obuhvaćene su sledeće oblasti:

* statička analiza izvornog koda radi pronalaženja potencijalnih grešaka i problematičnih konstrukcija;
* analiza postojećeg skupa testova i dodavanje novih jediničnih testova za odabrane funkcionalnosti koje nisu dovoljno obuhvaćene postojećim testovima;
* analiza pokrivenosti izvornog koda postojećim i novododatim testovima;
* dinamička analiza upravljanja memorijom tokom izvršavanja aplikacije;
* fuzz testiranje odabranih delova aplikacije različitim i neočekivanim ulazima;
* analiza kompleksnosti izvornog koda radi izdvajanja složenijih funkcija koje mogu biti teže za razumevanje, održavanje i testiranje.

Za svaku od navedenih oblasti izabran je odgovarajući alat ili tehnika. Način njihove primene, konfiguracija, dobijeni rezultati i uočeni problemi prikazani su u narednim poglavljima.

## 7. Izbor tehnika i alata

Za analizu projekta Mafija odabrane su sledeće tehnike i alati:

|  # | Alat / tehnika             | Namena                                           |
| -: | -------------------------- | ------------------------------------------------ |
|  1 | QTest + GCOV/LCOV | Jedinično testiranje i analiza pokrivenosti koda |
|  2 | clang-tidy                 | Statička analiza izvornog koda                   |
|  3 | Cppcheck                   | Statička analiza izvornog koda                   |
|  4 | Valgrind Memcheck          | Dinamička analiza upravljanja memorijom          |
|  5 | libFuzzer                  | Fuzz testiranje odabranih komponenti             |
|  6 | Lizard                     | Analiza kompleksnosti izvornog koda              |

Izabrani alati omogućavaju primenu različitih tehnika verifikacije i analizu različitih aspekata projekta. Način primene svakog alata, njegova konfiguracija i dobijeni rezultati detaljnije su prikazani u narednim poglavljima.

## 8. Okruženje

Analiza projekta Mafija sprovedena je u Linux okruženju. Osnovne karakteristike korišćenog okruženja su:

| Komponenta        | Verzija / okruženje         |
| ----------------- | --------------------------- |
| Operativni sistem | Ubuntu 26.04 LTS (Resolute) |
| CMake             | 4.2.3                       |
| GCC / G++         | 15.2.0                      |
| Clang / Clang++   | 21.1.8                      |
| Qt                | 6.10.2                      |
| QtTest            | 6.10.2                      |

Projekat i alati za njegovu analizu organizovani su tako da se direktorijum `Mafija`, koji predstavlja Git podmodul sa analiziranim izvornim kodom, nalazi unutar repozitorijuma ovog rada. Direktorijumi koji sadrže skripte i rezultate pojedinačnih analiza nalaze se pored njega.

Skripte korišćene za automatizaciju analiza pisane su za Bash okruženje. Za izgradnju projekta i delova projekta potrebnih za pojedinačne analize korišćen je CMake.


## 9. Rezultati pojedinačnih analiza

### 9.1 Jedinično testiranje i analiza pokrivenosti

U okviru analize projekta izvršeno je dodatno jedinično testiranje korišćenjem Qt Test (QTest) okvira. Pre pisanja novih testova analiziran je postojeći skup Catch2 testova kako bi se utvrdilo koje su funkcionalnosti projekta već testirane i izdvojili dodatni testni scenariji koji mogu otkriti ponašanja koja postojeći testovi ne proveravaju.

Novi QTest testovi smešteni su u direktorijum `unit_tests/qtest/` repozitorijuma za analizu, čime su odvojeni od izvornog koda i postojećih testova projekta Mafija. Postojeći Catch2 testovi nisu menjani.

Pored izvršavanja novih testova, analizirana je i pokrivenost izvornog koda testovima. Za merenje pokrivenosti korišćeni su GCOV i LCOV, pri čemu je analizirana zajednička pokrivenost ostvarena postojećim Catch2 testovima i novododatim QTest testovima.


#### Postojeći Catch2 testovi i testirane komponente

Pre dodavanja novih QTest testova analiziran je postojeći skup Catch2 testova projekta Mafija. Cilj ove analize bio je da se utvrdi koje su komponente i funkcionalnosti već obuhvaćene testiranjem, kako bi se prilikom pisanja novih testova izbeglo nepotrebno ponavljanje postojećih testnih scenarija.

Postojeći testni skup obuhvata sledeće komponente:

| Komponenta             | Testirane funkcionalnosti                                                                                       |
| ---------------------- | --------------------------------------------------------------------------------------------------------------- |
| `GameManager`          | Promene faza igre, obrada završetka noći i glasanja, eliminacija igrača i provera završetka igre                |
| `GameState`            | Pronalaženje i eliminacija igrača, određivanje živih igrača, upravljanje fazama igre i određivanje pobednika    |
| `NightAbilitiesServer` | Pokretanje noćne faze i emitovanje odgovarajućeg signala                                                        |
| `Player`               | Kreiranje igrača, promena uloge, stanje živ/mrtav i provera uloge igrača                                        |
| `PlayerIcon`           | Povezivanje grafičkog elementa sa igračem, prikaz boje uloge, eliminacija igrača i dimenzije grafičkog elementa |
| `VoteCounter`          | Dodavanje, promena i uklanjanje glasova, kao i određivanje rezultata glasanja i obrada izjednačenog rezultata   |
| `VoiceClient`          | Upravljanje `mute` i `deafen` stanjima glasovnog klijenta                                                       |
| `VoiceProtocol`        | Tipovi glasovnih poruka, audio parametri i formiranje poruka protokola                                          |
| `VoiceServer`          | Dodavanje i uklanjanje klijenata, upravljanje njihovim stanjima i rutiranje glasovnih podataka                  |

U postojećem skupu nalazi se i fajl `test_role.cpp`, ali on ne sadrži definisane Catch2 test slučajeve.

Analiza ovih testova poslužila je kao osnova za izbor dodatnih funkcionalnosti i graničnih slučajeva koji su zatim testirani pomoću QTest okvira.


#### Izbor i implementacija QTest testova

Na osnovu analize postojećih Catch2 testova odabrane su funkcionalnosti za koje su napisani dodatni testovi pomoću QTest okvira. Cilj nije bio ponovno implementiranje postojećih testnih slučajeva, već proširivanje testnog skupa funkcionalnostima, graničnim slučajevima i scenarijima koji prethodno nisu bili obuhvaćeni.

QTest testovi smešteni su u direktorijum `unit_tests/qtest/`, odvojeno od izvornog koda projekta Mafija i njegovih postojećih Catch2 testova. Postojeći testovi projekta nisu menjani.

Dodatnim testovima obuhvaćene su sledeće komponente:

| Komponenta     | Predmet dodatnog testiranja                                                                                                       |
| -------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| `GameState`    | Stanje igrača, dozvoljene akcije i komunikacija u različitim fazama igre, glasanje, validnost ciljeva i informacije o stanju igre |
| `Protocol`     | Konverzija tipova poruka i uloga i formiranje različitih poruka protokola                                                         |
| `VoteCounter`  | Dodatni slučajevi brojanja glasova, posebno ponašanje vezano za `voting_for_nobody`                                               |
| `VotingRound`  | Tok pojedinačne runde glasanja, prihvatanje glasova i završetak runde                                                             |
| `VotingServer` | Pokretanje i završetak faze glasanja, obrada rezultata i ponašanje u slučaju izjednačenja                                         |
| `ChatServer`   | Obrada prijavljivanja korisnika, pokretanje igre, komunikacija, glasanje, noćne akcije i odabrani neispravni zahtevi              |
| `ChatClient`   | Obrada primljenih serverskih poruka, emitovanje odgovarajućih signala i formiranje poruka koje klijent šalje serveru              |

Pri implementaciji testova korišćeno je više mehanizama Qt i QTest okvira. Makroi `QVERIFY` i `QCOMPARE` korišćeni su za proveru uslova i poređenje dobijenih rezultata sa očekivanim vrednostima.

Za proveru Qt signala korišćen je `QSignalSpy`. On omogućava praćenje određenog signala i proveru da li je signal emitovan, koliko puta je emitovan i koje su vrednosti prosleđene kao njegovi argumenti. Ovo je posebno značajno kod komponenti kod kojih rezultat neke operacije nije predstavljen samo povratnom vrednošću funkcije, već se o promeni obaveštavaju druge komponente emitovanjem Qt signala.

Pošto deo aplikacije koristi asinhronu komunikaciju, u testovima su korišćeni i mehanizmi kao što su `QTRY_COMPARE_WITH_TIMEOUT`, `QTRY_VERIFY_WITH_TIMEOUT`, `QTest::qWait` i obrada Qt događaja. Oni omogućavaju da test sačeka da se očekivani asinhroni događaj dogodi, umesto da rezultat proverava neposredno nakon pokretanja operacije.

Za testiranje klijentsko-serverske komunikacije korišćeni su `QTcpSocket` i `QTcpServer`. U testovima klase `ChatClient` lokalni `QTcpServer` simulira serversku stranu komunikacije, dok se u testovima klase `ChatServer` pomoću `QTcpSocket` objekata simuliraju stvarni klijenti. Na taj način testovi mogu da uspostave lokalnu TCP vezu, pošalju poruke u formatu koji koristi aplikacija i provere odgovor druge strane bez ručnog pokretanja kompletne aplikacije.

Za formiranje i obradu mrežnih poruka korišćene su i Qt klase `QJsonObject`, `QJsonDocument` i `QDataStream`, u skladu sa načinom na koji sama aplikacija razmenjuje podatke između klijenta i servera.

Posebna pažnja posvećena je mehanizmu glasanja i vrednosti `voting_for_nobody`. Ova vrednost u implementaciji glasanja ima posebno značenje i koristi se za predstavljanje slučaja kada se ne glasa ni za jednog igrača. Međutim, `voting_for_nobody` nije rezervisana niti zabranjena kao korisničko ime, pa stvarni igrač može da se prijavi koristeći upravo to ime.

Time nastaje moguć konflikt između specijalne vrednosti koju koristi implementacija i validnog korisničkog imena. Postojeći `VoteCounter` testovi proveravali su uobičajene slučajeve glasanja i izjednačenja, ali nisu proveravali ponašanje sistema kada stvarni igrač ima korisničko ime `voting_for_nobody`.

Zbog toga su dodati testovi koji proveravaju da igrač sa tim korisničkim imenom može normalno da glasa, da njegov glas bude registrovan i da može da se pojavi kao rezultat glasanja kada dobije najveći broj glasova ili kada je izjednačen sa drugim kandidatom. Ovi slučajevi su najpre provereni direktno na klasi `VoteCounter`, a zatim i kroz `VotingRound`, kako bi se ispitalo da li se isto ponašanje pravilno prenosi kroz viši nivo implementacije mehanizma glasanja.

Na ovaj način novi QTest testovi predstavljaju dopunu postojećeg Catch2 testnog skupa i omogućavaju proveru većeg broja funkcionalnih, graničnih i neispravnih ulaznih scenarija.


#### Pokretanje QTest testova

Za automatizovanu izgradnju i pokretanje dodatih QTest testova napravljena je posebna Bash skripta koja se nalazi u direktorijumu `unit_tests/`. Skripta pri svakom pokretanju uklanja prethodni build direktorijum, ponovo konfiguriše i izgrađuje testove i zatim pokreće sve definisane QTest testove.

Testovi se mogu pokrenuti iz direktorijuma `unit_tests` izvršavanjem skripte:

```bash
./run_qtests.sh
```

Skripta najpre kreira čist build direktorijum i konfiguriše QTest projekat pomoću CMake-a. Nakon uspešne izgradnje testovi se pokreću pomoću CTest-a.

Izlaz testova istovremeno se prikazuje na standardnom izlazu i čuva u fajlu:

```text
unit_tests/qtest_results.txt
```

Na ovaj način rezultat izvršavanja ostaje sačuvan i može se koristiti za kasniju analizu i reprodukciju dobijenih rezultata.

Prilikom pokretanja izvršavaju se testovi za komponente `GameState`, `Protocol`, `VoteCounter`, `VotingRound`, `VotingServer`, `ChatServer` i `ChatClient`. Pojedinačni testovi proveravaju očekivano ponašanje komponenti, dok su određeni testni scenariji namerno formulisani tako da mogu ukazati na grešku u postojećoj implementaciji ukoliko se dobijeno ponašanje razlikuje od očekivanog.


#### Rezultati QTest testova

Izgradnja svih dodatih QTest testova završena je uspešno, nakon čega je pomoću CTest-a pokrenuto sedam test programa: `qtest_votecounter`, `qtest_votinground`, `qtest_chatserver`, `qtest_protocol`, `qtest_votingserver`, `qtest_chatclient` i `qtest_gamestate`.

Od sedam pokrenutih test programa, četiri su završena bez neuspešnih testnih slučajeva, dok su tri označena kao neuspešna:

| Test program         | Rezultat                |
| -------------------- | ----------------------- |
| `qtest_votecounter`  | 5 prošlo, 3 neuspešna   |
| `qtest_votinground`  | 5 prošlo, 1 neuspešan   |
| `qtest_chatserver`   | 19 prošlo, 5 neuspešnih |
| `qtest_protocol`     | 62 prošlo, 0 neuspešnih |
| `qtest_votingserver` | 9 prošlo, 0 neuspešnih  |
| `qtest_chatclient`   | 43 prošlo, 0 neuspešnih |
| `qtest_gamestate`    | 50 prošlo, 0 neuspešnih |

CTest je zbog toga prijavio da su tri od sedam test programa neuspešna. Neuspeh ovih test programa nije posledica problema prilikom izgradnje testova, već pojedinačnih testnih slučajeva kod kojih se stvarno ponašanje analizirane implementacije razlikuje od očekivanog ponašanja definisanog testom.

Neuspešni testovi pronađeni su u komponentama `VoteCounter`, `VotingRound` i `ChatServer`. U `VoteCounter` testovima neuspešna su tri slučaja povezana sa vrednošću `voting_for_nobody`. Kada igrač sa ovim korisničkim imenom dobije najveći broj glasova, očekivan je rezultat sa jednim igračem, dok implementacija vraća prazan rezultat. Isto ponašanje javlja se kada je `voting_for_nobody` izjednačen sa drugim kandidatom, kao i kada igrač sa ovim imenom istovremeno učestvuje kao glasač i kandidat.

Problem je potvrđen i na višem nivou kroz `VotingRound`. Test `playerNamedVotingForNobodyCanBeEliminated()` očekuje da se igrač `voting_for_nobody`, nakon dobijanja najvećeg broja glasova, nalazi u rezultatu glasanja. Dobijeni rezultat je, međutim, prazan.

Pored problema vezanog za glasanje, testovima klase `ChatServer` otkriveno je još nekoliko slučajeva u kojima se ponašanje implementacije razlikuje od očekivanog. Neuspešni testovi odnose se na obradu dupliranog igrača pri pokretanju igre, neslaganje broja uloga i povezanih igrača, mogućnost da klijent pošalje noćnu akciju u ime drugog igrača, mogućnost glasanja u ime drugog igrača i broj poruka sa listama čekaonice koje se šalju nakon izbora igrača.

Dobijeni rezultati pokazuju da većina dodatih testnih scenarija prolazi, ali su novi testovi istovremeno otkrili nekoliko ponašanja koja zahtevaju detaljniju analizu. Uočeni problemi razmatrani su pojedinačno kako bi se utvrdio njihov uzrok i mogući uticaj na rad aplikacije.


#### Analiza pokrivenosti koda

Pored izvršavanja jediničnih testova analizirana je i pokrivenost izvornog koda projekta Mafija testovima. Cilj ove analize bio je da se utvrdi u kojoj meri postojeći i novododati testovi obuhvataju izvorni kod projekta i da se identifikuju delovi implementacije koji tokom testiranja nisu izvršeni.

Za merenje pokrivenosti korišćeni su alati GCOV i LCOV. GCOV prikuplja podatke o izvršavanju instrumentiranog programa, dok je LCOV korišćen za obradu i objedinjavanje prikupljenih podataka i generisanje preglednog HTML izveštaja.

Analizirana je zajednička pokrivenost ostvarena postojećim Catch2 testovima projekta Mafija i novododatim QTest testovima. Najpre su izgrađeni i pokrenuti postojeći Catch2 testovi, nakon čega su prikupljeni podaci o pokrivenosti ostvareni njihovim izvršavanjem. Zatim su izgrađeni i pokrenuti QTest testovi i na isti način su prikupljeni podaci o njihovoj pokrivenosti. Dobijeni rezultati su nakon toga objedinjeni pomoću LCOV-a.

Za automatizaciju celog postupka napravljena je posebna Bash skripta koja se nalazi u direktorijumu `unit_tests/`. Projekat se prilikom analize izgrađuje sa opcijama `--coverage`, `-O0` i `-g`, čime se omogućava prikupljanje podataka o izvršenim delovima izvornog koda.

Neuspešni QTest testovi ne prekidaju postupak prikupljanja pokrivenosti. Pojedini dodatni testovi napisani su sa ciljem da otkriju potencijalne probleme u postojećoj implementaciji, pa njihov neuspeh predstavlja rezultat testiranja koji je potrebno analizirati. Kod izvršen tokom takvog testa i dalje se uzima u obzir prilikom određivanja pokrivenosti.

Nakon objedinjavanja rezultata iz konačnog izveštaja uklanjaju se fajlovi koji nisu predmet analize, kao što su sistemske i Qt biblioteke, Catch2 biblioteka, generisani fajlovi i sami testni fajlovi. Na taj način konačni rezultat predstavlja pokrivenost relevantnog izvornog koda projekta Mafija.

Analiza pokrivenosti može se reprodukovati iz direktorijuma `unit_tests` pokretanjem:

```bash
./run_tests_coverage.sh
```

Tokom izvršavanja skripte posebno se čuvaju podaci o pokrivenosti ostvareni Catch2 i QTest testovima, a zatim se formira njihov objedinjeni i filtrirani LCOV izveštaj.

Na kraju se generiše HTML izveštaj `index.html` koji se nalazi u direktorijumu:

```text
unit_tests/coverage-report/html/
```

HTML izveštaj omogućava pregled pokrivenosti po direktorijumima i izvornim fajlovima, kao i pregled pojedinačnih linija koda koje jesu ili nisu izvršene tokom testiranja. Na taj način moguće je preciznije utvrditi koji delovi projekta imaju dobru pokrivenost testovima, a za koje delove bi bilo korisno dodati nove testne slučajeve.

#### Rezultati analize pokrivenosti

Zajedničkim izvršavanjem postojećih Catch2 i novododatih QTest testova ostvarena je pokrivenost od **77,3% linija**, odnosno izvršeno je 1280 od ukupno 1656 analiziranih linija koda. Pokrivenost funkcija iznosi **80,7%**, pri čemu je izvršeno 180 od ukupno 223 funkcije. Pokrivenost grana je niža i iznosi **49,8%**, odnosno obuhvaćeno je 1518 od ukupno 3046 grana.

| Metrika  | Pokrivenost | Pokriveno | Ukupno |
| -------- | ----------: | --------: | -----: |
| Linije   |       77,3% |      1280 |   1656 |
| Funkcije |       80,7% |       180 |    223 |
| Grane    |       49,8% |      1518 |   3046 |

Pokrivenost se razlikuje između pojedinih delova projekta. Najveća pokrivenost linija ostvarena je u direktorijumu `game`, gde iznosi **86,1%**. Direktorijumi `server_client/Server` i `server_client/Shared` takođe imaju visoku pokrivenost linija, od **83,3%** i **83,0%**, dok pokrivenost direktorijuma `server_client/Client` iznosi **72,7%**. Najniža pokrivenost zabeležena je za `GUI/graphics_items`, gde je izvršeno **35,8%** analiziranih linija.

| Deo projekta           | Linije | Funkcije | Grane |
| ---------------------- | -----: | -------: | ----: |
| `game`                 |  86,1% |    88,9% | 63,7% |
| `server_client/Server` |  83,3% |    79,5% | 49,4% |
| `server_client/Shared` |  83,0% |    84,5% | 53,1% |
| `server_client/Client` |  72,7% |    73,6% | 48,1% |
| `GUI/graphics_items`   |  35,8% |    50,0% | 15,7% |

Dobijeni rezultati pokazuju da testovi dobro obuhvataju značajan deo logike igre i klijentsko-serverske komunikacije, dok je pokrivenost grafičkih komponenti znatno niža. Takođe, pokrivenost grana je u svim analiziranim delovima projekta niža od pokrivenosti linija i funkcija, što ukazuje da postoje različite putanje kroz uslovne konstrukcije koje nisu izvršene tokom testiranja.

Detaljniji pregled pojedinačnih fajlova pokazuje da je, na primer, za `GameState.cpp` ostvarena pokrivenost od **95,0% linija** i **100,0% funkcija**, dok je pokrivenost grana **77,2%**. Sa druge strane, pojedine komponente imaju znatno nižu pokrivenost. Fajl `nightabilitiesserver.cpp` ima **24,0%** pokrivenosti linija, dok `nightaction.cpp` ima **0,0%** pokrivenosti linija i funkcija.

Rezultati pokrivenosti na taj način ne pokazuju samo ukupan procenat testiranog koda, već omogućavaju i izdvajanje komponenti kojima bi prilikom daljeg razvoja testnog skupa trebalo posvetiti dodatnu pažnju.



#### Uočeni problemi

Izvršavanjem dodatih QTest testova uočeno je više slučajeva u kojima se ponašanje postojeće implementacije razlikuje od očekivanog. Neuspešni testovi ukazuju na probleme u mehanizmu glasanja, validaciji podataka prilikom pokretanja igre, proveri identiteta igrača prilikom obrade zahteva i slanju poruka klijentima.

##### Konflikt između korisničkog imena `voting_for_nobody` i specijalne vrednosti za glasanje

U implementaciji glasanja vrednost `voting_for_nobody` ima posebno značenje i koristi se za predstavljanje slučaja kada igrač ne želi da glasa ni za jednog kandidata. Međutim, serverska strana ne zabranjuje korišćenje ove vrednosti kao korisničkog imena. Test `votingForNobodyUsernameIsAccepted()` potvrđuje da se klijent sa korisničkim imenom `voting_for_nobody` može uspešno prijaviti.

Problem nastaje kada takav igrač postane kandidat za eliminaciju. U testu `votedNoneStrictlyWins()` igrač `voting_for_nobody` dobija najveći broj glasova i očekuje se da rezultat sadrži upravo tog igrača. Umesto jednog rezultata, implementacija vraća praznu listu. Slično ponašanje javlja se u testu `votedNoneTiedWithPlayer()`, gde je ovaj igrač izjednačen sa drugim kandidatom, ali se umesto oba kandidata ponovo dobija prazan rezultat.

Isti problem potvrđen je testom `votingForNobodyAsPlayerAndVoteTarget()`, kao i na višem nivou implementacije testom `playerNamedVotingForNobodyCanBeEliminated()` klase `VotingRound`. Time je pokazano da problem nije ograničen samo na izolovano brojanje glasova, već utiče i na rezultat kompletne runde glasanja.

Uzrok problema je korišćenje vrednosti koja može predstavljati validno korisničko ime istovremeno kao specijalne vrednosti sa posebnim značenjem u programskoj logici. Problem bi se mogao izbeći rezervisanjem ovog korisničkog imena ili, pogodnije, razdvajanjem informacije o glasanju za „nikoga“ od običnog korisničkog imena kandidata.

##### Nedovoljna validacija podataka prilikom pokretanja igre

Dva neuspešna testa klase `ChatServer` odnose se na podatke koji se koriste prilikom pokretanja igre.

Test `duplicatePlayerInStartGameIsRejected()` proverava slučaj u kojem zahtev za pokretanje igre sadrži duplirano ime igrača. Dobijeno stanje sadrži pet živih igrača umesto šest očekivanih, što pokazuje da duplikat ne dovodi do pravilnog odbacivanja celog zahteva, već se obrada nastavlja i formira nekonzistentno početno stanje igre.

Test `gameDoesNotStartWhenRoleCountDiffersFromConnectedPlayers()` proverava slučaj u kojem se broj prosleđenih uloga ne poklapa sa brojem povezanih igrača. Implementacija u ovom slučaju prepoznaje neslaganje i ne formira igrače u `GameState` objektu, dok test očekuje da prethodno stanje povezanih igrača ostane očuvano. Dobijeni rezultat pokazuje da je potrebno pažljivije definisati i proveriti ponašanje servera kada zahtev za pokretanje igre sadrži nekonzistentne podatke.

##### Mogućnost slanja akcije u ime drugog igrača

Testovima `clientCanUseNightAbilityAsAnotherPlayer()` i `clientCanVoteAsAnotherPlayer()` uočeno je da server prihvata identitet igrača naveden u sadržaju primljene poruke bez dovoljne provere da li taj identitet odgovara klijentu koji je poruku zaista poslao.

U prvom slučaju povezani klijent šalje zahtev za korišćenje noćne moći navodeći ime drugog igrača. Očekivano je da server takvu akciju odbaci, ali je odgovarajući signal ipak emitovan. U drugom slučaju klijent na isti način šalje glas u ime drugog igrača, a server glas prihvata i emituje signal za glasanje.

Ovakvo ponašanje predstavlja značajan problem u aplikaciji za više igrača, jer klijent ne bi trebalo da može da izvršava akcije predstavljajući se kao drugi učesnik igre. Server bi prilikom obrade zahteva trebalo da poveže akciju sa identitetom klijenta koji ju je poslao i proveri da li se taj identitet poklapa sa igračem u čije ime se akcija izvršava.

##### Višestruko slanje liste čekaonice

Test `playerSelectionSendsOnlyOneWaitingRoomList()` pokazao je da se nakon izbora igrača šalju dve poruke sa podacima o čekaonici, iako je očekivano slanje jedne poruke.

U testu je izbrojano dve poruke tipa waiting-room umesto očekivane jedne. Ovo ukazuje na to da se ista informacija tokom obrade jednog zahteva šalje više puta. Iako ovaj problem nema isti uticaj na ispravnost igre kao prethodno navedeni problemi, nepotrebno dupliranje mrežnih poruka može dovesti do redundantne obrade na klijentskoj strani i nepotrebne mrežne komunikacije.

Dodati QTest testovi su na ovaj način, pored proširivanja pokrivenosti postojećeg testnog skupa, omogućili identifikovanje konkretnih graničnih i neispravnih scenarija koji nisu bili obuhvaćeni postojećim Catch2 testovima.

#### Ograničenja

Sprovedeno jedinično testiranje ne predstavlja iscrpnu proveru svih mogućih ponašanja projekta Mafija. Novi QTest testovi usmereni su na odabrane komponente, granične slučajeve i funkcionalnosti koje nisu bile dovoljno obuhvaćene postojećim Catch2 testovima. Zbog toga uspešno izvršavanje određenog testa ne garantuje odsustvo grešaka u drugim scenarijima korišćenja iste komponente.

Testiranje klijentsko-serverske komunikacije izvršeno je lokalno, pri čemu su pomoću `QTcpSocket` i `QTcpServer` objekata simulirani učesnici komunikacije. Ovakvi testovi omogućavaju proveru stvarne obrade mrežnih poruka, ali ne obuhvataju sve probleme koji se mogu javiti u realnom mrežnom okruženju, kao što su kašnjenja, prekidi veze i istovremena komunikacija većeg broja klijenata.

Analiza pokrivenosti obuhvata samo kod koji je izvršen tokom postojećih Catch2 i novododatih QTest testova. Visoka pokrivenost linija ili funkcija sama po sebi ne predstavlja dokaz ispravnosti programa, jer izvršavanje određene linije ne znači nužno da su proverena sva moguća stanja i svi mogući rezultati njenog izvršavanja. Ovo je posebno vidljivo kod pokrivenosti grana, koja je u sprovedenoj analizi niža od pokrivenosti linija i funkcija.

Pokrivenost pojedinih delova projekta značajno se razlikuje. Grafičke komponente imaju nižu pokrivenost od većine komponenti koje implementiraju logiku igre i komunikaciju, dok pojedini delovi implementacije noćnih akcija takođe ostaju slabo ili potpuno nepokriveni testovima. Zbog toga dobijeni ukupni procenat pokrivenosti ne treba posmatrati kao ravnomernu pokrivenost celog projekta.

Konačno, neuspešan test ukazuje na razliku između očekivanog i stvarnog ponašanja, ali ne mora sam po sebi predstavljati grešku u implementaciji. Za svaki takav slučaj potrebno je proveriti da li je očekivanje definisano testom u skladu sa predviđenim ponašanjem aplikacije. Zbog toga su neuspešni testovi dodatno analizirani pre izvođenja zaključaka o pronađenim problemima.


### 9.2. clang-tidy

#### 9.2.1. Konfiguracija i pokretanje

#### 9.2.2. Rezultati

#### 9.2.3. Analiza značajnih nalaza

#### 9.2.4. Ograničenja

### 9.3. Cppcheck

#### 9.3.1. Konfiguracija i pokretanje

#### 9.3.2. Rezultati

#### 9.3.3. Analiza značajnih nalaza

#### 9.3.4. Ograničenja

### 9.4. Valgrind Memcheck

#### 9.4.1. Konfiguracija i pokretanje

#### 9.4.2. Rezultati

#### 9.4.3. Analiza značajnih nalaza

#### 9.4.4. Ograničenja

### 9.5. libFuzzer

#### 9.5.1. Konfiguracija i pokretanje

#### 9.5.2. Fuzz targeti i korpusi

#### 9.5.3. Rezultati

#### 9.5.4. Analiza nalaza

#### 9.5.5. Ograničenja

### 9.6. Lizard

#### 9.6.1. Konfiguracija i pokretanje

#### 9.6.2. Rezultati analize kompleksnosti

#### 9.6.3. Analiza najsloženijih funkcija

#### 9.6.4. Ograničenja

## 10. Zaključak
