
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, říjen 2014
**                              Radek Hranický, listopad 2015
**                              Radek Hranický, říjen 2016
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {
	for(int i = 0; i < HTSIZE; i++) {
		(*ptrht)[i] = NULL;			//inicializace kazdeho pointru v poli na NULL
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není,
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	if (*ptrht == NULL || (*ptrht)[hashCode(key)] == NULL) {    //vrati NULL pokud tabulka nebo radek tabulky neni innicializovany
		return NULL;
	}else {
		tHTItem* tempItem = (*ptrht)[hashCode(key)];           	//ulozeni pointru na prvni prvek radku
		while(tempItem != NULL) {                          		//pruchod radku od zacatku do konce
			if(tempItem->key == key) return tempItem;       	//pokud je pozadovany prvek nalezen tak ho vratim
			tempItem = tempItem->ptrnext;
		}
		return NULL;                                        	//nebo vratim NULL
	}
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {
	if (*ptrht == NULL) {
        ;
    }else {
	        tHTItem* tempItem = htSearch(ptrht, key);          		//pokus o nalezeni itemu v tabulce
        if (tempItem == NULL) {										//pokud v tabulce jeste neni
			tHTItem* insertItem = (tHTItem*)malloc(sizeof(tHTItem));//vytvoreni noveho itemu
            if ((*ptrht)[hashCode(key)] == NULL) {               	//pokud je odpovidajici radek prazdny
                insertItem->key = key;								//pridani atributu itemu a vlozeni na zacatek radku
				insertItem->data = data;
                insertItem->ptrnext = NULL;
                (*ptrht)[hashCode(key)] = insertItem;
            }else {                                        			//na odpovidajicim radku uz nejake itemy jsou
                insertItem->key = key;								//pridani atributu itemu
				insertItem->data = data;
                insertItem->ptrnext = (*ptrht)[hashCode(key)];      //nastaveni pointru itemu na aktualni zacatek radku
                (*ptrht)[hashCode(key)] = insertItem;            	//prenastaveni pointru na radek tabulky na vkladany item
            }
        }else {
            tempItem->data = data;									//item uz v tabulce takze se aktualizuji data
        }
    }
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {
	if (*ptrht == NULL) {
        return NULL;
    }else {
        tHTItem* tempItem = htSearch(ptrht, key);				//vyhledani itemu
        if (tempItem != NULL) {
			return &(tempItem->data);							//pokud byl nalezen, vrací funkce ukazatel na položku
        }else {
            return NULL;
        }
    }
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {
	if (*ptrht == NULL) {
		;
	}else {
		tHTItem *prevItem = (*ptrht)[hashCode(key)];
		tHTItem *tempItem = (*ptrht)[hashCode(key)];					//nalezeni prislusneho radku na kterem se vyskytuje item ke smazani

		while(tempItem != NULL) {										//pokud takovy radek existuje, existuje nejspise i takovy item
			if(tempItem->key == key) {
				if(tempItem == (*ptrht)[hashCode(key)]) {				//pokud je item ke smazani prvni v radku
					(*ptrht)[hashCode(key)] =tempItem->ptrnext;			//ukazatel na zacatek radku se posune za nej
				}else {
					prevItem->ptrnext = tempItem->ptrnext;				//jinak se ukazatel na dalsi item predchazejiciho itemu nastavi na ukazatel na dalsi item itemu ke smazani
				}
				free(tempItem);
			}else {
				prevItem = tempItem;									//posouvani pomocnych ukazatelu na item
				tempItem = tempItem->ptrnext;
			}
		}
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
	for (int i = 0; i< HTSIZE; i++) {									//cyklus pro pruchod celym polem
        tHTItem* tempItem = (*ptrht)[i];								//pomocny item pro pruchod radku pole
        tHTItem* delItem;
        while(tempItem != NULL) {
            delItem = tempItem;											//item po itemu se postupne uvolni
            tempItem = tempItem->ptrnext;
            free(delItem);
        }
        (*ptrht)[i] = NULL;												//ukazatel na pole se nastavi na NULL
    }
}
