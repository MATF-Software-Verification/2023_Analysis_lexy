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


### 9.2 Statička analiza pomoću clang-tidy

Za statičku analizu izvornog koda projekta Mafija korišćen je alat `clang-tidy`. Ovaj alat analizira C++ izvorni kod bez potrebe za izvršavanjem programa i omogućava pronalaženje potencijalnih grešaka, problematičnih programskih konstrukcija, problema koji mogu uticati na performanse, kao i mesta na kojima se mogu primeniti savremenije C++ konstrukcije.

Analiza je sprovedena nad produkcionim izvornim kodom projekta Mafija, dok postojeći testovi i generisani fajlovi nisu uključeni u analizu. Za pravilno određivanje opcija kompajliranja, putanja do zaglavlja i drugih informacija potrebnih za analizu korišćena je CMake baza komandi za kompajliranje `compile_commands.json`.

Za analizu su uključene sledeće grupe `clang-tidy` provera:

| Grupa provera   | Namena                                                                            |
| --------------- | --------------------------------------------------------------------------------- |
| `bugprone-*`    | Pronalaženje konstrukcija koje mogu dovesti do grešaka ili neočekivanog ponašanja |
| `performance-*` | Pronalaženje konstrukcija koje mogu nepotrebno uticati na performanse             |
| `modernize-*`   | Predlozi za korišćenje savremenijih C++ konstrukcija                              |
| `readability-*` | Pronalaženje konstrukcija koje otežavaju čitljivost i održavanje koda             |

Za automatizaciju analize napravljena je Bash skripta smeštena u direktorijumu `clang-tidy/`. Skripta proverava dostupnost potrebnih alata, priprema CMake build direktorijum i generiše `compile_commands.json`, nakon čega pokreće `clang-tidy` nad odabranim izvornim fajlovima projekta.

Rezultat analize istovremeno se prikazuje na standardnom izlazu i čuva u fajlu:

```text
clang-tidy/clang-tidy-report.txt
```

Na taj način kompletan izlaz alata ostaje sačuvan i može se naknadno analizirati, kao i koristiti za reprodukciju rezultata.

#### Konfiguracija i pokretanje analize

Za automatizaciju statičke analize napravljena je Bash skripta `clang-tidy.sh`, smeštena u direktorijumu `clang-tidy/`. Skripta pri svakom pokretanju uklanja prethodni build direktorijum i prethodno generisani izveštaj, nakon čega ponovo konfiguriše i izgrađuje projekat.

Tokom CMake konfiguracije uključena je opcija:

```text id="3aehpu"
CMAKE_EXPORT_COMPILE_COMMANDS=ON
```

kojom se generiše baza `compile_commands.json`. Ovaj fajl sadrži komande i opcije korišćene za kompajliranje pojedinačnih izvornih fajlova i omogućava alatu `clang-tidy` da ih analizira u odgovarajućem kontekstu projekta.

Pre pokretanja analize `compile_commands.json` se dodatno filtrira. U analizu su uključeni produkcioni `.cpp`, `.cc` i `.cxx` fajlovi iz direktorijuma:

```text id="ag5mkd"
game/
GUI/
server_client/
```

dok su testovi, build direktorijum i Qt/CMake generisani fajlovi, uključujući `_autogen` fajlove, isključeni iz analize. Nakon filtriranja u bazi je ostao ukupno **31 izvorni fajl** nad kojim je pokrenut `clang-tidy`.

Analiza je izvršena sa sledećim grupama provera:

```text id="5ctss8"
bugprone-*
performance-*
modernize-*
readability-*
```

Grupa `bugprone-*` korišćena je za pronalaženje konstrukcija koje mogu dovesti do grešaka ili neočekivanog ponašanja, `performance-*` za potencijalno neefikasne konstrukcije, `modernize-*` za predloge vezane za savremeniji C++, a `readability-*` za probleme koji utiču na čitljivost i održavanje koda.

Analiza se može reprodukovati iz direktorijuma `clang-tidy` pokretanjem:

```bash id="1l9e9w"
./clang-tidy.sh
```

Kompletan izlaz analize čuva se u fajlu:

```text id="dl0hsw"
clang-tidy/clang-tidy-report.txt
```

Veliki broj upozorenja koje `clang-tidy` navodi kao generisane tokom obrade potiče iz koda biblioteka i drugih delova koji nisu predmet analize. Takva upozorenja su potisnuta, dok se u izveštaju prikazuju nalazi koji se odnose na analizirani kod projekta Mafija.

#### Rezultati clang-tidy analize

Nakon filtriranja baze `compile_commands.json`, alat `clang-tidy` pokrenut je nad ukupno **31 izvornim fajlom** projekta Mafija.

Analiza je pronašla veći broj upozorenja iz uključenih grupa `bugprone`, `performance`, `modernize` i `readability`. Najveći deo prijavljenih nalaza odnosi se na preporuke za modernizaciju C++ koda i poboljšanje njegove čitljivosti, dok su prijavljeni i nalazi koji ukazuju na potencijalno neefikasne ili problematične konstrukcije.

Među često prijavljivanim nalazima nalaze se:

* korišćenje klasičnog oblika povratnog tipa umesto trailing return type sintakse (`modernize-use-trailing-return-type`);
* izostavljanje vitičastih zagrada oko tela uslovnih naredbi i petlji (`readability-braces-around-statements`);
* implicitne konverzije pokazivača i numeričkih vrednosti u `bool` (`readability-implicit-bool-conversion`);
* kratka imena promenljivih (`readability-identifier-length`);
* prosleđivanje objekata po vrednosti kada bi se moglo izbeći nepotrebno kopiranje (`performance-unnecessary-value-param`);
* korišćenje numeričkih konstanti direktno u kodu (`readability-magic-numbers`);
* sužavajuće konverzije numeričkih tipova (`bugprone-narrowing-conversions`);
* ponavljanje identičnih grana upravljačkih konstrukcija (`bugprone-branch-clone`).

Na primer, u fajlu `GameState.cpp` alat je prijavio više slučajeva prosleđivanja parametara po vrednosti koji mogu dovesti do nepotrebnog kopiranja, kao i više nalaza vezanih za čitljivost i modernizaciju koda.

U fajlu `nightabilitiesserver.cpp` prijavljena je sužavajuća konverzija prilikom smeštanja rezultata funkcije u promenljivu tipa `int`, kao i dve uzastopne identične grane jedne `switch` naredbe. Ovi nalazi pripadaju grupi `bugprone`.

Alat je tokom analize prikazivao i veoma veliki broj generisanih upozorenja koja potiču iz koda biblioteka i drugih delova koji nisu predmet analize. Takva upozorenja su potisnuta i ne predstavljaju broj problema pronađenih u izvornom kodu projekta Mafija.

Dobijeni rezultati pokazuju da najveći deo nalaza predstavlja preporuke koje se odnose na način pisanja, čitljivost i modernizaciju postojećeg C++ koda. Zbog toga svi prijavljeni nalazi nemaju isti značaj za ispravnost programa. U narednoj podsekciji izdvojeni su nalazi koji su ocenjeni kao relevantniji sa stanovišta potencijalnih grešaka i kvaliteta implementacije.


#### Uočeni problemi

Iako veliki deo `clang-tidy` nalaza predstavlja preporuke vezane za čitljivost i modernizaciju koda, analizom izveštaja izdvojeno je nekoliko nalaza koji mogu imati veći značaj za kvalitet i pouzdanost implementacije.

##### Sužavajuće konverzije numeričkih tipova

U fajlu `server_client/Shared/nightabilitiesserver.cpp` pronađena je implicitna sužavajuća konverzija prilikom određivanja slučajnog indeksa:

```cpp
int random_index = rng->bounded(players_to_kill.size());
```

Alat prijavljuje `bugprone-narrowing-conversions`, jer rezultat može biti šireg celobrojnog tipa od promenljive `int` u koju se smešta. Ovakve konverzije mogu dovesti do gubitka vrednosti ukoliko se konvertovana vrednost ne može predstaviti ciljnim tipom.

U konkretnom slučaju broj igrača u realnoj partiji verovatno neće biti dovoljno veliki da izazove takav problem, pa nalaz ne predstavlja neposredno reprodukovanu grešku u radu aplikacije. Ipak, ukazuje na nepodudaranje tipova koje je moguće izbeći korišćenjem odgovarajućeg tipa promenljive.

##### Identične grane `switch` naredbe

U istom fajlu `nightabilitiesserver.cpp` prijavljen je nalaz `bugprone-branch-clone`. Alat je detektovao dve uzastopne grane `switch` naredbe koje sadrže identično ponašanje.

Ovakav nalaz ne mora predstavljati funkcionalnu grešku, jer različite vrednosti mogu namerno imati isto ponašanje. Međutim, duplirana implementacija otežava održavanje koda i može ukazivati na slučajno ponavljanje logike. Zbog toga bi trebalo proveriti da li je identično ponašanje ovih slučajeva namerno i, ukoliko jeste, razmotriti njihovo objedinjavanje.

##### Nepotrebno kopiranje parametara

