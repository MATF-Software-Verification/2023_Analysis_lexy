# Izveštaj analize projekta

## 1. Clang Static Analyzer  

Cilj statičke analize je provera izvornog koda projekta u cilju otkrivanjna potencijalnih problema u kodu bez izvršavanja programa. Projekat je prvo konfigurisan pomoću CMake-a, pri čemu su kao C i C++ kompajleri eksplicitno postavljeni `clang` i `clang++`. Nakon uspešne konfiguracije, projekat je izgradjen pomoću `cmake --build`, pri čemu je build pokrenut kroz `scan-build`. Na taj način je Clang Static Analyzer mogao da izvrši statičku analizu izvornog koda tokom procesa kompajliranja.
Uključene provere: core, unix, security, deadcode. 

### Zaključak  
Clang Static Analyzer nije pronašao probleme u okviru navedenih provera. Clang kompajler prijavljuje jedan warning u fajlu GUI/mainwindow.cpp koji se odnosi na switch naredbu koja ne proverava sve definisane enum vrednosti, što je prepoznato kao potencijalni problem. Detaljni isps se nalazi u dataoteci clang_static_analzyer/izlaz.txt. 
