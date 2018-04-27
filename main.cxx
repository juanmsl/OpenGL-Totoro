/***********************************************************************
	INFORMATION
***********************************************************************/
/*
	glBegin(<PARAMETER>)
	https://www3.ntu.edu.sg/home/ehchua/programming/opengl/images/GL_GeometricPrimitives.png
*/

/*
	Juan Manuel Sánchez Lozano
	Universidad Pontificia Javeriana
	Introducción a la computación grafica
*/

/*
	Para aumentar el efecto de puntillismo, puede variar el valor
	de 'GRANULADO' en la linea 48 de este archivo, puede variarla
	entre 1 y 3 recomendadamente.

	El valor de 'SECTION' en la linea 49, controla el tamaño del
	dibujo, puede variarlo entre 20 y 80 recomendadamente, pero
	el tamaño por defecto y recomendado es 56.
*/

/***********************************************************************
	LIBRARIES - DEFINES - UTILITY FUNCTIONS
***********************************************************************/

#include <iostream>
#include <string>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <GL/freeglut.h>
#include <GL/gl.h>

#define COLOR_FONDO 		HSL(122, 11, 47, true)
#define COLOR_NARIZ 		HSL(0, 0, 20)
#define COLOR_SOMBRA 		HSL(127, 11, 33)
#define COLOR_SOMBRA_CLARA 	HSL(127, 11, 70)
#define COLOR_CONTORNO	 	HSL(120, 19, 8)
#define COLOR_BLANCO 		HSL(0, 0, 96)

#define GROSOR_CONTORNO	glPointSize(3)
#define GROSOR_MEDIO 	glPointSize(2)
#define GROSOR_SOMBRA 	glPointSize(1)

#define GRANULADO 	1 	// MIN 1 - MAX 3
#define SECTION 	56	// MIN 20 - MAX 80 - (Recomendado 56)
#define WIDTH 		SECTION * 27
#define HEIGHT 		SECTION * 11

void HSL(float H, float S, float L, bool fondo = false);
void RGB(float R, float G, float B, bool fondo = false);
void p(float x, float y);
void circle(float x, float y, float radius, bool filled = false);
void circleIn(float x, float y, float radius, float g1, float g2);
void elipse(float x, float y, float radius_x, float radius_y);
float value_of_curve(float x, float y, float H, float K, float value);
void semi_curve(float x, float y, float H, float K);
void curve(float x_i, float y_i, float x_f, float y_f, float H, float K);
void line(float x_i, float y_i, float x_f, float y_f);

float mod(float a, float b) {
	int res = a/b;
	float md = (float)res * b;
	float m = a - md;
	return m;
}

/***********************************************************************
	COLOR FUNCTIONS
***********************************************************************/

/*
	Algorithm implemented from the formula in
	http://www.rapidtables.com/convert/color/hsl-to-rgb.htm

	H: Hue [0 - 360]
	S: Saturation [0 - 100]
	L: Lightness [0 - 100]

	Colour wheel for HSL: https://bitsofco.de/img/blog/26/hsl.png
*/
void HSL(float H, float S, float L, bool fondo) {
	H /= 60;
	S /= 100;
	L /= 100;

	float C = (1 - fabs(2 * L - 1)) * S;
	float X = C * (float)(1 - fabs(mod(H, 2) - 1));
	float m = L - C/2;

	float R, G, B;

	int quadrant = H;

	switch(quadrant) {
		case 0: R=C; G=X; B=0; break;
		case 1: R=X; G=C; B=0; break;
		case 2: R=0; G=C; B=X; break;
		case 3: R=0; G=X; B=C; break;
		case 4: R=X; G=0; B=C; break;
		case 5: R=C; G=0; B=X; break;
	}

	R = (R + m) * 255;
	G = (G + m) * 255;
	B = (B + m) * 255;

	RGB(R, G, B, fondo);
}

void RGB(float R, float G, float B, bool fondo) {
	if(fondo) glClearColor(R/255.0, G/255.0, B/255.0, 1.0);
	else glColor4f(R/255.0, G/255.0, B/255.0, 1.0);
}

/***********************************************************************
	SHAPE FUNCTIONS
***********************************************************************/

void p(float x, float y) {
	if((rand()%100)%GRANULADO == 0)
		glVertex2f(x,y);
}

void circle(float x, float y, float radius, bool filled) {
	glBegin(GL_POINTS);
		for(float i = 0; i < 360; i++){
			float grd = i * 2.0 * 3.1416 / 360;
			for(float j = 0; j < radius && filled; j++) {
				p(x + j * cos(i), y + j * sin(i));
			}
			p(x + radius * cos(i), y + radius * sin(i));
		}
	glEnd();
}