Na više mesta alat je prijavio `performance-unnecessary-value-param`. Na primer, u klasi `GameState` pojedini objekti tipa `QString` i `std::shared_ptr<Player>` prosleđuju se funkcijama po vrednosti iako se unutar funkcije koriste samo za čitanje.

U takvim slučajevima prosleđivanje preko konstantne reference može izbeći nepotrebno kreiranje kopije objekta. Ovaj nalaz prvenstveno predstavlja potencijalno poboljšanje performansi i ne ukazuje na grešku koja menja rezultat izvršavanja programa.

##### Parametri koje je moguće slučajno zameniti

U funkciji `create_voice_data_message` iz fajla `voiceprotocol.cpp` alat je prijavio `bugprone-easily-swappable-parameters` za susedne parametre `timestamp` i `sequence`. Njihovi tipovi dozvoljavaju međusobne implicitne konverzije, zbog čega bi poziv funkcije sa zamenjenim redosledom argumenata mogao da prođe kompajliranje.

Ovaj nalaz ne znači da je u postojećem kodu pronađen poziv sa pogrešnim redosledom argumenata, već ukazuje na interfejs funkcije kod kojeg takvu grešku kompajler ne bi nužno otkrio.

##### Ostali nalazi

Pored izdvojenih nalaza, `clang-tidy` je prijavio veći broj preporuka koje se uglavnom odnose na stil i održavanje koda. Među njima su dodavanje vitičastih zagrada oko tela upravljačkih konstrukcija, eksplicitno poređenje pokazivača sa `nullptr`, zamena numeričkih konstanti imenovanim konstantama, korišćenje `auto`, uklanjanje dupliranih `include` direktiva i različite preporuke za modernizaciju C++ sintakse.

Ovi nalazi mogu doprineti čitljivijem i lakše održivom kodu, ali sami po sebi ne predstavljaju dokaz funkcionalne greške u programu. Zbog toga su u okviru ove analize nalazi iz grupa `bugprone` i `performance` razmatrani odvojeno od pretežno stilskih preporuka iz grupa `modernize` i `readability`.

#### Ograničenja

Rezultate `clang-tidy` analize potrebno je tumačiti u skladu sa vrstom uključenih provera. Prijavljeno upozorenje ne predstavlja nužno grešku u programu, već može predstavljati preporuku za poboljšanje čitljivosti, performansi ili načina korišćenja savremenih C++ konstrukcija.

Ovo je posebno izraženo kod grupa `modernize` i `readability`, koje su proizvele veliki broj preporuka vezanih za stil i strukturu koda. Zbog toga broj prijavljenih upozorenja nije moguće direktno posmatrati kao broj pronađenih grešaka.

Ni nalazi iz grupe `bugprone` ne predstavljaju automatski potvrđene greške. Na primer, sužavajuća konverzija može biti bezbedna za vrednosti koje se realno pojavljuju u aplikaciji, dok identične grane upravljačke konstrukcije mogu biti rezultat namerne implementacije. Takve nalaze potrebno je dodatno analizirati u kontekstu izvornog koda.

Analiza je ograničena na produkcione izvorne fajlove iz direktorijuma `game`, `GUI` i `server_client`. Testni, build i generisani Qt/CMake fajlovi namerno su isključeni, pa rezultati ne predstavljaju statičku analizu svakog fajla koji postoji u repozitorijumu.

`clang-tidy` je zato u ovom radu korišćen prvenstveno kao alat za izdvajanje potencijalno problematičnih mesta u kodu koja zahtevaju dodatnu proveru, a ne kao dokaz postojanja ili odsustva funkcionalnih grešaka.


### 9.3 Statička analiza pomoću Cppcheck

Kao drugi alat za statičku analizu izvornog koda projekta Mafija korišćen je `Cppcheck`. Za razliku od `clang-tidy` analize, kod koje su bile uključene i grupe provera vezane za modernizaciju i čitljivost koda, Cppcheck je konfigurisan tako da se analiza usmeri na upozorenja, potencijalne probleme sa performansama i prenosivošću koda.

Za analizu su uključene sledeće kategorije:

```text id="fc08fo"
warning
performance
portability
```

Provere iz kategorije `style` namerno su isključene. Na taj način izbegnuto je nepotrebno ponavljanje stilskih provera koje su već obuhvaćene analizom pomoću alata `clang-tidy`.

Za automatizaciju analize napravljena je Bash skripta `cppcheck.sh`, smeštena u direktorijumu `cppcheck/`. Kao i kod `clang-tidy` analize, projekat se najpre konfiguriše i izgrađuje pomoću CMake-a, pri čemu se generiše baza `compile_commands.json`. Na taj način Cppcheck dobija informacije o stvarnom načinu kompajliranja pojedinačnih izvornih fajlova projekta.

Pre pokretanja analize baza komandi za kompajliranje se filtrira kako bi se iz analize isključio direktorijum `tests/`. Pored testova, analiza ne obuhvata CMake i Qt generisane fajlove, `_autogen` i `_deps` direktorijume i Catch2 kod.

Za pravilno prepoznavanje Qt konstrukcija Cppcheck je pokrenut sa opcijom:

```text id="d6g9zi"
--library=qt
```

Analiza je dodatno pokrenuta sa opcijom `--inconclusive`, čime je omogućeno prijavljivanje i nalaza za koje alat ne može sa potpunom sigurnošću da utvrdi da predstavljaju problem.

Analiza se može reprodukovati iz direktorijuma `cppcheck` pokretanjem:

```bash id="j6lljq"
./cppcheck.sh
```

Kompletan izlaz analize čuva se u fajlu:

```text id="5zpgc6"
cppcheck/cppcheck-report.txt
```


#### Konfiguracija i pokretanje analize

Za automatizaciju Cppcheck analize napravljena je Bash skripta `cppcheck.sh`, koja se nalazi u direktorijumu `cppcheck/`. Skripta proverava dostupnost potrebnih alata `cppcheck`, `cmake` i `python3`, a zatim uklanja prethodni build direktorijum i prethodno generisani izveštaj kako bi se svaka analiza izvršavala u čistom okruženju.

Pre pokretanja Cppcheck-a projekat se konfiguriše pomoću CMake-a uz opciju:

