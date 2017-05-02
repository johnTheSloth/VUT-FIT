/**
* @file        student.cpp
* @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
* @author      Kamil Behun, VUT FIT Brno, ibehun@fit.vutbr.cz
* @date        13.03.2017
*
* @brief       Deklarace funkci studentu. DOPLNUJI STUDENTI
*
*/

#include "base.h"
#include "student.h"
#include "globals.h"

/**
* @brief Vraci barvu pixelu z pozice [x, y]
* @param[in] x X souradnice pixelu
* @param[in] y Y souradnice pixelu
* @return Barva pixelu na pozici [x, y] ve formatu RGBA
*/
RGBA getPixel(int x, int y)
{
	if (x >= width || y >= height || x < 0 || y < 0) {
		IZG_ERROR("Pristup do framebufferu mimo hranice okna\n");
	}
	return framebuffer[y * width + x];
}

/**
* @brief Nastavi barvu pixelu na pozici [x, y]
* @param[in] x X souradnice pixelu
* @param[in] y Y souradnice pixelu
* @param[in] color Barva pixelu ve formatu RGBA
*/
void putPixel(int x, int y, RGBA color)
{
	if (x >= width || y >= height || x < 0 || y < 0) {
		IZG_ERROR("Pristup do framebufferu mimo hranice okna\n");
	}
	framebuffer[y * width + x] = color;
}

/**
* @brief Vykresli usecku se souradnicemi [x1, y1] a [x2, y2]
* @param[in] x1 X souradnice 1. bodu usecky
* @param[in] y1 Y souradnice 1. bodu usecky
* @param[in] x2 X souradnice 2. bodu usecky
* @param[in] y2 Y souradnice 2. bodu usecky
* @param[in] color Barva pixelu usecky ve formatu RGBA
* @param[in] arrow Priznak pro vykresleni sipky (orientace hrany)
*/
void drawLine(int x1, int y1, int x2, int y2, RGBA color, bool arrow = false)
{

	if (arrow) {
		// Sipka na konci hrany
		double vx1 = x2 - x1;
		double vy1 = y2 - y1;
		double length = sqrt(vx1 * vx1 + vy1 * vy1);
		double vx1N = vx1 / length;
		double vy1N = vy1 / length;
		double vx1NN = -vy1N;
		double vy1NN = vx1N;
		int w = 3;
		int h = 10;
		int xT = x2 + (int)(w * vx1NN - h * vx1N);
		int yT = y2 + (int)(w * vy1NN - h * vy1N);
		int xB = x2 - (int)(w * vx1NN - h * vx1N);
		int yB = y2 - (int)(w * vy1NN - h * vy1N);
		pinedaTriangle(Point(x2, y2), Point(xT, yT), Point(xB, yB), color, color, false);
	}

	bool steep = abs(y2 - y1) > abs(x2 - x1);

	if (steep) {
		SWAP(x1, y1);
		SWAP(x2, y2);
	}

	if (x1 > x2) {
		SWAP(x1, x2);
		SWAP(y1, y2);
	}

	const int dx = x2 - x1, dy = abs(y2 - y1);
	const int P1 = 2 * dy, P2 = P1 - 2 * dx;
	int P = 2 * dy - dx;
	int y = y1;
	int ystep = 1;
	if (y1 > y2) ystep = -1;

	for (int x = x1; x <= x2; x++) {
		if (steep) {
			if (y >= 0 && y < width && x >= 0 && x < height) {
				putPixel(y, x, color);
			}
		}
		else {
			if (x >= 0 && x < width && y >= 0 && y < height) {
				putPixel(x, y, color);
			}
		}

		if (P >= 0) {
			P += P2;
			y += ystep;
		}
		else {
			P += P1;
		}
	}
}