void circleIn(float x, float y, float radius, float g1, float g2) {
	glBegin(GL_POINTS);
		for(float i = g1; i < g2; i++){
			float grd = i * 2.0 * 3.1416 / 360;
			p(x + radius * cos(grd), y + radius * sin(grd));
		}
	glEnd();
}

void elipse(float x, float y, float radius_x, float radius_y) {
	glBegin(GL_POINTS);
		for(float i = 0; i < 360; i++){
			HSL(120, 100, 50);
			float grd = i * 2.0 * 3.1416 / 360;
			p(x + radius_x * cos(i), y + radius_y * sin(i));
		}
	glEnd();
}

float value_of_curve(float x, float y, float H, float K, float value) {
	float P = ((x - H)*(x - H)) / (4 * (y - K));

	return ((4 * P * K) + ((value - H) * (value - H))) / (4 * P);
}

void semi_curve(float x, float y, float H, float K) {
	/*
		Equation of the parabola with vertex (H, K) and passing through the point (x, y)

		(x - H)² = 4P (y - K)

		P is cleared and replaced for the function y(x)

		P = (x - H)² / 4(y - K)

		y(x) = (4PK + (x - H)²) / 4P
	*/
	float P = ((x - H)*(x - H)) / (4 * (y - K));

	for(float X = (x < H ? x : H); X <= (x > H ? x : H); X++) {
		float Y = ((4 * P * K) + ((X - H) * (X - H))) / (4 * P);
		p(X, Y);
	}
}

void curve(float x_i, float y_i, float x_f, float y_f, float H, float K) {
	semi_curve(x_i, y_i, H, K);
	semi_curve(x_f, y_f, H, K);
}

void line(float x_i, float y_i, float x_f, float y_f) {
	float m = (y_f - y_i) / (x_f - x_i);

	float x_Mayor = (x_i > x_f ? x_i : x_f);
	float x_Menor = (x_i < x_f ? x_i : x_f);
	float y_Mayor = (y_i > y_f ? y_i : y_f);
	float y_Menor = (y_i < y_f ? y_i : y_f);

	if(x_i == x_f) {
		for(float Y = y_i ; Y <= y_f; Y++) {
			p(x_i, Y);
		}
	} else if(y_i == y_f) {
		for(float X = x_i ; X <= x_f; X++) {
			p(X, y_i);
		}
	} else if(x_Mayor - x_Menor > y_Mayor - y_Menor) {
		for(float X = x_Menor ; X <= x_Mayor; X++) {
			float Y = m * (X - x_i) + y_i;
			p(X, Y);
		}
	} else {
		for(float Y = y_Menor ; Y <= y_Mayor; Y++) {
			float X = ((Y - y_i) + (m * x_i)) / m;
			std::cout<<X<<","<<Y<<'\n';
			p(X, Y);
		}
	}

}

/***********************************************************************
	DRAW PARTS FUNCTIONS
***********************************************************************/

void pelos() {
	float x_i = SECTION * 12;
	float x_f = x_i + SECTION * 3;
	float y = SECTION * 7.3;

	COLOR_SOMBRA;
	GROSOR_SOMBRA;
	glBegin(GL_POINTS);

		for(float i = x_i; i <= x_f; i++) {
			for(float j = y - SECTION * 0.25; j <= y + SECTION * 0.6; j++) {
				float v_s, v_i;
				if(i < x_i + SECTION * 1.5) {
					if(i < x_i + SECTION * 0.2) {
						v_s = value_of_curve(x_i, y, x_i + SECTION * 1.5, y + SECTION * 0.6, i);
						v_i = value_of_curve(x_i, y, x_i + SECTION * 0.2, y - SECTION * 0.25, i);
					} else {
						v_s = value_of_curve(x_i, y, x_i + SECTION * 1.5, y + SECTION * 0.6, i);
						v_i = value_of_curve(x_i, y - SECTION * 0.3, x_i + SECTION * 1.5, y + SECTION * 0.20, i);
					}
				} else {
					if(i >= x_f - SECTION * 0.2) {
						v_s = value_of_curve(x_f, y, x_i + SECTION * 1.5, y + SECTION * 0.6, i);
						v_i = value_of_curve(x_f, y, x_f - SECTION * 0.2, y - SECTION * 0.25, i);
					} else {
						v_s = value_of_curve(x_f, y, x_i + SECTION * 1.5, y + SECTION * 0.6, i);
						v_i = value_of_curve(x_f, y - SECTION * 0.3, x_i + SECTION * 1.5, y + SECTION * 0.20, i);
					}
				}
				if(v_i < j && j < v_s)
					p(i, j);
			}
		}

		/*semi_curve(x_i + SECTION * 0.80, y + SECTION * 0.40, x_i, y);
		semi_curve(x_i + SECTION * 0.80, y + SECTION * 0.40, x_i + SECTION * 0.65, y + SECTION * 0.15);
		
		semi_curve(x_i + SECTION * 0.65, y + SECTION * 0.15, x_i + SECTION * 1.55, y + SECTION * 0.45);
		semi_curve(x_i + SECTION * 1.30, y + SECTION * 0.25, x_i + SECTION * 1.55, y + SECTION * 0.45);
		
		semi_curve(x_i + SECTION * 1.30, y + SECTION * 0.25, x_i + SECTION * 2.10, y + SECTION * 0.40);
		semi_curve(x_i + SECTION * 1.90, y + SECTION * 0.20, x_i + SECTION * 2.10, y + SECTION * 0.40);
		
		semi_curve(x_i + SECTION * 1.90, y + SECTION * 0.20, x_i + SECTION * 2.70, y + SECTION * 0.25);
		semi_curve(x_i + SECTION * 2.50, y + SECTION * 0.10, x_i + SECTION * 2.70, y + SECTION * 0.25);
		
		semi_curve(x_f, y, x_i + SECTION * 2.5, y + SECTION * 0.1);*/
		
	glEnd();
}

