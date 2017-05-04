# IPK PROJEKT 1
##### body: 8/10
##### Jan Grossmann - xgross09
##### Klient/server aplikace pro přenos souborů, s komunikací pomocí HTTP a RESTful API.

## **FTREST <- KLIENT**
	$ ./ftrest COMMAND REMOTE-PATH [LOCAL-PATH]

 - COMMAND:
	 - PUT
	 - GET
	 - DELETE
 - REMOTE-PATH:
	 - cesta k souboru nebo adresáři na serveru
 - LOCAL-PATH:
	 - cesta v lokální souborovém systému, povinné pro PUT

**PŘÍKLADY POUŽITÍ:**

	Vytvoření adresáře dir na serveru bežícím na lokálním počítači a portu 6677:
	$ ftrest mkd http://localhost:6677/USER/dir

	Nahrání souboru doc.pdf na serveru do adresáře dir:
	$ ftrest put http://localhost:6677/USER/dir/doc.pdf ~/doc.pdf

	Zjištění obsahu adresáře dir:
	$ ftrest lst http://localhost:6677/USER/dir

	Stažení souboru doc.pdf do lokálního adresáře:
	$ ftrest get http://localhost:6677/USER/dir/doc.pdf

	Odstranění souboru doc.pdf:
	$ ftrest del http://localhost:6677/USER/doc.pdf

	Odstranění adresáře dir:
	$ ftrest rmd http://localhost:6677/USER/dir

##IMPLEMENTACE FTREST

 - Ověření správnosti argumetnů, získáni hostname, portu, remote-path a případně local-path
 - Ze získaných argumentů vytvořím hlavičku HTTP requestu, hostname zpracuji pomoci **gethostbyname()**
 - Do struktury **sockaddr_in** se uloží údaje důležité pro navázání spojení
 - Zkusím vytvořit spojení se serverm pomocí **connect()**
 - Pokud se jedná o příkaz *put*, za hlavičku přidám načtená binární data a odešlu 1. paket pomocí **send()**
	 - Odesílám soubor po částech dokud se neodešle celý
 - Pokud se jedná o kterýkoliv jiný příkaz, odešlu pouze hlavičku
 - Počkám na zpracování requestu serverem a přijmu odpověd pomocí **recv()**
	 - Pokud se jedná o příkaz *get*, příjmám pakety dokud soubor neobsahuje požadovanou velikost z Content-Length
 - Zpracuji hlavičku HTTP response serveru a v případě chyby vypíšu hlášení na STDERR

## HLAVIČKA HTTP REQUEST
	 Date:            Timestamp klienta v době vytvoření požadavku
	 Accept:          Požadavaný typ obsahu pro odpověď
	 Accept-Encoding: Podporový způsob kódování dat (identity, gzip, deflate)
	 Content-Type:    MIME typ obsahu požadavku (pro PUT či POST)
	 Content-Length:  Délka obsahu požadavku (pro PUT či POST)


## ZÁKLADNÍ PŘÍKAZY

	del - smaže soubor určený REMOTE-PATH na serveru
	get - zkopíruje soubor z REMOTE-PATH do aktuálního lokálního adresáře či na místo určené pomocí LOCAL-PATH, je-li uvedeno
	put - zkopíruje soubor z LOCAL-PATH do adresáře REMOTE-PATH
	lst - vypíše obsah vzdáleného adresáře na standardní výstup
	mkd - vytvoří adresář specifikovaný v REMOTE-PATH na serveru, pokud REMOTE-PATH obsahuje neplatnou cestu, vrací chybu
	rmd - odstraní adresář specifikovaný V REMOTE-PATH ze serveru


## MOŽNÉ CHYBOVÉ HLÁŠENÍ


 - **Not a directory**
	 - když REMOTE-PATH ukazuje na soubor, ale je použita operace lst, rmd
 - **Directory not found**
	 - když REMOTE-PATH neukazuje na žádny existující objekt při použití operace lst, rmd
 - **Directory not empty**
	 - když REMOTE-PATH ukazuje na adresář, který není prázdný a je použita operace rmdir
 - **Already exists**
	 - když REMOTE-PATH ukazuje na adresář/soubor, který již existuje a je použita operace mkd či put.
 - **Not a file.**
	 - když REMOTE-PATH ukazuje na adresář, ale je použita operace del, get.
 - **File not found**
	 - když REMOTE-PATH neukazuje na žádny existující objekt při použití operace del, get
 - **User Account Not Found**
	 - pokud je operace nad neexistujícím uživatelem.
 - **Unknown error** pro ostatní chyby.


## **FTRESTD <- SERVER**

	$ ./ftrestd [-r ROOT-FOLDER] [-p PORT]

 - -r ROOT-FOLDER:
	 - specifikuje kořenový adresář, kde budou ukládány soubory pro jednotlivé uživatele, defaultní hodnota je aktuální
 - -p PORT:
	 - specifikuje port, na kterém bude server naslouchat, implicitně 6677


## IMPLEMENTACE FTRESTD
 - Ověření správnosti argumetnů, případně získáni portu a ROOT-FOLDER
 - Do struktury **sockaddr_in** se uloží údaje důležité pro navázání spojení
 - Vytvořím si socket nad kterým zavolám funkce **socket(), bind() a listen()**
 - Pomocí funkce **accept()** příjmu spojení, a spustím nekonečnou smyčku
 - Vyčistím si obsah bufferu do kterého budu příjmat data, počkám na přijetí hlavičky a tu zpracuji
 - Polde typu souboru a typu operace zjistím o jakou funkci z *lst, get, put, mkd, rmd, del* se jedná
 - Tuto funkci vykonám a vygeneruji hlavičku s informací o úspěchu a nebo neúspěchu a se všemi náležitostmi
	 - pokud funkcí byla operace *get*, odesílám soubor po částech, dokud se neodešle celý


## HLAVIČKA HTTP RESPONSE
	Date:             Timestamp serveru v době vyřízení požadavku.
	Content-Type:     Typ obsahu odpovědi podle MIME
	Content-Length:   Délka obsahu odpovědi
	Content-Encoding: Typ kódování obsahu (identity, gzip, deflate)

##STAVOVÉ KÓDY
	200 OK           - operace byla provedena úsopěšně
	404 Not Found    - objekt (soubor/adresář) v požadavku neexistuje
	400 Bad Request  - při přístupu k objektu jiného typu než uvedeného v požadaku
	490 Other        - vlastní implementace pro User account not found
	492 Other        - vlastní implementace pro Directory not empty



######2017-3-19