```bash
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

Na taj način generiše se baza `compile_commands.json`, koja sadrži informacije o načinu kompajliranja izvornih fajlova projekta, uključujući odgovarajuće include putanje, definicije i opcije kompajlera. Nakon konfiguracije izvršava se i izgradnja projekta kako bi se proverilo da projekat može uspešno da se izgradi i kako bi bili generisani fajlovi potrebni za Qt projekat.

Pre same analize baza `compile_commands.json` se filtrira pomoću Python skripte ugrađene u `cppcheck.sh`. Zadržavaju se fajlovi koji pripadaju projektu Mafija, dok se postojeći testovi iz direktorijuma `tests/` isključuju iz analize.

Cppcheck se zatim pokreće nad filtriranom bazom komandi za kompajliranje. Za pravilnu analizu Qt projekta korišćena je opcija:

```text
--library=qt
```

Uključene su sledeće kategorije provera:

```text
--enable=warning,performance,portability
```

Dok su stilske provere eksplicitno isključene opcijom:

```text
--disable=style
```

Stilske provere nisu korišćene u okviru Cppcheck analize kako bi se izbeglo njihovo nepotrebno preklapanje sa proverama čitljivosti i modernizacije koje su već sprovedene pomoću alata `clang-tidy`.

Dodatno je korišćena opcija `--inconclusive`, koja omogućava da Cppcheck prijavi i potencijalne probleme za koje na osnovu statičke analize ne može sa potpunom sigurnošću da utvrdi da predstavljaju grešku.

Analiza se može reprodukovati iz direktorijuma `cppcheck` pokretanjem:

```bash
./cppcheck.sh
```

Tokom izvršavanja rezultat analize se istovremeno prikazuje na standardnom izlazu i čuva u fajlu:

```text
cppcheck/cppcheck-report.txt
```

Na ovaj način sačuvan je kompletan izlaz Cppcheck analize koji se može koristiti za pregled i reprodukciju dobijenih rezultata.


#### Rezultati Cppcheck analize

Cppcheck analiza uspešno je završena nad svim fajlovima izdvojenim iz filtrirane baze komandi za kompajliranje. Alat je tokom izvršavanja proverio ukupno **32 od 32 fajla**.

Analizom je prijavljeno ukupno **32 nalaza**. Svi prijavljeni nalazi pripadaju kategoriji `performance`, dok u konačnom izveštaju nisu pronađeni nalazi iz uključenih kategorija `warning` i `portability`.

Prijavljeni nalazi mogu se dodatno podeliti prema tipu Cppcheck provere:

| Cppcheck provera        | Broj nalaza | Značenje                                                                                              |
| ----------------------- | ----------: | ----------------------------------------------------------------------------------------------------- |
| `passedByValue`         |          23 | Parametar se prosleđuje po vrednosti iako bi mogao biti prosleđen preko konstantne reference          |
| `passedByValueCallback` |           6 | Isto potencijalno nepotrebno kopiranje kod funkcija koje se koriste kao callback funkcije             |
| `returnByReference`     |           2 | Funkcija vraća kopiju člana objekta koji bi potencijalno mogao biti vraćen preko konstantne reference |
| `iterateByValue`        |           1 | Elementi kolekcije se tokom iteracije kopiraju umesto da im se pristupa preko reference               |
| **Ukupno**              |      **32** |                                                                                                       |

Najveći broj nalaza, ukupno **23**, odnosi se na proveru `passedByValue`. Ovi nalazi ukazuju na funkcije koje primaju složenije objekte, kao što su `QString` i Qt kolekcije, po vrednosti iako se parametri unutar funkcije ne menjaju.

Takvi nalazi pojavljuju se u više delova projekta, uključujući implementaciju glasanja, noćnih akcija, stanja igre i grafičkog korisničkog interfejsa. Na primer, u `votecounter.cpp` Cppcheck prijavljuje parametre `voter`, `vote` i `player`, dok se u `GameState.cpp` isti problem prijavljuje za više funkcija koje kao parametar primaju korisničko ime.

Pored toga, šest nalaza označeno je proverom `passedByValueCallback`. U ovim slučajevima Cppcheck prepoznaje da bi prosleđivanje preko konstantne reference moglo smanjiti kopiranje, ali istovremeno navodi da se analizirana funkcija koristi kao callback funkcija. Zbog toga ovakve preporuke zahtevaju dodatnu pažnju pre eventualne izmene potpisa funkcije.

Dva nalaza `returnByReference` odnose se na funkcije koje vraćaju kopiju člana objekta, dok je jedan nalaz `iterateByValue` prijavljen za `range-based for` petlju u `chatserver.cpp`, u kojoj se elementi kolekcije kopiraju tokom iteracije.

Dobijeni rezultati pokazuju da Cppcheck u analiziranoj verziji projekta nije pronašao probleme iz kategorija `warning` i `portability`. Svi prijavljeni nalazi odnose se na potencijalna poboljšanja performansi, prvenstveno smanjenjem nepotrebnog kopiranja objekata.


#### Uočeni nalazi

Cppcheck analiza nije prijavila probleme iz kategorija `warning` i `portability`. Svi pronađeni nalazi pripadaju kategoriji `performance` i uglavnom ukazuju na mesta na kojima se objekti nepotrebno kopiraju.

Najzastupljeniji nalaz je `passedByValue`. Cppcheck ga prijavljuje kada se složeniji objekat prosleđuje funkciji po vrednosti, iako se unutar funkcije ne menja i mogao bi biti prosleđen preko konstantne reference.

Na primer, u klasi `VoteCounter` ovaj nalaz prijavljen je za parametre `voter`, `vote` i `player`, koji su tipa `QString`. Prosleđivanje ovih objekata po vrednosti može dovesti do kreiranja nepotrebnih kopija.

Isti obrazac posebno je izražen u klasi `GameState`. Cppcheck je prijavio prosleđivanje objekata `QString` po vrednosti u funkcijama kao što su `get_player_by_username`, `is_player_alive`, `kill_player`, `can_player_act`, `can_player_vote`, `is_valid_target`, `player_exists` i `can_player_chat`.

Potencijalno poboljšanje u ovakvim slučajevima bilo bi korišćenje konstantne reference, na primer:

```cpp
bool GameState::is_player_alive(const QString &username)
```

umesto:

```cpp
bool GameState::is_player_alive(QString username)
```

Na taj način funkcija može da koristi postojeći objekat bez njegovog menjanja i bez potrebe za kreiranjem dodatne kopije.

Posebnu grupu predstavljaju nalazi `passedByValueCallback`. Oni se pojavljuju kod funkcija povezanih sa Qt signalima i slotovima. Cppcheck prepoznaje mogućnost prosleđivanja parametara preko konstantne reference, ali istovremeno upozorava da se funkcija koristi kao callback. Takvi nalazi prijavljeni su, između ostalog, u klasi `ChatServer`.

Ove preporuke ne treba primenjivati automatski, jer promena potpisa funkcije koja učestvuje u Qt signal-slot komunikaciji zahteva proveru kompatibilnosti sa odgovarajućim signalom i ostatkom interfejsa.

Cppcheck je prijavio i nalaz `iterateByValue` u fajlu `chatserver.cpp`. U `range-based for` petlji promenljiva `x` deklarisana je po vrednosti, zbog čega se elementi kolekcije kopiraju tokom iteracije:

```cpp
for (auto x : plrs)
```

Alat preporučuje pristup elementima preko konstantne reference kada njihovo kopiranje nije potrebno.

Prijavljena su i dva nalaza `returnByReference`. Jedan od njih odnosi se na funkciju `NightAction::get_target()`, koja vraća objekat `QString` po vrednosti, dok Cppcheck ukazuje da bi član `m_target` mogao biti vraćen preko konstantne reference. Drugi takav nalaz odnosi se na funkciju `ServerWorker::get_username()`.

Pronađeni problemi ne ukazuju na promenu funkcionalnog ponašanja programa, već predstavljaju mogućnosti za smanjenje nepotrebnog kopiranja objekata i potencijalno poboljšanje performansi. Posebno je značajno što se isti obrazac prosleđivanja objekata po vrednosti ponavlja u većem broju komponenti projekta, pa se ne radi o izolovanom slučaju već o obrascu prisutnom u implementaciji.


#### Ograničenja

Rezultati Cppcheck analize predstavljaju statičku procenu izvornog koda i prijavljeni nalazi ne moraju predstavljati stvarne greške u programu. U sprovedenoj analizi svi pronađeni nalazi pripadaju kategoriji `performance` i prvenstveno predstavljaju preporuke za smanjenje nepotrebnog kopiranja objekata.

Posebnu pažnju zahtevaju nalazi `passedByValueCallback`. Cppcheck kod ovih nalaza prepoznaje da se analizirane funkcije koriste kao callback funkcije, odnosno kao deo Qt signal-slot mehanizma. Zbog toga predloženu promenu načina prosleđivanja parametara nije opravdano automatski primeniti bez prethodne provere potpisa odgovarajućih signala i funkcija.

Analiza je izvršena sa uključenim kategorijama `warning`, `performance` i `portability`, dok je kategorija `style` namerno isključena. Zbog toga rezultati Cppcheck analize ne obuhvataju stilske preporuke koje bi alat mogao da prijavi. Takve provere nisu uključene kako bi se izbeglo njihovo preklapanje sa proverama čitljivosti i modernizacije sprovedenim pomoću alata `clang-tidy`.

Iz analize su takođe isključeni postojeći testovi i generisani fajlovi koji nisu deo produkcionog izvornog koda. Dobijeni rezultati se zato odnose na analizirani produkcioni kod projekta Mafija u okviru izabrane konfiguracije Cppcheck-a.

Kao i kod drugih alata za statičku analizu, odsustvo upozorenja iz određene kategorije ne predstavlja dokaz da u programu ne postoje problemi te vrste. Cppcheck može prijaviti samo probleme koje njegove uključene provere mogu statički da prepoznaju.


### 9.4 Dinamička analiza pomoću Valgrind Memcheck

Za dinamičku analizu upravljanja memorijom korišćen je alat Valgrind, odnosno njegov alat `Memcheck`. Za razliku od prethodno korišćenih alata za statičku analizu, Memcheck analizira ponašanje programa tokom njegovog stvarnog izvršavanja.

Cilj analize bio je pronalaženje potencijalnih problema pri radu sa memorijom, kao što su pristup neispravnim memorijskim lokacijama, korišćenje neinicijalizovanih vrednosti i curenje memorije.

Analiza je izvršena nad grafičkom aplikacijom Mafija, odnosno izvršnim fajlom `GUI/GUI`. Tokom rada aplikacije praćen je konkretan scenario igre kako bi se izvršio što veći deo funkcionalnosti povezanih sa tokom jedne partije.

U posmatranom scenariju kreirana je nova igra sa šest igrača. U igri su učestvovala tri građanina, dva člana mafije i jedan detektiv. Nakon kreiranja igre praćen je njen tok od početka do završetka partije, uključujući smenjivanje faza igre i akcije igrača koje se izvršavaju tokom partije.

Na ovaj način Valgrind analiza nije bila ograničena samo na pokretanje i zatvaranje aplikacije, već je Memcheck pratio upravljanje memorijom tokom konkretnog izvršavanja glavnog toka igre. Dobijeni rezultati se zato odnose na putanje kroz program koje su izvršene u okviru opisanog scenarija.

Pošto aplikacija koristi Qt i dodatne sistemske biblioteke, Valgrind tokom izvršavanja prati i kod tih biblioteka. Zbog toga prijavljene probleme nije moguće automatski pripisati izvornom kodu projekta Mafija. Prilikom analize rezultata posebno je posmatran stek poziva svakog relevantnog nalaza kako bi se razlikovali problemi koji potiču iz projekta od problema prijavljenih unutar Qt i drugih biblioteka.


#### Konfiguracija i pokretanje analize

Za automatizaciju Valgrind analize napravljena je Bash skripta `valgrind.sh`, smeštena u direktorijumu `valgrind/`. Skripta najpre proverava dostupnost alata CMake i Valgrind, a zatim uklanja prethodni build direktorijum i prethodno generisani izveštaj.

Projekat se ponovo konfiguriše pomoću CMake-a u `Debug` režimu:

```text id="zw2x8h"
-DCMAKE_BUILD_TYPE=Debug
```

Nakon uspešne konfiguracije izgrađuje se ceo projekat, a zatim se proverava postojanje izvršnog fajla:

```text id="vgr-target"
valgrind/build/GUI/GUI
```

Valgrind Memcheck pokreće se sa sledećim opcijama:

```text id="vgr-options"
--tool=memcheck
--leak-check=full
--show-leak-kinds=all
--track-origins=yes
--num-callers=30
--error-limit=no
--error-exitcode=1
```

Opcijom `--leak-check=full` zahteva se detaljna analiza curenja memorije, dok `--show-leak-kinds=all` omogućava prikaz svih kategorija pronađene memorije. Opcija `--track-origins=yes` koristi se za dodatno praćenje porekla neinicijalizovanih vrednosti, a `--num-callers=30` omogućava prikaz dužeg steka poziva radi lakšeg određivanja porekla problema.

Opcijom `--error-limit=no` isključeno je ograničavanje ukupnog broja prijavljenih grešaka, dok `--error-exitcode=1` omogućava skripti da na osnovu izlaznog statusa prepozna da je Valgrind tokom izvršavanja prijavio greške.

U ovoj analizi nisu korišćeni suppression fajlovi. Zbog toga kompletan izveštaj sadrži i nalaze koji potiču iz Qt-a i drugih biblioteka koje aplikacija koristi. Oni su zadržani u izvornom izveštaju, a njihovo poreklo se razmatra prilikom tumačenja rezultata.

Analiza se može reprodukovati iz direktorijuma `valgrind` pokretanjem:

```bash id="vgr-run"
./valgrind.sh
```

Kompletan izlaz Valgrind analize istovremeno se prikazuje na standardnom izlazu i čuva u fajlu:

```text id="vgr-report"
valgrind/full-report.txt
```


#### Rezultati Valgrind analize

Tokom opisanog scenarija Valgrind Memcheck je pratio izvršavanje grafičke aplikacije od njenog pokretanja, preko kreiranja i odigravanja partije, do završetka izvršavanja.

Na kraju izvršavanja Valgrind je prijavio:

```text
ERROR SUMMARY: 3412 errors from 2941 contexts
```

što znači da je tokom posmatranog izvršavanja registrovano 3412 pojava problema raspoređenih u 2941 različit kontekst. U ovoj analizi nisu korišćena suppression pravila za uklanjanje nalaza koji potiču iz spoljnih biblioteka.

Analiza memorije koja je ostala zauzeta na kraju izvršavanja dala je sledeći rezultat:

| Kategorija        | Količina memorije | Broj blokova |
| ----------------- | ----------------: | -----------: |
| `definitely lost` |         298,088 B |        1,102 |
| `indirectly lost` |         524,100 B |       14,663 |
| `possibly lost`   |           5,480 B |           59 |
| `still reachable` |       3,518,610 B |       41,986 |
| `suppressed`      |              28 B |            1 |

Kategorija `definitely lost` označava memoriju za koju Valgrind na kraju izvršavanja više ne može da pronađe pokazivač na alocirani blok. `Indirectly lost` predstavlja memoriju koja je postala nedostupna posredno, na primer zato što je izgubljen objekat koji je sadržao pokazivače ka drugim alokacijama. Kategorija `possibly lost` obuhvata slučajeve kod kojih Valgrind ne može pouzdano da utvrdi da li je memorija zaista izgubljena, dok `still reachable` označava memoriju koja nije oslobođena do završetka programa, ali za koju i dalje postoji pokazivač.

Međutim, navedene vrednosti predstavljaju zbirne rezultate celog procesa. Pošto aplikacija koristi Qt, Qt Multimedia i druge sistemske biblioteke, u njih ulaze i alokacije i prijavljeni problemi koji ne moraju poticati iz izvornog koda projekta Mafija.

Na primer, već pri inicijalizaciji aplikacije prijavljen je `Invalid read of size 16` čiji se stek poziva nalazi u bibliotekama `Qt6Core`, `Qt6DBus` i `Qt6Gui`, dok se `main.cpp` projekta pojavljuje tek kao mesto sa kojeg je pokrenuta inicijalizacija Qt aplikacije. Zbog toga ovakav nalaz nije opravdano samo na osnovu zbirnog izveštaja proglasiti greškom u kodu projekta Mafija.

Iz tog razloga ukupni `ERROR SUMMARY` i `LEAK SUMMARY` predstavljaju početnu tačku analize, dok je za utvrđivanje relevantnih problema potrebno posebno analizirati stekove poziva i proveriti da li se u njima pojavljuju funkcije i izvorni fajlovi projekta Mafija.


#### Uočeni problemi

Zbog velikog broja nalaza koji potiču iz Qt-a i drugih biblioteka, prilikom pregleda Valgrind izveštaja posebna pažnja posvećena je zapisima čiji stek poziva vodi do izvornog koda projekta Mafija.

Jedan od značajnijih nalaza odnosi se na objekte koji predstavljaju uloge igrača. Tokom kreiranja igre funkcija `ChatServer::start_game()` dinamički kreira objekte odgovarajućih uloga. Valgrind pokazuje da memorija povezana sa tim objektima ostaje izgubljena nakon završetka posmatranog izvršavanja.

Za objekat klase `DetectiveRole` Valgrind prijavljuje:

```text
120 bytes in 1 blocks are indirectly lost
...
Role::Role(QObject*)
DetectiveRole::DetectiveRole(QObject*)
ChatServer::start_game(...)
```

Stek poziva pokazuje da je objekat kreiran tokom izvršavanja funkcije `ChatServer::start_game()` u `chatserver.cpp`.

Isti obrazac pojavljuje se i za objekte klase `MafiaRole`. U posmatranoj partiji postoje dva igrača sa ovom ulogom, a Valgrind prijavljuje:

```text
240 bytes in 2 blocks are indirectly lost
...
Role::Role(QObject*)
MafiaRole::MafiaRole(QObject*)
ChatServer::start_game(...)
```

Broj od dva bloka odgovara broju objekata `MafiaRole` kreiranih u posmatranom scenariju igre.

Analogno tome, za `CitizenRole` prijavljeno je:

```text
360 bytes in 3 blocks are indirectly lost
...
Role::Role(QObject*)
CitizenRole::CitizenRole(QObject*)
ChatServer::start_game(...)
```

U ovom slučaju tri prijavljena bloka odgovaraju trima građanima koji su kreirani prilikom pokretanja analizirane partije.

Ovi nalazi su posebno značajni jer se broj izgubljenih blokova poklapa sa brojem objekata odgovarajućih uloga u konkretnom scenariju: jedan `DetectiveRole`, dva `MafiaRole` i tri `CitizenRole`. Takođe, stekovi poziva direktno vode do funkcije `ChatServer::start_game()`, odnosno do koda projekta Mafija. Zbog toga ovi nalazi predstavljaju znatno jači pokazatelj problema sa životnim vekom objekata u projektu od velikog broja nalaza koji se pojavljuju isključivo unutar Qt ili sistemskih biblioteka.

Valgrind je prijavio i izgubljenu memoriju povezanu sa inicijalizacijom glasovne komunikacije. U steku poziva pojavljuju se `VoiceClient::init_playback()`, konstruktor klase `VoiceClient`, `ChatClient` i `MainWindow`. Međutim, deo same alokacije i narednih poziva nalazi se unutar `Qt6Multimedia`, pa ovaj nalaz nije moguće sa istom sigurnošću pripisati grešci u upravljanju memorijom projekta samo na osnovu Valgrind izveštaja.

Pored curenja memorije, kompletan izveštaj sadrži veliki broj prijava poput `Invalid read` i `Conditional jump or move depends on uninitialised value(s)`. Veliki deo ovih prijava nastaje unutar Qt-a, Wayland-a, multimedijalnih i drugih sistemskih biblioteka, često bez steka koji omogućava da se problem poveže sa konkretnom linijom izvornog koda projekta. Zbog toga oni nisu tretirani kao potvrđene greške projekta Mafija.

Najjasniji nalaz ove analize zato predstavlja upravljanje životnim vekom objekata uloga kreiranih prilikom pokretanja igre. Rezultati ukazuju da bi trebalo proveriti način na koji `ChatServer` kreira, čuva i na kraju oslobađa objekte izvedene iz klase `Role`, odnosno da li im je pravilno definisano vlasništvo i životni vek.


#### Ograničenja

Valgrind Memcheck je alat za dinamičku analizu, pa dobijeni rezultati zavise od putanja kroz program koje su izvršene tokom posmatranog scenarija. U ovoj analizi praćena je jedna kompletna partija sa šest igrača, u kojoj su učestvovala tri građanina, dva člana mafije i jedan detektiv. Zbog toga analiza ne obuhvata sve moguće kombinacije uloga, sve moguće akcije igrača niti sve putanje izvršavanja aplikacije.

Ovo ograničenje je posebno važno prilikom tumačenja odsustva pojedinih nalaza. Na primer, u analiziranom scenariju nije korišćena uloga doktora, pa se u Valgrind izveštaju ne pojavljuje odgovarajući objekat `DoctorRole`. Na osnovu toga se ne može zaključiti da za ovu ulogu ne postoji isti ili sličan problem sa upravljanjem životnim vekom objekta. Nasuprot tome, za uloge koje su bile deo posmatrane partije — `CitizenRole`, `MafiaRole` i `DetectiveRole` — Valgrind je registrovao izgubljenu memoriju povezanu sa objektima kreiranim u funkciji `ChatServer::start_game()`.

Dodatno ograničenje predstavlja korišćenje Qt-a i većeg broja spoljnih biblioteka. Valgrind prati ceo proces, pa kompletan izveštaj sadrži veliki broj nalaza nastalih u Qt-u, Qt Multimedia, Wayland-u i drugim sistemskim bibliotekama. Zbog toga ukupni broj prijavljenih grešaka i ukupna količina izgubljene memorije ne mogu biti direktno predstavljeni kao problemi projekta Mafija.

U analizi nisu korišćeni suppression fajlovi za uklanjanje poznatih nalaza spoljnih biblioteka. Ovakav pristup omogućava očuvanje kompletnog Valgrind izlaza, ali istovremeno značajno povećava količinu rezultata koje je potrebno ručno pregledati. Relevantnost pojedinačnih nalaza zato je procenjivana na osnovu steka poziva i prisustva funkcija i izvornih fajlova projekta Mafija.

Konačno, Valgrind nalaz ukazuje na mesto i okolnosti pod kojima memorija ostaje neoslobođena, ali sam po sebi ne određuje uvek uzrok problema niti način na koji bi vlasništvo nad objektom trebalo ispraviti. Za potvrđene nalaze povezane sa objektima uloga potrebno je dodatno analizirati način njihovog kreiranja, čuvanja i uništavanja u implementaciji `ChatServer` klase.


### 9.5 Fuzz testiranje pomoću libFuzzera

Za fuzz testiranje projekta Mafija korišćen je `libFuzzer`, coverage-guided fuzzing alat koji generiše i menja ulazne podatke sa ciljem pronalaženja novih putanja kroz program i otkrivanja ulaza koji mogu izazvati pad programa ili druge greške tokom izvršavanja.

Za fuzz testiranje odabrana je komponenta `protocol`, koja definiše strukturu i pomoćne funkcije za formiranje poruka koje se razmenjuju između različitih delova aplikacije. Ova komponenta predstavlja pogodan cilj za fuzz testiranje zato što obrađuje podatke koji se pojavljuju u komunikacionom protokolu i sadrži veći broj konverzija i funkcija za kreiranje različitih tipova poruka.

Za potrebe analize implementiran je fuzz harness `fuzz_protocol.cpp`. Svaki ulaz koji generiše libFuzzer najpre se posmatra kao niz bajtova maksimalne dužine 4096 bajtova. Ulaz se zatim pretvara u `QString`, nakon čega se pozivaju funkcije za konverziju između tekstualnih vrednosti i enumeracija `MessageType` i `Role`.

Isti ulaz se zatim pokušava parsirati kao JSON dokument. Neispravni JSON ulazi i JSON vrednosti koje nisu objekti bezbedno se odbacuju, dok se za validne JSON objekte nastavlja obrada pojedinačnih polja.

Harness obrađuje osnovna polja protokola, podatke o igračima, glasovima, noćnim akcijama, ulogama i čekaonici. Na osnovu dobijenih vrednosti zatim se pozivaju funkcije iz `protocol.cpp` za kreiranje različitih poruka, između ostalog poruka za prijavljivanje igrača, chat komunikaciju, noćne akcije, stanje igrača i igre, glasanje, dodelu uloga, početak i kraj igre i ažuriranje čekaonice.

Na kraju se svi generisani `QJsonObject` objekti ponovo serijalizuju u JSON. Na ovaj način se za svaki fuzz ulaz ne proverava samo JSON parser, već se ulaz propagira kroz veći broj funkcija komunikacionog protokola projekta Mafija.

#### Konfiguracija i pokretanje fuzz testiranja

Za automatizaciju fuzz testiranja napravljena je Bash skripta `fuzz_protocol.sh`, smeštena u direktorijumu `libfuzzer/`. Skripta proverava dostupnost alata `clang`, `clang++` i `cmake`, priprema početni corpus, ponovo izgrađuje fuzz target i zatim pokreće libFuzzer. Kompletan izlaz izvršavanja istovremeno se prikazuje na standardnom izlazu i čuva u fajlu `libfuzzer_report.txt`.
Za početni corpus pripremljeni su ulazi koji predstavljaju karakteristične vrednosti komunikacionog protokola projekta. Corpus sadrži jednostavne tekstualne vrednosti za tipove poruka i uloge, kao što su `message`, `game_state_update`, `night_started`, `Mafia`, `Police`, `Doctor` i `Citizen`.

Pored tekstualnih vrednosti, u početni corpus uključene su validne JSON poruke koje predstavljaju različite situacije iz protokola, među kojima su prijavljivanje i napuštanje igre, noćne akcije, glasanje, promena stanja igre, dodela uloga, početak igre, završetak igre i ažuriranje podataka u čekaonici. Za složenije strukture uključeni su i ulazi sa nizovima igrača, glasovima, informacijama o noćnim akcijama i brojem pojedinačnih uloga.

Corpus sadrži i nekoliko namerno neispravnih ili neuobičajenih JSON ulaza, kao što su prazan objekat, `null` vrednosti, polje `type` pogrešnog tipa i sintaksno nepotpun JSON. Njihova svrha je da se od samog početka testira ponašanje implementacije i za ulaze koji ne odgovaraju očekivanoj strukturi protokola.

Pri prvom pokretanju skripte formiran je početni corpus od **36 ulaza**. Ako corpus već postoji i nije prazan, skripta ga ne briše, već zadržava postojeće ulaze, čime je omogućeno da se ulazi koje libFuzzer pronađe tokom prethodnih izvršavanja koriste u narednim analizama.
Pre izgradnje fuzz targeta briše se prethodni `build` direktorijum. Projekat za fuzz testiranje zatim se konfiguriše korišćenjem kompajlera Clang i Clang++, nakon čega se izgrađuje izvršni fajl `fuzz_protocol`.

Fuzz testiranje pokreće se komandom ekvivalentnom:

```bash
./build/fuzz_protocol \
    ./corpus \
    -artifact_prefix=./crashes/ \
    -max_len=4096 \
    -max_total_time=60