void nariz() {
	float x_i = SECTION * 12;
	float x_f = x_i + SECTION * 3;
	float y = SECTION * 7;

	COLOR_CONTORNO;
	GROSOR_CONTORNO;
	glBegin(GL_POINTS);
		COLOR_NARIZ;
		for(float i = x_i; i <= x_f; i++) {
			for(float j = y - SECTION * 0.5; j <= y + SECTION * 0.5; j++){
				float v_s, v_i;
				if(i < x_i + SECTION * 0.4) {
					v_s = value_of_curve(x_i, y, x_i + SECTION * 1.5, y + SECTION * 0.5, i);
					v_i = value_of_curve(x_i, y, x_i + SECTION * 0.4, y - SECTION * 0.1, i);
				} else if(i < x_i + SECTION * 0.8){
					v_s = value_of_curve(x_i, y, x_i + SECTION * 1.5, y + SECTION * 0.5, i);
					v_i = value_of_curve(x_i + SECTION * 0.40, y - SECTION * 0.10, x_i + SECTION * 0.8, y + SECTION * 0.1, i);
					float v_ii = value_of_curve(x_i + SECTION * 1.2, y - SECTION * 0.3, x_i + SECTION * 0.4, y - SECTION * 0.1, i);
					if(v_ii < j && j < v_i) {
						COLOR_CONTORNO;
						p(i, j);
					} else {
						COLOR_NARIZ;
					}
				} else if(i < x_i + SECTION * 1.2) {
					v_s = value_of_curve(x_i, y, x_i + SECTION * 1.5, y + SECTION * 0.5, i);
					v_i = value_of_curve(x_i + SECTION * 1.2, y - SECTION * 0.29, x_i + SECTION * 0.8, y + SECTION * 0.1, i);
					float v_ii = value_of_curve(x_i + SECTION * 1.2, y - SECTION * 0.3, x_i + SECTION * 0.4, y - SECTION * 0.1, i);
					if(v_ii < j && j < v_i) {
						COLOR_CONTORNO;
						p(i, j);
					} else {
						COLOR_NARIZ;
					}
				} else if(i < x_i + SECTION * 1.5) {
					v_s = value_of_curve(x_i, y, x_i + SECTION * 1.5, y + SECTION * 0.5, i);
					v_i = value_of_curve(x_i + SECTION * 1.2, y - SECTION * 0.29, x_i + SECTION * 1.5, y - SECTION * 0.5, i);
				} else if(i < x_f - SECTION * 1.2) {
					v_s = value_of_curve(x_f, y, x_i + SECTION * 1.5, y + SECTION * 0.5, i);
					v_i = value_of_curve(x_f - SECTION * 1.2, y - SECTION * 0.29, x_i + SECTION * 1.5, y - SECTION * 0.5, i);
				} else if(i < x_f - SECTION * 0.8) {
					v_s = value_of_curve(x_f, y, x_i + SECTION * 1.5, y + SECTION * 0.5, i);
					v_i = value_of_curve(x_f - SECTION * 1.2, y - SECTION * 0.29, x_f - SECTION * 0.8, y + SECTION * 0.1, i);
					float v_ii = value_of_curve(x_f - SECTION * 1.2, y - SECTION * 0.3, x_f - SECTION * 0.4, y - SECTION * 0.1, i);
					if(v_ii < j && j < v_i) {
						COLOR_CONTORNO;
						p(i, j);
					} else {
						COLOR_NARIZ;
					}
				} else if(i < x_f - SECTION * 0.4) {
					v_s = value_of_curve(x_f, y, x_i + SECTION * 1.5, y + SECTION * 0.5, i);
					v_i = value_of_curve(x_f - SECTION * 0.40, y - SECTION * 0.10, x_f - SECTION * 0.8, y + SECTION * 0.1, i);
					float v_ii = value_of_curve(x_f - SECTION * 1.2, y - SECTION * 0.3, x_f - SECTION * 0.4, y - SECTION * 0.1, i);
					if(v_ii < j && j < v_i) {
						COLOR_CONTORNO;
						p(i, j);
					} else {
						COLOR_NARIZ;
					}
				} else {
					v_s = value_of_curve(x_f, y, x_i + SECTION * 1.5, y + SECTION * 0.5, i);
					v_i = value_of_curve(x_f, y, x_f - SECTION * 0.4, y - SECTION * 0.1, i);
				}

				if(v_i < j && j < v_s){
					p(i, j);
				}
			}
		}

		COLOR_CONTORNO;
		p(x_i, y); p(x_f, y);
		curve(x_i, y, x_f, y, x_i + SECTION * 1.5, y + SECTION * 0.5); // PARTE SUPERIOR

		semi_curve(x_i, y, x_i + SECTION * 0.4, y - SECTION * 0.1);// ENTRADA INFERIOR IZQUIERDA
		semi_curve(x_i + SECTION * 1.2, y - SECTION * 0.3, x_i + SECTION * 0.4, y - SECTION * 0.1);// ENTRADA INFERIOR IZQUIERDA

		semi_curve(x_f, y, x_f - SECTION * 0.4, y - SECTION * 0.1);// ENTRADA INFERIOR DERECHA
		semi_curve(x_f - SECTION * 1.2, y - SECTION * 0.3, x_f - SECTION * 0.4, y - SECTION * 0.1);// ENTRADA INFERIOR DERECHA

		curve(x_i + SECTION * 1.2, y - SECTION * 0.29, x_f - SECTION * 1.2, y - SECTION * 0.29, x_i + SECTION * 1.5, y - SECTION * 0.5);

		curve(x_i + SECTION * 0.40, y - SECTION * 0.10, x_i + SECTION * 1.2, y - SECTION * 0.29, x_i + SECTION * 0.8, y + SECTION * 0.1);
		curve(x_f - SECTION * 1.2, y - SECTION * 0.29, x_f - SECTION * 0.40, y - SECTION * 0.10, x_f - SECTION * 0.8, y + SECTION * 0.1);


	glEnd();
}