/**
* @brief Vyplni a vykresli trojuhelnik
* @param[in] v1 Prvni bod trojuhelniku
* @param[in] v2 Druhy bod trojuhelniku
* @param[in] v3 Treti bod trojuhelniku
* @param[in] color1 Barva vyplne trojuhelniku
* @param[in] color2 Barva hranice trojuhelniku
* @param[in] arrow Priznak pro vykresleni sipky (orientace hrany)
*
* SPOLECNY UKOL. Doplnuji studenti se cvicicim.
*/
void pinedaTriangle(const Point &v1, const Point &v2, const Point &v3, const RGBA &color1, const RGBA &color2, bool arrow)
{
	// Nalezeni obalky (minX, maxX), (minY, maxY) trojuhleniku.
	Point min(MIN(v1.x, MIN(v2.x, v3.x)), MIN(v1.y, MIN(v2.y, v3.y)));
	Point max(MAX(v1.x, MAX(v2.x, v3.x)), MAX(v1.y, MAX(v2.y, v3.y)));
	//////// DOPLNTE KOD /////////

	// Oriznuti obalky (minX, maxX, minY, maxY) trojuhleniku podle rozmeru okna.

	min.x = MIN(width - 1, MAX(0, min.x));
	max.x = MIN(width - 1, MAX(0, max.x));

	min.y = MIN(height - 1, MAX(0, min.y));
	max.y = MIN(height - 1, MAX(0, max.y));


	// Spocitani parametru hranove funkce (A, B, C) pro kazdou hranu.
	// Hranova funkce je obecna rovnice primky Ax + By + C = 0.
	// Primku prochazejici body (x1, y1) a (x2, y2) urcime jako
	// (y1 - y2) * x + (x2 - x1) * y + x1 * y2 - x2 * y1 = 0.

	int a12 = v1.y - v2.y;
	int b12 = v2.x - v1.x;
	int c12 = v1.x*v2.y - v2.x*v1.y;

	int a23 = v2.y - v3.y;
	int b23 = v3.x - v2.x;
	int c23 = v2.x*v3.y - v3.x*v2.y;

	int a31 = v3.y - v1.y;
	int b31 = v1.x - v3.x;
	int c31 = v3.x*v1.y - v1.x*v3.y;


	// Vyplnovani: Cyklus pres vsechny body (x, y) v obdelniku (minX, minY), (maxX, maxY).
	// Pro aktualizaci hodnot hranove funkce v bode P (x +/- 1, y) nebo P (x, y +/- 1)
	// vyuzijte hodnoty hranove funkce E (x, y) z bodu P (x, y).

	int e12 = a12*min.x + b12*min.y + c12;
	int e23 = a23*min.x + b23*min.y + c23;
	int e31 = a31*min.x + b31*min.y + c31;


	for (int y = min.y; y < max.y; y++) {
		int e12L = e12;
		int e23L = e23;
		int e31L = e31;

		for (int x = min.x; x < max.x; x++) {
			if (e12L >= 0 && e23L >= 0 && e31L >= 0) {
				putPixel(x, y, color1);
			}

			e12L += a12;
			e23L += a23;
			e31L += a31;
		}

		e12 += b12;
		e23 += b23;
		e31 += b31;
	}

	// Prekresleni hranic trojuhelniku barvou color2.
	drawLine(v1.x, v1.y, v2.x, v2.y, color2, arrow);
	drawLine(v2.x, v2.y, v3.x, v3.y, color2, arrow);
	drawLine(v3.x, v3.y, v1.x, v1.y, color2, arrow);
}

