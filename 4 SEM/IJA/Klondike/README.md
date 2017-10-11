## IJA Klondike 90/90
**Zadání:** Navrhněte a implementujte aplikaci Pasians (Solitaire) Klondike, která vychází ze stejnojmenné karetní hry.
**Poznámka:** Zadání definuje podstatné vlastnosti aplikace, které musí být splněny. Předpokládá se, že detaily řešení si doplní řešitelské týmy. 

**Specifikace požadavků**

 - aplikace bude implementovat základní pravidla hry, která jsou uvedena   [zde](https://en.wikipedia.org/wiki/Klondike_%28solitaire%29) 
 - řešitelé mohou zvolit variantu uvedených pravidel či rozšíření      podle svého uvážení 
 - další požadavky 
	 - bude možné rozehrát až 4 hry       současně 
	 - pokud    bude rozehrána      pouze jedna hra, bude plocha grafického 
	   rozhraní    obsahovat       pouze tuto hru (jedno hrací plátno) 
	 - pokud	   	   bude          rozehrána       více než jedna hra, bude plocha
	   	   grafického rozhraní                rozdělena do 4 dlaždic, každá pro
	   	   jednu hru (hrací plátno)
	 - nevyužité     dlaždice nebudou nic obsahovat
	 - počet rozehraných her lze měnit za        běhu
 - kdykoliv je možné hru	   ukončit 
 - každá hra umožňuje operaci undo       (alespoň pět tahů)
 - rozehranou hru je možné uložit a znovu načíst         (historii tahů         	   	   	   	   pro undo není třeba ukládat) 
 - aplikaci nabízí možnost      nápovědy         	   	   	   	   tahu (na žádost zobrazí vhodným způsobem jaké tahy je možné          	   	     	   provést) 
 - součástí aplikace bude grafické uživatelské rozhraní  
	 - zobrazující hrací plátno (plátna) s kartami 
	 - ovládací prvky (uložit,  načíst, ukončit hru, založit novou hru, ...)