void ojo_izquierdo() {
	float x_i = SECTION * 7.6;
	float x_f = x_i + SECTION * 2.45;
	float y_i = SECTION * 7;
	float y_f = SECTION * 7.5;

	COLOR_CONTORNO;
	GROSOR_CONTORNO;
	glBegin(GL_POINTS);
		p(x_i, y_i); p(x_f, y_f);

		// PESTAÑAS
		line(x_f, y_f + 3, x_f + SECTION * 0.5, y_f - SECTION * 0.1);
		line(x_f, y_f + 2, x_f + SECTION * 0.5, y_f - SECTION * 0.1);
		line(x_f, y_f, x_f + SECTION * 0.5, y_f - SECTION * 0.1);
		line(x_f, y_f - 2, x_f + SECTION * 0.5, y_f - SECTION * 0.1);
		line(x_f, y_f - 3, x_f + SECTION * 0.5, y_f - SECTION * 0.1);

		line(x_i, y_i + 3, x_i - SECTION * 0.5, y_i - SECTION * 0.1);
		line(x_i, y_i + 2, x_i - SECTION * 0.5, y_i - SECTION * 0.1);
		line(x_i, y_i, x_i - SECTION * 0.5, y_i - SECTION * 0.1);
		line(x_i, y_i - 2, x_i - SECTION * 0.5, y_i - SECTION * 0.1);
		line(x_i, y_i - 3, x_i - SECTION * 0.5, y_i - SECTION * 0.1);
	glEnd();
	
	GROSOR_MEDIO;
	glBegin(GL_POINTS);
		COLOR_BLANCO;
		circle(x_i + SECTION * 1.15, y_f, SECTION * 1.3, true);
	glEnd();

	GROSOR_CONTORNO;
	COLOR_SOMBRA_CLARA;
	glBegin(GL_POINTS);
		circleIn(x_i + SECTION * 1.15, y_f, SECTION * 1.3, -90, 90);
		circleIn(x_i + SECTION * 1.10, y_f, SECTION * 1.3, -90, 90);
		circleIn(x_i + SECTION * 1.05, y_f, SECTION * 1.3, -90, 90);
		circleIn(x_i + SECTION * 1.00, y_f, SECTION * 1.3, -90, 90);
		circleIn(x_i + SECTION * 0.95, y_f, SECTION * 1.3, -90, 90);
	glEnd();
	
	GROSOR_CONTORNO;
	glBegin(GL_POINTS);
		COLOR_CONTORNO;
		circle(x_i + SECTION * 1.15, y_f, SECTION * 1.3);
		circle(x_i + SECTION * 1.15, y_f, SECTION * 1.35);
		circle(x_i + SECTION * 1.35, y_f - SECTION * 0.15, SECTION * 0.5, true);
	glEnd();

	GROSOR_MEDIO;
	glBegin(GL_POINTS);
		COLOR_BLANCO;
		circle(x_i + SECTION * 1.1, y_f - SECTION * 0.15, SECTION * 0.15, true);
		circle(x_i + SECTION * 1.25, y_f - SECTION * 0.45, SECTION * 0.07, true);
	glEnd();
}