```

Opcijom `-max_len=4096` maksimalna veličina jednog generisanog ulaza ograničena je na 4096 bajtova, dok `-max_total_time=60` ograničava trajanje jednog pokretanja fuzz testiranja na 60 sekundi. Direktorijum `crashes/` određen je kao mesto za čuvanje ulaza koji bi izazvali pad ili drugi problem koji libFuzzer registruje kao grešku.

Analiza se može reprodukovati iz direktorijuma `libfuzzer` pokretanjem:

```bash
./fuzz_protocol.sh
```

U analiziranom pokretanju korišćeni su Clang i Clang++ verzije **21.1.8** i CMake verzije **4.2.3**, a fuzz target je uspešno konfigurisan i izgrađen pre početka fuzz testiranja.




#### Rezultati fuzz testiranja

Fuzz testiranje komponente `protocol` izvršavano je tokom **60 sekundi**, počevši od početnog corpusa koji je sadržao 36 pripremljenih ulaza. Nakon inicijalizacije libFuzzer je prikazao sledeće početno stanje:

```text
#37 INITED cov: 1065 ft: 1111 corp: 27/1356b exec/s: 0 rss: 46Mb
```

Na kraju izvršavanja dobijen je rezultat:

```text
#1123223 DONE cov: 1140 ft: 3009 corp: 232/122Kb lim: 4096 exec/s: 18413 rss: 562Mb
```

Pojedinačne vrednosti koje libFuzzer prikazuje imaju sledeće značenje:

| Metrika    | Konačna vrednost | Značenje                                                                                                                                      |
| ---------- | ---------------: | --------------------------------------------------------------------------------------------------------------------------------------------- |
| `#1123223` |        1.123.223 | Ukupan broj izvršenih fuzz ulaza do završetka testiranja                                                                                      |
| `DONE`     |                — | Fuzz sesija je završena predviđenim završetkom izvršavanja                                                                                    |
| `cov`      |             1140 | Broj pokrivenih kontrolnih tačaka koje libFuzzer koristi za praćenje pokrivenosti instrumentisanog programa                                   |
| `ft`       |             3009 | Broj pronađenih coverage features, odnosno osobina izvršavanja koje libFuzzer koristi kao povratnu informaciju pri izboru interesantnih ulaza |
| `corp`     |       232/122 KB | Corpus sadrži 232 sačuvana ulaza ukupne veličine približno 122 KB                                                                             |
| `lim`      |             4096 | Trenutna maksimalna veličina ulaza koji libFuzzer generiše, u bajtovima                                                                       |
| `exec/s`   |            18413 | Približan broj fuzz ulaza izvršenih u sekundi u trenutku završetka                                                                            |
| `rss`      |           562 MB | Resident Set Size, odnosno približna količina fizičke memorije koju je proces u tom trenutku zauzimao                                         |

