## Compilanje

Projekt zahtjeva SFML biblioteku za C++ \
U compile.bat je potrebno unijeti putanju do SFML biblioteke.

## Pokretanje

Program prima jedan command-line argument koji predstavlja putanju do .tsp datoteke u kojoj se nalazi skup točaka koje program parsira i generira graf. 

primjer:
```powershell
...> .\build\main.exe .\res\point131.tsp
```
Alternativno moguće je unijeti putanju i nakon pokretanja. 

primjer:
```powershell
...> .\build\main.exe 
Path: (čeka unos)
```
## Ispis

Program ispisuje liniju svaki put kad nađe novi najbolji put, nakon izvršenja zadnje iteracije otvara prozor u kojem će biti vizualizacija tog dobivenog puta. Vizualizacija je interaktivna (zoom, pomicanje mišem). Također je moguće raspršiti i približiti sve točke kako bi se stisnuti grafovi mogli bolje pregledati (tipke gore i dolje)