void ojo_derecho() {
	float x_i = SECTION * 17;
	float x_f = x_i + SECTION * 2.45;
	float y_i = SECTION * 7.5;
	float y_f = SECTION * 7;

	COLOR_CONTORNO;
	GROSOR_CONTORNO;

	glBegin(GL_POINTS);
		p(x_i, y_i); p(x_f, y_f);

		// PESTAÑAS
		line(x_f, y_f + 3, x_f + SECTION * 0.5, y_f - SECTION * 0.1);
		line(x_f, y_f + 2, x_f + SECTION * 0.5, y_f - SECTION * 0.1);
		line(x_f, y_f, x_f + SECTION * 0.5, y_f - SECTION * 0.1);
		line(x_f, y_f - 2, x_f + SECTION * 0.5, y_f - SECTION * 0.1);
		line(x_f, y_f - 3, x_f + SECTION * 0.5, y_f - SECTION * 0.1);

		line(x_i, y_i + 3, x_i - SECTION * 0.5, y_i - SECTION * 0.1);
		line(x_i, y_i + 2, x_i - SECTION * 0.5, y_i - SECTION * 0.1);
		line(x_i, y_i, x_i - SECTION * 0.5, y_i - SECTION * 0.1);
		line(x_i, y_i - 2, x_i - SECTION * 0.5, y_i - SECTION * 0.1);
		line(x_i, y_i - 3, x_i - SECTION * 0.5, y_i - SECTION * 0.1);
	glEnd();

	GROSOR_MEDIO;
	glBegin(GL_POINTS);
		COLOR_BLANCO;
		circle(x_i + SECTION * 1.3, y_i, SECTION * 1.3, true);
	glEnd();

	GROSOR_CONTORNO;
	COLOR_SOMBRA_CLARA;
	glBegin(GL_POINTS);
		circleIn(x_i + SECTION * 1.3, y_i, SECTION * 1.3, -90, 90);
		circleIn(x_i + SECTION * 1.25, y_i, SECTION * 1.3, -90, 90);
		circleIn(x_i + SECTION * 1.2, y_i, SECTION * 1.3, -90, 90);
		circleIn(x_i + SECTION * 1.15, y_i, SECTION * 1.3, -90, 90);
		circleIn(x_i + SECTION * 1.1, y_i, SECTION * 1.3, -90, 90);
	glEnd();
	
	GROSOR_CONTORNO;
	glBegin(GL_POINTS);
		COLOR_CONTORNO;
		circle(x_i + SECTION * 1.3, y_i, SECTION * 1.3);
		circle(x_i + SECTION * 1.3, y_i, SECTION * 1.35);
		circle(x_i + SECTION * 1.1, y_i - SECTION * 0.15, SECTION * 0.5, true);
	glEnd();
	
	GROSOR_MEDIO;
	glBegin(GL_POINTS);
		COLOR_BLANCO;
		circle(x_i + SECTION * 0.85, y_i - SECTION * 0.15, SECTION * 0.15, true);
		circle(x_i + SECTION * 1.05, y_i - SECTION * 0.45, SECTION * 0.07, true);
	glEnd();
}