/**
* @brief Vyplni a vykresli polygon
* @param[in] points Pole bodu polygonu
* @param[in] size Pocet bodu polygonu (velikost pole "points")
* @param[in] color1 Barva vyplne polygonu
* @param[in] color2 Barva hranice polygonu
*
* SAMOSTATNY BODOVANY UKOL. Doplnuji pouze studenti.
*/
void pinedaPolygon(const Point *points, const int size, const RGBA &color1, const RGBA &color2)
{
	// Pri praci muzete vyuzit pro vas predpripravene datove typy z base.h., napriklad:
	//
	//      Pro ukladani parametru hranovych funkci muzete vyuzit prichystany vektor parametru hranovych funkci "EdgeParams":
	//
	//          EdgeParams edgeParams(size)                         // Vytvorite vektor (pole) "edgeParams" parametru hranovych funkci o velikosti "size".
	//          edgeParams[i].a, edgeParams[i].b, edgeParams[i].c   // Pristup k parametrum (a, b, c) hranove funkce v poli "edgeParams" na indexu "i".
	//
	//      Pro ukladani hodnot hranovych funkci muzete vyuzit prichystany vektor hodnot hranovych funkci "EdgeFncValues":
	//
	//          EdgeFncValues edgeFncValues(size)                   // Vytvorite vektor (pole) "edgeFncValues" hodnot hranovych funkci o velikosti "size".
	//          edgeFncValues[i]                                    // Pristup k hodnote hranove funkce v poli "edgeFncValues" na indexu "i".
	//

	// Nalezeni obalky (minX, maxX), (minY, maxY) polygonu.

	Point min(points[0].x, points[0].y);
	Point max(points[0].x, points[0].y);

	for (int i = 0; i <= size; i++) {
		if (points[i].x < min.x) {
			min.x = points[i].x;
		}
		if (points[i].x > max.x) {
			max.x = points[i].x;
		}

		if (points[i].y < min.y) {
			min.y = points[i].y;
		}
		if (points[i].y > max.y) {
			max.y = points[i].y;
		}

	}

	// Oriznuti obalky (minX, maxX), (minY, maxY) polygonu podle rozmeru okna

	if (min.x < 0) {
		min.x = 0;
	}
	if (min.y < 0) {
		min.y = 0;
	}
	if (max.x >= width) {
		max.x = width - 1;
	}
	if (max.y >= height) {
		max.y = height - 1;
	}

	// Spocitani parametru hranove funkce pro kazdou hranu.
	// Hranova funkce je obecna rovnice primky Ax + By + C = 0.
	// Primku prochazejici body (x1, y1) a (x2, y2) urcime jako
	// (y1 - y2) * x + (x2 - x1) * y + x1 * y2 - x2 * y1 = 0.

	EdgeParams params(size);

	for (int i = 0; i < size; i++) {
		int index = (i + 1) % size;
		params[i].a = points[i].y - points[index].y;
		params[i].b = points[index].x - points[i].x;
		params[i].c = points[i].x * points[index].y - points[index].x * points[i].y;

	}
	// Test konvexnosti polygonu    

	for (int i = 0; i < size; i++) {
		int index = (i + 2) % size;
		int test = params[i].a * points[index].x + params[i].b * points[index].y + params[i].c;
		if (test < 0) {
			printf("nekonvexni");
			return;
		}
	}

	// Vyplnovani: Cyklus pres vsechny body (x, y) v obdelniku (minX, minY), (maxX, maxY).
	// Pro aktualizaci hodnot hranove funkce v bode P (x +/- 1, y) nebo P (x, y +/- 1)
	// vyuzijte hodnoty hranove funkce E (x, y) z bodu P (x, y) */

	EdgeFncValues temp(size);

	for (int i = 0; i < size; i++) {
		temp[i] = params[i].a * min.x + params[i].b * min.y + params[i].c;
	}


	for (int y = min.y; y <= max.y; y++) {
		EdgeFncValues temp2 = temp;

		for (int x = min.x; x <= max.x; x++) {
			bool in = true;
			for (int j = 0; j < size; j++) {
				if (temp2[j] < 0) {
					in = false;
				}
			}
			if (in) {
				putPixel(x, y, color1);
			}
			for (int i = 0; i < size; i++) {
				temp2[i] += params[i].a;
			}

		}

		for (int j = 0; j < size; j++) {
			temp[j] += params[j].b;
		}
	}


	// Prekresleni hranic polygonu barvou color2.
	for (int i = 0; i < size; i++) {
		drawLine(points[i].x, points[i].y, points[(i + 1) % size].x, points[(i + 1) % size].y, color2/*, true*/);
	}
}