Broj ispred statusa, `#1123223`, pokazuje koliko je ulaza libFuzzer do tog trenutka izvršio. Prema tome, tokom ovog pokretanja izvršeno je ukupno **1.123.223 fuzz iteracija**.

Metrika `cov` predstavlja pokrivenost koju libFuzzer prati nad instrumentisanim kodom. Nakon inicijalizacije njena vrednost iznosila je 1065, dok je na kraju dostigla 1140. Povećanje ove vrednosti pokazuje da su mutirani ulazi tokom testiranja doveli do izvršavanja dodatnih delova analiziranog koda. Ovu metriku ne treba poistovećivati sa procentualnom pokrivenošću linija dobijenom pomoću GCOV/LCOV analize, jer libFuzzer prikazuje sopstvenu coverage metriku namenjenu usmeravanju fuzz procesa.

Metrika `ft` predstavlja broj osobina izvršavanja koje libFuzzer prati kako bi procenio da li je neki generisani ulaz interesantan. Njena vrednost povećana je sa 1111 nakon inicijalizacije na **3009** na kraju testiranja. Rast `ft` pokazuje da je libFuzzer tokom rada otkrivao nova ponašanja relevantna za usmeravanje daljeg generisanja ulaza. Zbog toga `ft` može nastaviti da raste i kada se vrednost `cov` ne promeni.