void boca() {
	float x_i = SECTION * 7;
	float x_f = x_i + SECTION * 13;
	float y = SECTION * 5;

	GROSOR_MEDIO;
	glBegin(GL_POINTS);
		COLOR_BLANCO;

		for(float i = x_i; i <= x_f; i++) {
			for(float j = (y - SECTION * 3.05); j <= (y + SECTION * 0.55); j++) {
				float v_s, v_i ;
				if(i < x_i + SECTION * 6.5){
					v_s = value_of_curve(x_i, y, x_i + SECTION * 6.5, y + SECTION * 0.5, i);
					v_i = value_of_curve(x_i, y, x_i + SECTION * 6.5, y - SECTION * 3.0, i);
				} else {
					v_s = value_of_curve(x_f, y, x_i + SECTION * 6.5, y + SECTION * 0.5, i);
					v_i = value_of_curve(x_f, y, x_i + SECTION * 6.5, y - SECTION * 3.0, i);
				}
				if(v_i < j && j < v_s){
					p(i, j);
				}
			}
		}
	glEnd();

	GROSOR_SOMBRA;
	COLOR_SOMBRA_CLARA;
	glBegin(GL_POINTS);
		for(float i = x_i; i <= x_f; i++) {
			for(float j = (y - SECTION * 3.05); j <= (y + SECTION * 0.55); j++) {
				float v_s = 0, v_i = 0;
				if(i < x_i + SECTION * 6.5){
					if(x_i + SECTION * 1.7 < i && i < x_i + SECTION * 1.8) {
						v_s = value_of_curve(x_i, y, x_i + SECTION * 6.5, y + SECTION * 0.5, i);
						v_i = value_of_curve(x_i, y, x_i + SECTION * 6.5, y - SECTION * 3.0, i);
					} else if(x_i + SECTION * 3.5 < i && i < x_i + SECTION * 3.7) {
						v_s = value_of_curve(x_i, y, x_i + SECTION * 6.5, y + SECTION * 0.5, i);
						v_i = value_of_curve(x_i, y, x_i + SECTION * 6.5, y - SECTION * 3.0, i);
					} else if(x_i + SECTION * 5.1 < i && i < x_i + SECTION * 5.40) {
						v_s = value_of_curve(x_i, y, x_i + SECTION * 6.5, y + SECTION * 0.5, i);
						v_i = value_of_curve(x_i, y, x_i + SECTION * 6.5, y - SECTION * 3.0, i);
					}
				} else {
					if(x_i + SECTION * 6.6 < i && i < x_i + SECTION * 7.00) {
						v_s = value_of_curve(x_f, y, x_i + SECTION * 6.5, y + SECTION * 0.5, i);
						v_i = value_of_curve(x_f, y, x_i + SECTION * 6.5, y - SECTION * 3.0, i);
					} else if(x_i + SECTION * 8.3 < i && i < x_i + SECTION * 8.80) {
						v_s = value_of_curve(x_f, y, x_i + SECTION * 6.5, y + SECTION * 0.5, i);
						v_i = value_of_curve(x_f, y, x_i + SECTION * 6.5, y - SECTION * 3.0, i);
					} else if(x_i + SECTION * 9.8 < i && i < x_i + SECTION * 10.4) {
						v_s = value_of_curve(x_f, y, x_i + SECTION * 6.5, y + SECTION * 0.5, i);
						v_i = value_of_curve(x_f, y, x_i + SECTION * 6.5, y - SECTION * 3.0, i);
					} else if(x_i + SECTION * 11.2 < i) {
						v_s = value_of_curve(x_f, y, x_i + SECTION * 6.5, y + SECTION * 0.5, i);
						v_i = value_of_curve(x_f, y, x_i + SECTION * 6.5, y - SECTION * 3.0, i);
					}
				}
				if(v_i < j && j < v_s){
					p(i, j);
				}
			}
		}
	glEnd();

	GROSOR_CONTORNO;
	glBegin(GL_POINTS);
		COLOR_CONTORNO;
		p(x_i, y); p(x_f, y);

		curve(x_i, y, x_f, y, x_i + SECTION * 6.5, y + SECTION * 0.5); // LABIO SUPERIOR
		curve(x_i, y+1, x_f, y+1, x_i + SECTION * 6.5, y + SECTION * 0.55); // LABIO SUPERIOR

		curve(x_i, y, x_f, y, x_i + SECTION * 6.5, y - SECTION * 3.0); // LABIO INFERIOR
		curve(x_i, y-1, x_f, y-1, x_i + SECTION * 6.5, y - SECTION * 3.05); // LABIO INFERIOR


		// DIENTES
		line(x_i + SECTION * 1.80, y - SECTION * 1.42, x_i + SECTION * 1.80, y + SECTION * 0.20);
		line(x_i + SECTION * 3.70, y - SECTION * 2.42, x_i + SECTION * 3.70, y + SECTION * 0.38);
		line(x_i + SECTION * 5.40, y - SECTION * 2.90, x_i + SECTION * 5.40, y + SECTION * 0.45);
		line(x_i + SECTION * 7.00, y - SECTION * 2.98, x_i + SECTION * 7.00, y + SECTION * 0.48);
		line(x_i + SECTION * 8.80, y - SECTION * 2.60, x_i + SECTION * 8.80, y + SECTION * 0.43);
		line(x_i + SECTION * 10.4, y - SECTION * 1.90, x_i + SECTION * 10.4, y + SECTION * 0.30);
	glEnd();
}

