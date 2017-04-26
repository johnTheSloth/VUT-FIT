	
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Bohuslav Křena, říjen 2016
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu, 
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem, 
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu, 
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/

    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
	
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	
    tDLElemPtr pom;

    while(L->First != NULL) {
        pom = L->First;             //ulozeni itemu
        L->First = L->First->rptr;  //posunuti o item v listu
        free(pom);                  //uvolneni itemu
    }
    L->Act = NULL;
    L->Last = NULL;

}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    tDLElemPtr pom = malloc(sizeof(struct tDLElem));
    if(pom != NULL) {
        pom->data = val;            //nastaveni hodnoty itemu
        pom->lptr = NULL;           //v listu pred itemem nic neni, proto NULL
        pom->rptr = L->First;       //nastaveni pointru za item na prvni item listu
        if(L->Last == NULL) {       //pokud byl list prazdny
            L->Last = pom;
        }else {
            L->First->lptr = pom; 
        }
        L->First = pom;             //nastaveni ->First na novy item
    }else {
        DLError();
    }

}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	
    tDLElemPtr pom = malloc(sizeof(struct tDLElem));
    if(pom != NULL) {
        pom->data = val;
        pom->lptr = L->Last;
        pom->rptr = NULL;
        if(L->Last != NULL) {               //pokud existuje posledni item listu
            L->Last->rptr = pom;            //nastavi se jeho ->rptr na pom item
            L->Last = L->Last->rptr;        //Last item se prenastavi na pom item
        }else {
            L->First = pom;                 //pokud je list prazdny prvni i posledni prvek se nastavi na pom item
            L->Last = pom;
        }

    }else {
        DLError();
    }

}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
    L->Act = L->First;

}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
    L->Act = L->Last;

}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

    if((L->First == NULL) && (L->Last == NULL)){        //v idealnim pripade ma prazdny seznam ukazatel FIRST i LAST nastaveny na NULL
        DLError();
    }else {
        *val = L->First->data;                          //predani hodnoty prvniho itemu
    }

}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if((L->First == NULL) && (L->Last == NULL)){        //v idealnim pripade ma prazdny seznam ukazatel FIRST i LAST nastaveny na NULL
        DLError();
    }else {
        *val = L->Last->data;                           //predani hodnoty posledniho itemu
    }
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/

    if((L->First == NULL) && (L->Last == NULL)){
        ;
    }else {
        tDLElemPtr pom = L->First;          //pomocny pointer ukazuje na 1. item listu
        if(L->First == L->Last) {           //list obsahuje jen 1 item
            L->First = NULL;
            L->Last = NULL;
            L->Act = NULL;
        }else {
            if(L->Act == L->First) {        //pokud je aktivni prvek prvni, aktivita se ztraci
                L->Act = NULL;
            }
            L->First = L->First->rptr;      //first ukazatel listu posunu o 1 "doprava"
            L->First->lptr = NULL;          //ukazatel first itemu lptr nastavim na NULL protoze prvek pred nim je zruseny
        }
        free(pom);
    }

}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 

    if((L->First == NULL) && (L->Last == NULL)){
        ;
    }else {
        tDLElemPtr pom = L->Last;          //pomocny pointer ukazuje na posledni item listu
        if(L->First == L->Last) {           //list obsahuje jen 1 item
            L->First = NULL;
            L->Last = NULL;
            L->Act = NULL;
        }else {
            if(L->Act == L->Last) {         //pokud je aktivni prvek posledni, aktivita se ztraci
                L->Act = NULL;
            }
            L->Last = L->Last->lptr;        //last ukazatel listu posunu o 1 "doleva"
            L->Last->rptr = NULL;           //ukazatel last itemu rptr nastavim na NULL protoze prvek za nim je zruseny
        }
        free(pom);
    }

}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
	
    if(L->Act == L->Last || L->Act == NULL) {
        ;                                           // nic se nedeje
    }else {
        tDLElemPtr pom = L->Act->rptr;              //pomocny pointer ukazuje na ruseny item
        if(L->Act->rptr != L->Last) {               //pokud je aktivni item predposledni
            L->Act->rptr = L->Act->rptr->rptr;      //"vyrazeni" ruseneho itemu z listu
            L->Act->rptr->lptr = L->Act;            //"vyrazeni" ruseneho itemu i pri pruchodu listem od zadu dopredu
        }else {
            L->Last = L->Act;
            L->Last->rptr = NULL;                   //zahozeni rptr protoze item je posledni v listu
        }
        free(pom);
    }

}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
	
    if(L->Act == NULL || L->Act == L->First) {
        ;
    }else {
        tDLElemPtr pom = L->Act->lptr;              //pomocny pointer ukazuje na ruseny item
        if(L->Act->lptr != L->First) {              //pokud je aktivni item jiny nez druhy
            L->Act->lptr = L->Act->lptr->lptr;      //"vyrazeni" ruseneho itemu z listu
            L->Act->lptr->rptr = L->Act;            //"vyrazeni" ruseneho itemu i pri pruchodu listem zepredu dozadu
        }else {                                     //pokud je aktivni item druhy
            L->First = L->Act;
            L->First->lptr = NULL;                  //zahozeni lptr protoze item je prvni v listu
        }
        free(pom);
    }

}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	
    if(L->Act != NULL) {
        tDLElemPtr pom = malloc(sizeof(struct tDLElem));
        if(pom == NULL) {                           //nedostatek mista pro malloc
            DLError();
        }else {
            pom->data = val;                        //prirazeni hodnoty do ->data
            pom->lptr = L->Act;                     //nastaveni lptr na predchozi, tedy Aktivni item
            pom->rptr = L->Act->rptr;               //nastaveni rptr na item za Aktivnim itemem
            L->Act->rptr = pom;                     //vlozeni pomocneho itemu do listu za aktivni item
            if(L->Act == L->Last) {                 //pokud byl aktivni item posledni, 
                L->Last = pom;                      //nastaveni ->Last na nove vlozeny item
            }else {
                L->Act->rptr->rptr->lptr = pom;     //nastaveni itemu za pridanym pom itemem aby na nej odkazoval pomoci lptr
            }
        }
    }

}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	
    if(L->Act != NULL) {
        tDLElemPtr pom = malloc(sizeof(struct tDLElem));
        if(pom == NULL) {                           //nedostatek mista pro malloc
            DLError();
        }else {
            pom->data = val;                        //prirazeni hodnoty do ->data
            pom->rptr = L->Act;                     //nastaveni rptr na dalsi, tedy Aktivni item
            pom->lptr = L->Act->lptr;               //nastaveni lptr na item pred Aktivnim itemem
            L->Act->lptr = pom;                     //vlozeni pomocneho itemu do listu pred aktivni item
            if(L->Act == L->First) {                //pokud byl aktivni item prvni, 
                L->First = pom;                     //nastaveni ->First na nove vlozeny item
            }else {
                L->Act->lptr->lptr->rptr = pom;     //nastaveni itemu za pridanym pom itemem aby na nej odkazoval pomoci lptr
            }
        }
    }
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/

    if(L->Act == NULL) {
        DLError();
    }else {
        *val = L->Act->data;                        //prirazeni hodnoty ->data aktivniho prvku do val
    }

}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	
    if(L->Act != NULL) {
        L->Act->data = val;                         //prirazeni hodnoty val do ->data
    }

}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	
    if(L->Act != NULL) {
        (L->Act == L->Last) ? (L->Act = NULL) :(L->Act=L->Act->rptr);       //explicitne radsi urcim kdy se stane seznam neaktivnim, a to pokud Aktualni prvek je Posledni
    }

}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/

    (L->Act == L->First) ? (L->Act = NULL) :(L->Act=L->Act->lptr);          //explicitne radsi urcim kdy se stane seznam neaktivnim, a to pokud Aktualni prvek je Prvni

}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/

    return (L->Act != NULL);	

}

/* Konec c206.c*/
