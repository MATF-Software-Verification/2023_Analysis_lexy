# Analiza projekta Mafija 

Seminarski rad u okviru kursa Verifikacija softvera na master studijama Matematičkog fakulteta, Univerziteta u Beogradu  
Autor: Milica Mihailović  
Broj indeksa: 1023/2022  

## Informacije o analiziranom projektu  

Naziv projekta: Mafija  
GitLab repozitorijum: [https://gitlab.com/matf-bg-ac-rs/course-rs/projects-2025-2026/Mafija.git](https://gitlab.com/matf-bg-ac-rs/course-rs/projects-2025-2026/Mafija.git)  
Grana: `main`  
Heš kod komita: `31565d605391de059c56128639cb4a7ba7111ae2`  


Mafija je studentski projekat implementiran u jeziku C++ u okviru kursa Razvoj softvera na Matematičkom fakultetu, Univerziteta u Beogradu. Mafija je igra za više igrača u kojoj je svakom igraču dodeljena uloga. Igrači na osnovu uloga imaju odgovarajuće moći koje mogu koristiti i koje im mogu pomoći prilikom glasanja za izbacivanje drugih igrača.  

## Spisak korišćenih alata  

### 1. clang-tidy

Za statičku analizu projekta korišćen je alat `clang-tidy`.

**Preduslovi:**

* `clang-tidy`
* `run-clang-tidy`
* `cmake`
* `python3`

Reprodukcija rezultata vrši se pokretanjem skripte iz direktorijuma `clang-tidy/`:

```bash
cd clang-tidy
./clang-tidy.sh
```

Rezultati analize čuvaju se u fajlu `clang-tidy-report.txt`.

### 2. Cppcheck

Za statičku analizu projekta korišćen je alat `Cppcheck`.

**Preduslovi:**

* `cppcheck`
* `cmake`
* `python3`

Reprodukcija rezultata vrši se pokretanjem skripte iz direktorijuma `cppcheck/`:

```bash
cd cppcheck
./cppcheck.sh
```

Rezultati analize čuvaju se u fajlu `cppcheck-report.txt`.

### 3. Valgrind Memcheck

Za dinamičku analizu upravljanja memorijom korišćen je alat `Valgrind Memcheck`.

**Preduslovi:**

* `valgrind`
* `cmake`

Reprodukcija rezultata vrši se pokretanjem skripte iz direktorijuma `valgrind/`:

```bash
cd valgrind
./valgrind.sh
```

Rezultati analize čuvaju se u fajlu `full-report.txt`.


### 4. QTest

Za testiranje projekta korišćeni su postojeći `Catch2` testovi i dodatni testovi napisani pomoću `QTest` okvira. Za merenje pokrivenosti koda testovima korišćeni su `GCOV` i `LCOV`.

**Preduslovi:**

* `cmake`
* `ctest`
* `gcc`
* `g++`
* `gcov`
* `lcov`
* `genhtml`
* Qt6 sa Qt Test modulom

Za pokretanje samo dodatnih QTest testova:

```bash id="3n3q9j"
cd unit_tests
./run_qtests.sh
```

Rezultati QTest testova čuvaju se u fajlu `qtest_results.txt`.

Za pokretanje postojećih Catch2 testova i dodatnih QTest testova, kao i generisanje zajedničkog izveštaja o pokrivenosti:

```bash id="skggrm"
cd unit_tests
./run_tests_coverage.sh
```

HTML izveštaj o pokrivenosti generiše se u:

```text id="4dtznf"
coverage-report/html/index.html
```
### 5. libFuzzer

Za fuzz testiranje projekta korišćen je `libFuzzer`.

**Preduslovi:**

* `clang`
* `clang++`
* `cmake`

Reprodukcija rezultata za `Protocol` komponentu vrši se pokretanjem skripte:

```bash id="h54kbe"
cd libfuzzer
./fuzz_protocol.sh
```

Rezultati izvršavanja čuvaju se u fajlu `libfuzzer_report.txt`, dok se eventualni crash artefakti čuvaju u direktorijumu `crashes/`.

### 6. Lizard

Za analizu kompleksnosti izvornog koda korišćen je alat `Lizard`.

**Preduslovi:**

* `lizard`

Alat se može instalirati komandom:

```bash
pipx install lizard
```

Reprodukcija rezultata vrši se pokretanjem skripte iz direktorijuma `lizard/`:

```bash
cd lizard
./lizard.sh
```

Rezultati analize čuvaju se u fajlu `lizard_results.txt`.


## Spisak zaključaka

* Postojećih **40 Catch2 test slučajeva sa 252 provere** uspešno prolazi, dok su dodatni QTest testovi otkrili problem sa vrednošću `voting_for_nobody`, koja se može koristiti i kao korisničko ime stvarnog igrača.
* Kombinovani testni skup ostvaruje pokrivenost od **77,3% linija, 80,7% funkcija i 49,8% grana**.
* `clang-tidy` je pronašao potencijalno rizične konstrukcije, uključujući **sužavajuće numeričke konverzije, identične grane i lako zamenljive parametre**, kao i preporuke vezane za performanse, čitljivost i modernizaciju koda.
* Cppcheck je prijavio **32 nalaza iz kategorije performansi**, uglavnom vezanih za nepotrebno prosleđivanje i kopiranje objekata po vrednosti; nisu prijavljeni nalazi iz kategorija `warning` i `portability`.
* Valgrind Memcheck je tokom kompletne partije registrovao **izgubljenu memoriju povezanu sa objektima `CitizenRole`, `MafiaRole` i `DetectiveRole`**, kreiranim prilikom pokretanja igre.
* libFuzzer je za 60 sekundi izvršio **1.123.223 iteracija** nad komponentom `protocol` i nije pronašao ulaz koji izaziva pad fuzz targeta.
* Lizard je među **301 analiziranom funkcijom** izdvojio **10 funkcija** koje prelaze zadati prag kompleksnosti ili dužine; najveći `CCN` iznosi **21**.

Detaljna analiza rezultata i pojedinačnih nalaza nalazi se u fajlu [`ProjectAnalysisReport.md`](ProjectAnalysisReport.md).