void bigotes_izquierdos() {
	float x_i_1 = SECTION * 1.60;	float x_f_1 = x_i_1 + SECTION * 4.9;
	float y_i_1 = SECTION * 7.80;	float y_f_1 = y_i_1 - SECTION * 1.8;

	float x_i_2 = SECTION * 1.00;	float x_f_2 = x_i_2 + SECTION * 5.0;
	float y_i_2 = SECTION * 5.50;	float y_f_2 = y_i_2 + SECTION * 0.0;

	float x_i_3 = SECTION * 2.00;	float x_f_3 = x_i_3 + SECTION * 4.3;
	float y_i_3 = SECTION * 3.50;	float y_f_3 = y_i_3 + SECTION * 1.0;

	COLOR_SOMBRA;
	GROSOR_SOMBRA;
	glBegin(GL_POINTS);
		line(x_i_1, y_i_1 - SECTION * 0.5, x_f_1 - 1, y_f_1 + 2);
		line(x_i_1, y_i_1 - SECTION * 0.5, x_f_1, y_f_1 + 1);
		line(x_i_1, y_i_1 - SECTION * 0.5, x_f_1, y_f_1);
		line(x_i_1, y_i_1 - SECTION * 0.5, x_f_1, y_f_1 - 1);
		line(x_i_1, y_i_1 - SECTION * 0.5, x_f_1 - 1, y_f_1 - 2);

		line(x_i_2 + SECTION * 0.1, y_i_2 - SECTION * 0.2, x_f_2 - 1, y_f_2 + 2);
		line(x_i_2 + SECTION * 0.1, y_i_2 - SECTION * 0.2, x_f_2, y_f_2 + 1);
		line(x_i_2 + SECTION * 0.1, y_i_2 - SECTION * 0.2, x_f_2, y_f_2);
		line(x_i_2 + SECTION * 0.1, y_i_2 - SECTION * 0.2, x_f_2, y_f_2 - 1);
		line(x_i_2 + SECTION * 0.1, y_i_2 - SECTION * 0.2, x_f_2 - 1, y_f_2 - 2);

		line(x_i_3 + SECTION * 0.2, y_i_3 - SECTION * 0.3, x_f_3 - 1, y_f_3 + 2);
		line(x_i_3 + SECTION * 0.2, y_i_3 - SECTION * 0.3, x_f_3, y_f_3 + 1);
		line(x_i_3 + SECTION * 0.2, y_i_3 - SECTION * 0.3, x_f_3, y_f_3);
		line(x_i_3 + SECTION * 0.2, y_i_3 - SECTION * 0.3, x_f_3, y_f_3 - 1);
		line(x_i_3 + SECTION * 0.2, y_i_3 - SECTION * 0.3, x_f_3 - 1, y_f_3 - 2);
	glEnd();

	COLOR_CONTORNO;
	GROSOR_CONTORNO;
	glBegin(GL_POINTS);
		p(x_i_1, y_i_1); p(x_f_1, y_f_1);
		p(x_i_2, y_i_2); p(x_f_2, y_f_2);
		p(x_i_3, y_i_3); p(x_f_3, y_f_3);

		line(x_i_1, y_i_1, x_f_1 - 1, y_f_1 + 2);
		line(x_i_1, y_i_1, x_f_1, y_f_1 + 1);
		line(x_i_1, y_i_1, x_f_1, y_f_1);
		line(x_i_1, y_i_1, x_f_1, y_f_1 - 1);
		line(x_i_1, y_i_1, x_f_1 - 1, y_f_1 - 2);

		line(x_i_2, y_i_2, x_f_2 - 1, y_f_2 + 2);
		line(x_i_2, y_i_2, x_f_2, y_f_2 + 1);
		line(x_i_2, y_i_2, x_f_2, y_f_2);
		line(x_i_2, y_i_2, x_f_2, y_f_2 - 1);
		line(x_i_2, y_i_2, x_f_2 - 1, y_f_2 - 2);

		line(x_i_3, y_i_3, x_f_3 - 1, y_f_3 + 2);
		line(x_i_3, y_i_3, x_f_3, y_f_3 + 1);
		line(x_i_3, y_i_3, x_f_3, y_f_3);
		line(x_i_3, y_i_3, x_f_3, y_f_3 - 1);
		line(x_i_3, y_i_3, x_f_3 - 1, y_f_3 - 2);
	glEnd();
}

