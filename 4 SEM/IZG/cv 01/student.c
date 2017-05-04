/******************************************************************************
 * Laborator 01 - Zaklady pocitacove grafiky - IZG
 * ihulik@fit.vutbr.cz
 *
 * $Id: $
 * 
 * Popis: Hlavicky funkci pro funkce studentu
 *
 * Opravy a modifikace:
 * -
 */

#include "student.h"
#include "globals.h"

#include <time.h>

/******************************************************************************
 ******************************************************************************
 Funkce vraci pixel z pozice x, y. Je nutne hlidat frame_bufferu, pokud 
 je dana souradnice mimo hranice, funkce vraci barvu (0, 0, 0).
 Ukol za 0.5 bodu */
S_RGBA getPixel(int x, int y)
{
	
	if (x < width && x >= 0 && y < height && y >= 0)
	{
		return frame_buffer[(y*width) + x];
	}
	
	return makeBlackColor(); //vraci barvu (0, 0, 0)
}

/******************************************************************************
 ******************************************************************************
 Funkce vlozi pixel na pozici x, y. Je nutne hlidat frame_bufferu, pokud 
 je dana souradnice mimo hranice, funkce neprovadi zadnou zmenu.
 Ukol za 0.5 bodu */
void putPixel(int x, int y, S_RGBA color)
{
	if (x < width && x >= 0 && y < height && y >= 0)
	{
		frame_buffer[(y*width) + x] = color;
	}
}

/******************************************************************************
 ******************************************************************************
 Funkce prevadi obrazek na odstiny sedi. Vyuziva funkce GetPixel a PutPixel.
 Ukol za 1 bod. */
void grayScale()
{
	S_RGBA color;
	int x, y, intensity;

	for (x = 0 ; x < width; x++) {
		for (y = 0; y < height; y++)
		{
			color = getPixel(x, y);
			intensity = ROUND(0.299*color.red + 0.587*color.green + 0.114*color.blue);
			color.red = intensity;
			color.green = intensity;
			color.blue = intensity;
			putPixel(x, y, color);
		}
	}
}

/******************************************************************************
 ******************************************************************************
 Funkce prevadi obrazek na cernobily pomoci algoritmu distribuce chyby.
 Ukol za 1 bod */
void errorDistribution()
{
	S_RGBA color;
	int x, y, error = 0, intensity = 0;
	/* Prevedeme obrazek na grayscale */
	grayScale();
	/* Projdeme vsechny pixely obrazku */
	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			color = getPixel(x, y);
			intensity = color.red;
			if (intensity <= 111)
				color = COLOR_BLACK;
			else
				color = COLOR_WHITE;
			error = intensity - color.red;
			putPixel(x, y, color);

			if (x+1 < width) addError(x + 1, y, error * 3.0 / 8.0);
			if (y + 1 < height) addError(x, y + 1, error * 3.0 / 8.0);
			if (x + 1 < width && y + 1 < height) addError(x + 1, y + 1, error * 1.0 / 4.0);
		}
	}
}

void addError(int x, int y, int err) {
	S_RGBA color = getPixel(x, y);
	int temp = ROUND(color.red + err);

	if (temp > 255) temp = 255;
	if (temp < 0) temp = 0;

	color.red = temp;
	color.green = temp;
	color.blue = temp;

	putPixel(x, y, color);
}


/******************************************************************************
 ******************************************************************************
 Funkce prevadi obrazek na cernobily pomoci metody prahovani.
 Demonstracni funkce */
void thresholding(int Threshold)
{

	/* Prevedeme obrazek na grayscale */
	grayScale();

	/* Projdeme vsechny pixely obrazku */
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			/* Nacteme soucasnou barvu */
			S_RGBA color = getPixel(x, y);

			/* Porovname hodnotu cervene barevne slozky s prahem.
			   Muzeme vyuzit jakoukoli slozku (R, G, B), protoze
			   obrazek je sedotonovy, takze R=G=B */
			if (color.red > Threshold)
				putPixel(x, y, COLOR_WHITE);
			else
				putPixel(x, y, COLOR_BLACK);
		}
}

/******************************************************************************
 ******************************************************************************
 Funkce prevadi obrazek na cernobily pomoci nahodneho rozptyleni. 
 Vyuziva funkce GetPixel, PutPixel a GrayScale.
 Demonstracni funkce. */
void randomDithering()
{

	/* Prevedeme obrazek na grayscale */
	grayScale();

	/* Inicializace generatoru pseudonahodnych cisel */
	srand((unsigned int)time(NULL));

	/* Projdeme vsechny pixely obrazku */
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			/* Nacteme soucasnou barvu */
			S_RGBA color = getPixel(x, y);
			
			/* Porovname hodnotu cervene barevne slozky s nahodnym prahem.
			   Muzeme vyuzit jakoukoli slozku (R, G, B), protoze
			   obrazek je sedotonovy, takze R=G=B */
			if (color.red > rand()%255)
			{
				putPixel(x, y, COLOR_WHITE);
			}
			else
				putPixel(x, y, COLOR_BLACK);
		}
}
/*****************************************************************************/
/*****************************************************************************/