Metrika `corp` opisuje trenutni radni corpus. Konačna vrednost `232/122Kb` znači da je libFuzzer sačuvao **232 interesantna ulaza**, ukupne veličine približno **122 KB**. Ulaz se zadržava u corpus-u kada doprinosi informacijama koje libFuzzer koristi za dalje istraživanje programa. Zbog toga veličina corpusa ne predstavlja broj svih generisanih ulaza — tokom analize izvršeno je više od milion iteracija, ali je sačuvan samo mali deo ulaza koji su bili korisni za dalje fuzz testiranje.

Tokom rada u izveštaju se pojavljuju oznake `NEW` i `REDUCE`. Oznaka `NEW` pokazuje da je pronađen ulaz koji donosi novu pokrivenost ili novu osobinu izvršavanja i zato se dodaje u corpus. Oznaka `REDUCE` označava da je pronađena manja ili jednostavnija verzija ulaza koja zadržava relevantno ponašanje, čime libFuzzer pokušava da corpus održava što efikasnijim.

Vrednost `lim` predstavlja ograničenje veličine generisanog ulaza. Na kraju izvršavanja iznosila je **4096 bajtova**, što odgovara opciji `-max_len=4096` zadatoj u skripti.

Metrika `exec/s` predstavlja brzinu izvršavanja fuzz testova. Na kraju analize iznosila je približno **18.413 izvršavanja u sekundi**. Veća vrednost omogućava da se u zadatom vremenu ispita veći broj različitih mutacija ulaza.

Metrika `rss` predstavlja količinu fizičke memorije koju je proces imao rezidentnu u memoriji. Na kraju izvršavanja iznosila je približno **562 MB**. Ova vrednost predstavlja potrošnju memorije fuzz procesa tokom rada i sama po sebi ne predstavlja curenje memorije.

LibFuzzer je tokom izvršavanja formirao i preporučeni rečnik vrednosti koje su se pokazale korisnim pri generisanju novih ulaza. Među njima se pojavljuju fragmenti kao što su `is_selec`, `vote`, `user` i `amount_`. Njihovo pojavljivanje pokazuje da je libFuzzer tokom analize prepoznao određene sekvence bajtova kao korisne za dostizanje različitih ponašanja analiziranog koda.

Najvažniji rezultat ovog pokretanja jeste da se fuzz testiranje završilo oznakom `DONE`, bez prijavljenog crash-a ili drugog ulaza koji bi prekinuo izvršavanje fuzz targeta. U okviru **1.123.223 izvršavanja** nije pronađen ulaz koji bi doveo do pada analiziranog fuzz targeta.

Ovaj rezultat ne predstavlja dokaz da komponenta `protocol` ne sadrži greške za proizvoljne ulaze. On pokazuje da u okviru implementiranog fuzz harness-a, korišćenog corpusa, maksimalne veličine ulaza od 4096 bajtova i vremenskog ograničenja od 60 sekundi libFuzzer nije pronašao ulaz koji izaziva pad programa.


#### Ograničenja

Rezultati fuzz testiranja odnose se isključivo na funkcionalnosti koje su obuhvaćene implementiranim fuzz harness-om. U ovoj analizi kao fuzz target odabrana je komponenta `protocol`, pa rezultat ne govori o otpornosti ostalih delova projekta Mafija na neočekivane ili neispravne ulaze.

Iako harness poziva veliki broj funkcija iz `protocol.cpp`, on ne reprodukuje kompletnu komunikaciju između stvarnog klijenta i servera. Generisani podaci obrađuju se direktno unutar fuzz targeta, bez uspostavljanja mrežne konekcije i bez kompletnog stanja koje postoji tokom stvarne partije. Zbog toga greške koje zavise od redosleda mrežnih poruka, stanja klijenta ili servera, konkurentnog izvršavanja ili interakcije više učesnika nisu obuhvaćene ovom analizom.

Rezultat takođe zavisi od kvaliteta početnog corpusa i vremena dostupnog libFuzzeru za istraživanje novih ulaza. Početni corpus sadržao je 36 ručno pripremljenih ulaza koji predstavljaju karakteristične tekstualne vrednosti, validne poruke protokola i određene neispravne JSON strukture. Tokom izvršavanja libFuzzer je ovaj skup proširivao novim ulazima, ali početni corpus ipak utiče na delove prostora ulaza koje alat može efikasnije da istraži.

Jedno pokretanje fuzz testiranja ograničeno je na 60 sekundi i maksimalnu veličinu pojedinačnog ulaza od 4096 bajtova. Duže izvršavanje ili drugačiji početni corpus mogli bi dovesti do pronalaženja dodatnih putanja kroz program i potencijalno novih problema koji tokom sprovedene analize nisu dostignuti.

Konačna vrednost `cov: 1140` ne predstavlja procenat pokrivenosti izvornog koda i ne može se direktno porediti sa rezultatima GCOV/LCOV analize. Ona predstavlja internu metriku pokrivenosti koju libFuzzer koristi za usmeravanje generisanja novih ulaza. Slično tome, vrednost `ft: 3009` predstavlja osobine izvršavanja koje libFuzzer koristi kao povratnu informaciju, a ne broj testiranih funkcija ili linija izvornog koda.

Činjenica da je izvršavanje završeno oznakom `DONE` nakon 1.123.223 iteracija bez pronađenog crash-a pokazuje da u okviru sprovedenog fuzz testiranja nije pronađen ulaz koji izaziva pad fuzz targeta. Međutim, odsustvo pronađenog crash-a ne predstavlja dokaz odsustva grešaka u komponenti `protocol`. Greška može zahtevati ulaz koji libFuzzer nije generisao, duže izvršavanje, drugačije stanje programa ili interakciju sa komponentama koje nisu uključene u fuzz harness.

Zbog navedenih ograničenja rezultat libFuzzer analize treba posmatrati kao proveru otpornosti odabranog dela komunikacionog protokola na veliki broj automatski generisanih i mutiranih ulaza, a ne kao potpunu verifikaciju komunikacionog sistema projekta Mafija.

### 9.6 Analiza kompleksnosti pomoću Lizard

Za analizu kompleksnosti izvornog koda projekta Mafija korišćen je alat `Lizard`. Za razliku od prethodno korišćenih alata, cilj ove analize nije pronalaženje konkretnih funkcionalnih grešaka, već identifikovanje funkcija koje se izdvajaju povećanom ciklomatskom kompleksnošću ili dužinom.

Veća kompleksnost funkcije može otežati razumevanje i održavanje koda, ali i njegovo potpuno testiranje, jer veći broj nezavisnih putanja kroz funkciju zahteva veći broj testnih slučajeva. Zbog toga rezultati Lizard analize mogu ukazati na delove implementacije koji predstavljaju dobre kandidate za dodatno testiranje ili refaktorisanje.

Lizard za analizirane funkcije prikazuje nekoliko metrika:

| Metrika    | Značenje                                                                                              |
| ---------- | ----------------------------------------------------------------------------------------------------- |
| `NLOC`     | Broj linija koda funkcije, bez praznih linija i komentara koje alat ne računa kao deo efektivnog koda |
| `CCN`      | Ciklomatska kompleksnost funkcije, odnosno mera broja nezavisnih putanja kroz njen kontrolni tok      |
| `token`    | Broj leksičkih tokena koji čine funkciju                                                              |
| `PARAM`    | Broj parametara funkcije                                                                              |
| `length`   | Ukupna dužina funkcije izražena brojem linija između njenog početka i kraja                           |
| `location` | Naziv funkcije, opseg linija i izvorni fajl u kojem se funkcija nalazi                                |