void bigotes_derechos() {
	float x_i_1 = SECTION * 20.5;	float x_f_1 = x_i_1 + SECTION * 4.5;
	float y_i_1 = SECTION * 06.0;	float y_f_1 = y_i_1 + SECTION * 1.8;

	float x_i_2 = SECTION * 21.0;	float x_f_2 = x_i_2 + SECTION * 5.0;
	float y_i_2 = SECTION * 05.5;	float y_f_2 = y_i_2 + SECTION * 0.0;

	float x_i_3 = SECTION * 21.2;	float x_f_3 = x_i_3 + SECTION * 3.8;
	float y_i_3 = SECTION * 04.6;	float y_f_3 = y_i_3 - SECTION * 0.2;

	COLOR_SOMBRA;
	GROSOR_SOMBRA;
	glBegin(GL_POINTS);
		line(x_i_1 + 1, y_i_1 + 2, x_f_1, y_f_1 - SECTION * 0.5);
		line(x_i_1, y_i_1 + 1, x_f_1, y_f_1 - SECTION * 0.5);
		line(x_i_1, y_i_1, x_f_1, y_f_1 - SECTION * 0.5);
		line(x_i_1, y_i_1 - 1, x_f_1, y_f_1 - SECTION * 0.5);
		line(x_i_1 + 1, y_i_1 - 2, x_f_1, y_f_1 - SECTION * 0.5);

		line(x_i_2 + 1, y_i_2 + 2, x_f_2 - SECTION * 0.1, y_f_2 - SECTION * 0.2);
		line(x_i_2, y_i_2 + 1, x_f_2 - SECTION * 0.1, y_f_2 - SECTION * 0.2);
		line(x_i_2, y_i_2, x_f_2 - SECTION * 0.1, y_f_2 - SECTION * 0.2);
		line(x_i_2, y_i_2 - 1, x_f_2 - SECTION * 0.1, y_f_2 - SECTION * 0.2);
		line(x_i_2 + 1, y_i_2 - 2, x_f_2 - SECTION * 0.1, y_f_2 - SECTION * 0.2);

		line(x_i_3 + 1, y_i_3 + 2, x_f_3 - SECTION * 0.2, y_f_3 - SECTION * 0.3);
		line(x_i_3, y_i_3 + 1, x_f_3 - SECTION * 0.2, y_f_3 - SECTION * 0.3);
		line(x_i_3, y_i_3, x_f_3 - SECTION * 0.2, y_f_3 - SECTION * 0.3);
		line(x_i_3, y_i_3 - 1, x_f_3 - SECTION * 0.2, y_f_3 - SECTION * 0.3);
		line(x_i_3 + 1, y_i_3 - 2, x_f_3 - SECTION * 0.2, y_f_3 - SECTION * 0.3);
	glEnd();

	COLOR_CONTORNO;
	GROSOR_CONTORNO;
	glBegin(GL_POINTS);
		p(x_i_1, y_i_1); p(x_f_1, y_f_1);
		p(x_i_2, y_i_2); p(x_f_2, y_f_2);
		p(x_i_3, y_i_3); p(x_f_3, y_f_3);

		line(x_i_1 + 1, y_i_1 + 2, x_f_1, y_f_1);
		line(x_i_1, y_i_1 + 1, x_f_1, y_f_1);
		line(x_i_1, y_i_1, x_f_1, y_f_1);
		line(x_i_1, y_i_1 - 1, x_f_1, y_f_1);
		line(x_i_1 + 1, y_i_1 - 2, x_f_1, y_f_1);

		line(x_i_2 + 1, y_i_2 + 2, x_f_2, y_f_2);
		line(x_i_2, y_i_2 + 1, x_f_2, y_f_2);
		line(x_i_2, y_i_2, x_f_2, y_f_2);
		line(x_i_2, y_i_2 - 1, x_f_2, y_f_2);
		line(x_i_2 + 1, y_i_2 - 2, x_f_2, y_f_2);

		line(x_i_3 + 1, y_i_3 + 2, x_f_3, y_f_3);
		line(x_i_3, y_i_3 + 1, x_f_3, y_f_3);
		line(x_i_3, y_i_3, x_f_3, y_f_3);
		line(x_i_3, y_i_3 - 1, x_f_3, y_f_3);
		line(x_i_3 + 1, y_i_3 - 2, x_f_3, y_f_3);
	glEnd();
}

/***********************************************************************
	GL FUNCTIONS
***********************************************************************/

void myInit (void) {
	COLOR_FONDO;
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	gluOrtho2D(0, WIDTH, 0, HEIGHT);
}

void myDisplay (void) {
	glClear(GL_COLOR_BUFFER_BIT);

	pelos();
	nariz();
	ojo_izquierdo();
	ojo_derecho();
	boca();
	bigotes_derechos();
	bigotes_izquierdos();

	glFlush(); 
}

void myResize (int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
}

int main (int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
	glutInitWindowSize(WIDTH, HEIGHT); 
	glutInitWindowPosition(100, 100); 
	glutCreateWindow("Graphics Window");  
	glutDisplayFunc(myDisplay); 
	glutReshapeFunc(myResize); 
	myInit(); 
	glutMainLoop(); 
	
	return 0;
}