U ovoj analizi posebna pažnja posvećena je metrikama `CCN` i `length`. Ciklomatska kompleksnost raste sa povećanjem broja grananja i mogućih putanja kroz funkciju. Funkcija sa `CCN = 1` ima jednostavan kontrolni tok bez dodatnih grananja, dok veće vrednosti ukazuju na veći broj mogućih putanja kroz funkciju.

Dužina funkcije sama po sebi ne predstavlja grešku, ali veoma duge funkcije mogu obavljati više različitih zadataka i biti teže za razumevanje, testiranje i održavanje. Zbog toga je u analizi korišćen i poseban prag za njihovu identifikaciju.

Na nivou celog analiziranog koda Lizard je pronašao **301 funkciju** i ukupno **4448 NLOC**. Prosečna funkcija ima **11,1 NLOC**, dok prosečna ciklomatska kompleksnost iznosi **2,6**. Alat je prema zadatim pragovima izdvojio ukupno **10 upozorenja**.

Ovi zbirni rezultati pokazuju da prosečna funkcija u projektu nema visoku ciklomatsku kompleksnost. Međutim, prosečne vrednosti mogu prikriti pojedinačne funkcije koje se značajno izdvajaju, pa su u nastavku posebno analizirane funkcije koje prelaze zadate pragove.


#### Konfiguracija i pokretanje analize

Za automatizaciju analize kompleksnosti napravljena je Bash skripta `lizard.sh`, smeštena u direktorijumu `lizard/`. Skripta određuje putanju do Git podmodula `Mafija` i definiše fajl `lizard_results.txt` u koji se čuva kompletan rezultat analize.

Pre pokretanja analize skripta proverava da li je alat `lizard` dostupan u sistemu i prikazuje njegovu verziju. Ako alat nije pronađen, skripta prekida izvršavanje i prikazuje komandu za njegovu instalaciju pomoću `pipx`. Takođe se proverava postojanje direktorijuma analiziranog projekta.
Pre svakog novog pokretanja briše se prethodni fajl sa rezultatima, čime se obezbeđuje da `lizard_results.txt` sadrži samo rezultate trenutne analize.

Lizard je pokrenut sledećom komandom:

```bash
lizard \
    -C 10 \
    -L 50 \
    -x "*/tests/*" \
    "$MAFIJA_DIR"
```

Opcijom `-C 10` postavljen je prag ciklomatske kompleksnosti na **10**. Funkcije čija je vrednost `CCN` veća od 10 Lizard izdvaja u sekciji sa upozorenjima. Ovaj prag omogućava da se pažnja usmeri na funkcije sa većim brojem nezavisnih putanja kroz kontrolni tok.

Opcijom `-L 50` postavljen je prag dužine funkcije na **50 linija**. Funkcije duže od ovog praga takođe se izdvajaju u sekciji sa upozorenjima, čak i kada njihova ciklomatska kompleksnost nije velika. Na taj način analiza obuhvata i funkcije koje nisu posebno složene po broju grananja, ali su dovoljno velike da mogu otežavati razumevanje i održavanje koda.

Opcijom:

```bash
-x "*/tests/*"
```

iz analize je isključen direktorijum sa postojećim testovima. Time se Lizard analiza usmerava na implementaciju samog projekta, umesto da zbirne metrike i upozorenja uključuju i testni kod.

Kompletan izlaz alata pomoću komande `tee` istovremeno se prikazuje na standardnom izlazu i čuva u fajlu `lizard_results.txt`.

Analiza se može reprodukovati iz direktorijuma `lizard` pokretanjem:

```bash
./lizard.sh
```

Nakon završetka analize skripta ispisuje lokaciju generisanog fajla `lizard_results.txt`, koji sadrži metrike za pojedinačne funkcije, zbirne metrike projekta i funkcije koje prelaze postavljene pragove.


#### Rezultati analize kompleksnosti

Lizard je u analiziranom kodu identifikovao ukupno **301 funkciju** i **4448 NLOC**. Prosečan broj NLOC po funkciji iznosi **11,1**, dok je prosečna ciklomatska kompleksnost **2,6**. Prema pragovima postavljenim prilikom pokretanja analize, alat je izdvojio ukupno **10 upozorenja**.

Upozorenja se odnose na funkcije koje imaju ciklomatsku kompleksnost veću od 10 ili dužinu veću od 50 linija. Rezultati izdvojenih funkcija prikazani su u sledećoj tabeli:

| Funkcija                               | NLOC | CCN | Dužina | Razlog izdvajanja |
| -------------------------------------- | ---: | --: | -----: | ----------------- |
| `ChatServer::handle_game_start`        |   50 |   9 |     70 | dužina            |
| `ChatServer::start_game`               |   76 |  14 |     89 | CCN i dužina      |
| `mafia_game::message_type_from_string` |   43 |  21 |     43 | CCN               |
| `mafia_game::message_type_to_string`   |   25 |  21 |     25 | CCN               |
| `ChatClient::json_received`            |   62 |  19 |     63 | CCN i dužina      |
| `MainGameWindow::MainGameWindow`       |   60 |   2 |     75 | dužina            |
| `MainGameWindow::intro_animation`      |   74 |   5 |     88 | dužina            |
| `JoinGameWindow::error`                |   56 |  18 |     56 | CCN i dužina      |
| `StartGameWindow::connected`           |   49 |   6 |     54 | dužina            |
| `StartGameWindow::error`               |   57 |  18 |     57 | CCN i dužina      |

Najveća ciklomatska kompleksnost zabeležena je u funkcijama `mafia_game::message_type_from_string` i `mafia_game::message_type_to_string`, koje obe imaju **CCN 21**. Prva funkcija ima 43 NLOC, a druga 25 NLOC, tako da su izdvojene isključivo zbog povećane ciklomatske kompleksnosti, a ne zbog dužine. Visoka vrednost CCN u ovom slučaju ukazuje na veliki broj mogućih grana prilikom pretvaranja između tekstualnih vrednosti i tipova poruka protokola.

Funkcija `ChatClient::json_received` ima **CCN 19**, 62 NLOC i ukupnu dužinu od 63 linije, pa prelazi oba postavljena praga. Ova funkcija obrađuje primljene JSON poruke i na osnovu njihovog tipa usmerava obradu na odgovarajuće funkcionalnosti klijenta. Veliki broj mogućih tipova poruka prirodno povećava broj grana, ali istovremeno znači da funkcija predstavlja deo implementacije sa većim brojem mogućih putanja izvršavanja.

Funkcije `JoinGameWindow::error` i `StartGameWindow::error` imaju **CCN 18**. Njihove dužine iznose 56 i 57 linija, pa obe prelaze i prag ciklomatske kompleksnosti i prag dužine. Povećana kompleksnost funkcija za obradu grešaka pokazuje da se u njima razlikuje veći broj mogućih stanja ili tipova grešaka.

`ChatServer::start_game` ima **CCN 14**, 76 NLOC i ukupnu dužinu od 89 linija. Funkcija zato takođe prelazi oba postavljena praga. Pošto je odgovorna za pokretanje igre, ova vrednost ukazuje da je u jednoj funkciji objedinjeno više koraka potrebnih za inicijalizaciju partije.

Preostalih pet upozorenja nastalo je samo zbog dužine funkcija. `ChatServer::handle_game_start` ima CCN 9 i dužinu 70 linija, `MainGameWindow::MainGameWindow` CCN 2 i dužinu 75 linija, `MainGameWindow::intro_animation` CCN 5 i dužinu 88 linija, dok `StartGameWindow::connected` ima CCN 6 i dužinu 54 linije. Funkcije iz ove grupe nisu izdvojene zbog velike ciklomatske kompleksnosti, već zato što njihova veličina prelazi postavljeni prag od 50 linija.

Rezultati pokazuju da povećana dužina funkcije i povećana ciklomatska kompleksnost ne moraju biti povezane. Na primer, konstruktor `MainGameWindow::MainGameWindow` ima dužinu od 75 linija, ali CCN svega 2, dok `message_type_to_string` ima samo 25 linija, ali CCN 21. Zbog toga je korisno posmatrati obe metrike prilikom identifikovanja delova koda koji mogu biti teži za razumevanje, održavanje ili testiranje.

Od ukupno 301 analizirane funkcije samo je **10 funkcija** izdvojeno prema zadatim pragovima. Zbirna prosečna vrednost CCN od 2,6 pokazuje da povećana kompleksnost nije ravnomerno raspoređena kroz projekat, već je koncentrisana u manjem broju funkcija koje obavljaju složeniju obradu protokola, serverske logike i korisničkog interfejsa.

#### Ograničenja

Lizard sprovodi statičku analizu strukture izvornog koda i izračunava metrike kompleksnosti bez izvršavanja programa. Zbog toga povećana vrednost ciklomatske kompleksnosti ili dužine funkcije ne predstavlja dokaz postojanja greške, već ukazuje na delove implementacije koji mogu biti složeniji za razumevanje, testiranje i održavanje.

Prilikom analize korišćeni su pragovi `CCN > 10` i `length > 50`. Izbor ovih pragova utiče na broj funkcija koje će alat izdvojiti kao upozorenja. Primena strožih pragova dovela bi do većeg broja izdvojenih funkcija, dok bi blaži pragovi prikazali samo funkcije sa izrazito velikim vrednostima kompleksnosti ili dužine. Zbog toga broj od 10 upozorenja treba tumačiti u odnosu na pragove korišćene u ovoj analizi, a ne kao apsolutan broj problematičnih funkcija.

Ciklomatska kompleksnost takođe ne uzima u obzir semantičku opravdanost pojedinačnih grananja. Na primer, funkcije `message_type_from_string` i `message_type_to_string` imaju najveću zabeleženu vrednost `CCN` od 21, ali veliki broj grana proizlazi iz potrebe da se razlikuje veći broj tipova poruka protokola. Sa druge strane, visoka vrednost i dalje znači da kroz takvu funkciju postoji veći broj mogućih putanja koje je potrebno uzeti u obzir prilikom testiranja.

Slično tome, velika dužina funkcije ne mora biti povezana sa složenim kontrolnim tokom. Konstruktor `MainGameWindow::MainGameWindow` ima dužinu od 75 linija, ali ciklomatsku kompleksnost svega 2. Nasuprot tome, `message_type_to_string` ima dužinu od samo 25 linija, ali `CCN` 21. Ovi primeri pokazuju da se pojedinačne metrike ne mogu posmatrati izolovano prilikom procene složenosti implementacije.

Iz analize je isključen direktorijum `tests/`, pa dobijene zbirne metrike i upozorenja ne obuhvataju postojeći testni kod projekta. Analiza je namerno usmerena na implementaciju aplikacije.

Konačno, Lizard ne određuje da li funkciju zaista treba refaktorisati niti predlaže funkcionalno ispravnu podelu složenog koda. Dobijeni rezultati predstavljaju indikatore koji pomažu pri identifikovanju delova projekta kojima bi prilikom daljeg razvoja trebalo posvetiti dodatnu pažnju. Za donošenje odluke o refaktorisanju potrebno je uz metrike razmotriti i odgovornost funkcije, strukturu okolnog koda i logiku koju ona implementira.

## 10. Zaključak

U okviru ovog rada sprovedena je analiza projekta Mafija primenom više različitih tehnika verifikacije softvera. Korišćeni su jedinični testovi i analiza pokrivenosti pomoću QTest-a i GCOV/LCOV-a, statička analiza pomoću alata `clang-tidy` i Cppcheck, dinamička analiza upravljanja memorijom pomoću Valgrind Memcheck-a, fuzz testiranje komunikacionog protokola pomoću libFuzzera i analiza kompleksnosti izvornog koda pomoću alata Lizard.

Početnom proverom utvrđeno je da se analizirana verzija projekta uspešno konfiguriše, izgrađuje i pokreće. Postojeći Catch2 testni skup takođe se uspešno izvršava: svih 40 test slučajeva sa ukupno 252 provere prolazi. Međutim, dodatni QTest testovi pokazali su da postojeći testni skup ne obuhvata sve značajne granične slučajeve.

### Uočeni problemi

Najznačajniji funkcionalni problem otkriven je dodatnim testiranjem logike glasanja. Vrednost `voting_for_nobody` koristi se kao posebna vrednost koja označava glasanje ni za jednog igrača, ali istovremeno može biti prihvaćena kao korisničko ime stvarnog igrača. Zbog ove kolizije značenja dolazi do problema prilikom obrade glasova kada u partiji zaista postoji igrač sa tim korisničkim imenom. Ovaj slučaj nije bio obuhvaćen postojećim Catch2 testovima, dok su ga novi QTest testovi uspešno otkrili.

Analiza pokrivenosti pokazala je da testovi pokrivaju 77,3% linija i 80,7% funkcija, ali samo 49,8% grana. Niža pokrivenost grana nije sama po sebi programska greška, ali pokazuje da značajan broj mogućih putanja izvršavanja nije obuhvaćen postojećim testovima i predstavlja prostor za proširenje testnog skupa.

Drugi značajan problem uočen je pomoću Valgrind Memcheck-a. Tokom kompletne partije sa šest igrača registrovana je izgubljena memorija povezana sa objektima `CitizenRole`, `MafiaRole` i `DetectiveRole` kreiranim prilikom pokretanja igre u funkciji `ChatServer::start_game()`. Broj registrovanih objekata odgovarao je broju odgovarajućih uloga u izvršenom scenariju, što ukazuje da je potrebno dodatno ispitati vlasništvo i životni vek ovih objekata. Pošto u analiziranoj partiji nije učestvovao doktor, na osnovu ovog izvršavanja nije moguće doneti isti zaključak za `DoctorRole`.

Ova dva rezultata — problem sa posebnom vrednošću `voting_for_nobody` i izgubljena memorija povezana sa objektima uloga — predstavljaju najznačajnije konkretne probleme uočene tokom sprovedene analize.

### Upozorenja i preporuke za poboljšanje

Za razliku od prethodnih problema, nalazi statičkih analizatora ne treba automatski tumačiti kao potvrđene programske greške. `clang-tidy` je izdvojio potencijalno rizične konstrukcije, među kojima su sužavajuće konverzije numeričkih tipova, identične uzastopne grane i susedni parametri kompatibilnih tipova koje je moguće lako zameniti prilikom poziva funkcije. Pored toga, prijavljene su preporuke koje se odnose na nepotrebno prosleđivanje objekata po vrednosti, čitljivost i modernizaciju C++ koda. Ovi nalazi predstavljaju mesta koja treba pregledati i po potrebi izmeniti, ali sami po sebi ne potvrđuju postojanje funkcionalne greške.

Cppcheck je prijavio 32 nalaza i svi su pripadali kategoriji performansi. Najveći broj odnosi se na prosleđivanje objekata po vrednosti i moguće nepotrebno kopiranje. Nisu prijavljeni nalazi iz kategorija `warning` i `portability`. Zbog toga rezultate Cppcheck-a prvenstveno treba posmatrati kao preporuke za optimizaciju načina prosleđivanja i vraćanja objekata, a ne kao skup pronađenih funkcionalnih grešaka.

Lizard je od 301 analizirane funkcije izdvojio 10 koje prelaze postavljeni prag ciklomatske kompleksnosti, dužine ili oba kriterijuma. Najveća ciklomatska kompleksnost iznosila je 21 i registrovana je u funkcijama `message_type_from_string` i `message_type_to_string`. Ovi rezultati takođe nisu programske greške. Oni identifikuju delove implementacije koji mogu biti zahtevniji za razumevanje, testiranje i održavanje i koji predstavljaju moguće kandidate za dodatno testiranje ili refaktorisanje.

### Analize bez pronađenog pada programa

Fuzz testiranjem komponente `protocol` izvršeno je više od 1,1 milion iteracija tokom 60 sekundi. LibFuzzer je tokom izvršavanja proširivao corpus i pronalazio nove putanje i osobine izvršavanja, ali nije pronađen ulaz koji bi izazvao pad fuzz targeta. Ovaj rezultat ne znači da komponenta `protocol` ne sadrži greške, već samo da u okviru implementiranog harness-a, korišćenog corpusa, maksimalne veličine ulaza i vremenskog ograničenja nije pronađen ulaz koji dovodi do pada programa.

### Završna ocena analize

Rezultati rada pokazuju zbog čega je korisno kombinovati različite tehnike verifikacije. Dodatni QTest testovi otkrili su funkcionalni problem koji postojeći testovi nisu obuhvatili, dok je Valgrind ukazao na problem sa upravljanjem životnim vekom objekata. Sa druge strane, `clang-tidy`, Cppcheck i Lizard prvenstveno su identifikovali potencijalno rizična mesta i preporuke za poboljšanje kvaliteta, performansi, čitljivosti i održavanja koda. LibFuzzer je omogućio proveru ponašanja odabranog dela komunikacionog protokola za veliki broj automatski generisanih ulaza, bez pronađenog pada u sprovedenom izvršavanju.

Na taj način je važno razlikovati **otkrivene probleme** od **upozorenja i preporuka alata**. Nalaz statičkog analizatora ili povećana vrednost metrike kompleksnosti ne predstavlja automatski dokaz postojanja greške, već ukazuje na mesto koje zahteva dodatnu analizu. Nasuprot tome, neuspešni testovi koji reprodukuju konkretan granični slučaj i Valgrind nalaz izgubljene memorije pružaju direktnije dokaze o problematičnom ponašanju analizirane implementacije.

Sprovedena analiza ipak ne predstavlja potpunu verifikaciju projekta Mafija. Dinamičke analize zavise od izvršenih scenarija, fuzz testiranje od implementiranog harness-a, corpusa i vremena izvršavanja, pokrivenost od dostupnog skupa testova, dok statički analizatori i metrike kompleksnosti mogu prijaviti nalaze koji ne predstavljaju stvarne greške. Dobijeni rezultati zato predstavljaju osnovu za dalje testiranje, potvrđivanje pojedinačnih nalaza, ispravljanje uočenih problema i unapređenje kvaliteta projekta.

