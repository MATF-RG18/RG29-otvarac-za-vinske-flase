#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define PI 3.14159265358979323846
/*Ptomenljiva koja oznacava da li se poziva tajmer*/
static int timer_active;

static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_display(void);

/*Promenljive koje se koriste u tajmeru (korakY, korak_ugao) i u funkciji on_display (korakY, korakUgao)
korak_ugao- za toliko se menja ugao rotacije otvaraca u svakom pozivu tajmera
korakUgao- koristi se za pravljenje spirale u on_display
korakY- koristi se za pravljenje spirale u on_display i za poredjenje float brojeva u tajmeru*/
float korakY = 1.0/180, korakUgao = PI/180, korak_ugao = 10;
float korak = -1.0/18;
/*ugao- ugao za rotiranje otvaraca, menja se u tajmeru*/
float ugao = 0;
/*duzina_pampura- koristi se u tajmeru i u on_display za pravljenje pampura*/
float duzina_pampura = 3.75;
/*spust- pomeraj otvaraca duz y-osu kasnije i pampura*/
float spust = 0;

int main(int argc, char **argv)
{
/*Inicijalizujem GLUT*/
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
/*Pravim prozor*/
    glutInitWindowSize(1200, 1200);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
/*Postavljanje callback funkcija*/
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);

    timer_active = 0;

    glEnable(GL_DEPTH_TEST);
 	glEnable(GL_BLEND);
/*Postavlja se blend funckija*/
    glBlendFunc(GL_SRC_COLOR, GL_SRC_COLOR);

    glClearColor(1, 1, 1, 0);

    glutMainLoop();

    return 0;
}

static void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
/*Zavrsava se program*/
        exit(0);
        break;

    case 'g':
    case 'G':
/*Pokrece se simulacija (g->GO)*/
        if (!timer_active)
		{
            timer_active = 1;
            glutTimerFunc(10, on_timer, 0);
        }
        break;

    case 's':
    case 'S':
/*Zaustavlja se simulacija (s->STOP)*/
        timer_active = 0;
        break;

	case 'r':
	case 'R':
/*Simulacija se vraca na pocetak (r->RESET)
vracamo sve promenljive vezane za tajmer na pocetne vrednosti*/
		ugao = 0;
		spust = 0;
		korak = -1.0/18;
		korak_ugao = 10;
    	glutPostRedisplay();
		break;
    }
}

static void on_timer(int value)
{
/* Proverava se da li poziv dolazi od naseg tajmera*/
    if (value != 0)
        return;

/*menjamo ugao rotacije otvaraca (ako je veci od 360 oduzimamo 360)*/
	if(ugao >= 360)
	{
		ugao = ugao - 360;
	}
	ugao = ugao + korak_ugao;

/*proveravamo da li je otvarac dosao do kraja pampura i ako jeste menjamo mu smer kretanja i zaustavljamo dalju rotaciju*/
	if(fabs(fabs(spust)-duzina_pampura) <= korakY*10 && korak < 0)
	{
		korak = -korak;
		korak_ugao = 0;
	}
/*ako se pampur krece na gore uz y-osu i dosao je do odredjene visine, zaustavljamo kretanje*/
	else if(fabs(fabs(spust)-duzina_pampura) <= korakY*10 && korak > 0)
	{
		korak = 0;
		korak_ugao = 0;
	}
/*menjamo spust odnosno pomeraj otvaraca, a kasnije i pampura, duz y-osu*/
	spust += korak;

    glutPostRedisplay();

    if (timer_active)
        glutTimerFunc(10, on_timer, 0);
}

static void on_reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 100);
}

static void on_display(void)
{
/*Postavlja se svetlo i materijal prvog objekta (pampur)*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat light_position[] = {0.5, 0.5, 0.5, 0};
    GLfloat light_ambient[] = {0.5, 0.5, 0.5, 1};
    GLfloat light_diffuse[] = {0.7, 0.7, 0.7, 1};
    GLfloat light_specular[] = {0.9, 0.9, 0.9, 1};

    GLfloat ambient_coeffs[] = {0.5, 200.0/438, 153.0/438, 1};
    GLfloat diffuse_coeffs[] = {0.3, 0.4, 0.2, 0.5};
    GLfloat specular_coeffs[] = {0.9, 0.9, 0.9, 0.9};
    GLfloat shininess = 50;

glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 70, 0, 0, 0, 0, 1, 0);

glPushMatrix();

/*Transliramo sve objekte gore po y-osi*/
	glTranslatef(0, 15, 0);

	glPushMatrix();

		GLUquadric* quad = gluNewQuadric();

/*Pravi se pampur: translira se ispod spirale otvaraca i rotira se u odgovarajuci polozaj*/
		glTranslatef(0, -3.5-2.75*PI, 0);
/*Translira se na gore ako je korak pozitivan odnosno ako je otvarac krenuo da ga vadi*/
		if(korak >= 0)
		{
			glTranslatef(0, duzina_pampura + spust, 0);
		}
		glRotatef(90, 1, 0, 0);
		gluCylinder(quad, 1, 1, duzina_pampura, 20, 10);
		glRotatef(-90, 1, 0, 0);
		if(korak >= 0)
		{
			glTranslatef(0, -(duzina_pampura + spust), 0);
		}

/*Postavlja se materijal unutrasnjeg sloja flase i menjamo blend funkciju da flasa bude providna kroz dva sloja*/
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		ambient_coeffs[0] = 0.1;
		ambient_coeffs[1] = 0.2;
		ambient_coeffs[2] = 0.0;
		ambient_coeffs[3] = 0.5;

		diffuse_coeffs[0] = 0.3;
		diffuse_coeffs[1] = 0.4;
		diffuse_coeffs[2] = 0.2;
		diffuse_coeffs[3] = 0.5;

		shininess = 5;

		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);

/*Sferni deo flase (ispod grlica) se pravi od polusfere koja se dobija klipovanjem sfere*/
    	double clip_plane[] = {0, 1, 0, 0};

/*Pravi se unutrasnji deo polusfere; translira se za duzinu grlica*/
		glTranslatef(0, -9.5, 0);
		glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, clip_plane);
			glutSolidSphere(3.8, 40, 40);
   		glDisable(GL_CLIP_PLANE0);

/*Pravi se valjak koji predstavlja unutrasnji deo flase, ispod tog sfernog dela; rotira se do adekvatnog polozaja*/
		glRotatef(90, 1, 0, 0);
		gluCylinder(quad, 3.8, 3.8, 16, 40, 40);
		glRotatef(-90, 1, 0, 0);

/*Pravi se valjak koji predstavlja unutrasnji deo grlica flase, iznad tog sfernog dela; rotira se do adekvatnog polozaja i translira iznad polusfere*/
		glTranslatef(0, 9.5, 0);
		glRotatef(90, 1, 0, 0);
		gluCylinder(quad, 1, 1, 6, 40, 40);
		glRotatef(-90, 1, 0, 0);
		glTranslatef(0, -9.5, 0);

/*Postavlja se materijal spoljasnjeg sloja flase*/
		ambient_coeffs[3] = 0.3;
		diffuse_coeffs[3] = 0.3;
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);

/*Pravi se spoljasnji deo polusfere*/
		glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, clip_plane);
			glutSolidSphere(4, 40, 40);
   		glDisable(GL_CLIP_PLANE0);

/*Pravi se valjak koji predstavlja spoljasnji deo flase, ispod tog sfernog dela; rotira se do adekvatnog polozaja*/
		glRotatef(90, 1, 0, 0);
		gluCylinder(quad, 4, 4, 16, 40, 40);
		glRotatef(-90, 1, 0, 0);

/*Pravi se valjak koji predstavlja spoljasnji deo grlica flase, iznad tog sfernog dela; rotira se do adekvatnog polozaja i translira iznad polusfere*/
		glTranslatef(0, 9.5, 0);
		glRotatef(90, 1, 0, 0);
		gluCylinder(quad, 1.25, 1.25, 6, 40, 40);
		glRotatef(-90, 1, 0, 0);
		glTranslatef(0, -9.5, 0);

	glPopMatrix();

/*Postavlja se materijal za sledeci objekat (drveni deo odnosno drska otvaraca)*/
	ambient_coeffs[0] = 0.3;
	ambient_coeffs[1] = 0.2;
	ambient_coeffs[2] = 0.2;
	ambient_coeffs[3] = 1;

	diffuse_coeffs[0] = 0.3;
	diffuse_coeffs[1] = 0.2;
	diffuse_coeffs[2] = 0.2;
	diffuse_coeffs[3] = 1;

	specular_coeffs[0] = 1;
	specular_coeffs[1] = 1;
	specular_coeffs[2] = 1;
	specular_coeffs[3] = 1;

	shininess = 20;

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	glPushMatrix();

/*Prvi objekat se translira na dole po y-osi za spust koji se menja tajmerom i rotira za ugao koji se menja tajmerom*/
		glTranslatef(0, spust, 0);
		glRotatef(-ugao, 0, 1, 0);

/*Pravi se braonkast valjak koji treba da predstavlja drveni drzac otvaraca; translira se i rotira do odgovarajuce pozicije*/
		glTranslatef(-4.5, 0, 0);
		glRotatef(90, 0, 1, 0);
		gluCylinder(quad, 2, 2, 9, 20, 10);
		glRotatef(-90, 0, 1, 0);
		glTranslatef(4.5, 0, 0);

/*prave se dva kruga da zatvore valjak*/
		float ugao_krug;

		glBegin(GL_POLYGON);
			for(ugao_krug = 0; ugao_krug <= 2*PI + PI/20; ugao_krug += PI/20)
			{
				glNormal3f(-1, 0, 0);
				glVertex3f(-4.5, 2*cos(ugao_krug), 2*sin(ugao_krug));
			}
		glEnd();

		glBegin(GL_POLYGON);
			for(ugao_krug = 0; ugao_krug <= 2*PI + PI/20; ugao_krug += PI/20)
			{
				glNormal3f(1, 0, 0);
				glVertex3f(4.5, 2*cos(ugao_krug), 2*sin(ugao_krug));
			}
		glEnd();

/*Pravi se sivi valjak koji je pocetak metalnog dela, pre spirale. 
Prvo se postavlja materijal za njega i spiralu*/
		ambient_coeffs[0] = 0.3;
		ambient_coeffs[1] = 0.3;
		ambient_coeffs[2] = 0.3;

    	diffuse_coeffs[0] = 0.4;
    	diffuse_coeffs[1] = 0.4;
    	diffuse_coeffs[2] = 0.4;

    	specular_coeffs[0] = 0.9;
    	specular_coeffs[1] = 0.9;
    	specular_coeffs[2] = 0.9;

		shininess = 5;

		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);

		light_ambient[0] = 0.5;
   		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

/*Translira se ispod prethodnog valjka i rotira se*/
		glTranslatef(0, -1.5, 0);
		glRotatef(90, 1, 0, 0);
		gluCylinder(quad, 0.7, 0.7, 3, 20, 10);

	glPopMatrix();

/*Pravi se spirala*/
	glPushMatrix();

/*Transliramo je ispod sivog valjka i za spust koji menja tajmer i rotiramo za ugao koji se menja tajmerom*/
		glTranslatef(0, -3.5+spust, 0);
		glRotatef(-ugao, 0, 1, 0);

		float ugao_spirale, ugao_valjka, precnik_valjka = 0.25, precnik_spirale = 0.4, krug = 2*PI;
		float centar_valjkaX0 = precnik_spirale, centar_valjkaY0 = 0, centar_valjkaZ0 = 0;
		float centar_valjkaX1 = precnik_spirale*cos(korakUgao), centar_valjkaY1 = -korakY, centar_valjkaZ1 = precnik_spirale*sin(korakUgao);

/*Spiralu pravimo tako sto pravimo krugove poluprecnika = precnik_valjka koji su paralelni sa y = 0 ravni i 
njihovi centri kad se povezu prave spiralu poluprecnika = precnik spirale i spusta po y-osi korakY. Tacke tih krugova povezujemo sa GL_TRIANGLE_STRIP*/
		glBegin(GL_TRIANGLE_STRIP);
		for(ugao_spirale = 0; ugao_spirale <= 4*krug + korakUgao; ugao_spirale += korakUgao)
		{
			for(ugao_valjka = 0; ugao_valjka <= krug + korakUgao; ugao_valjka += korakUgao)
			{
				glNormal3f(precnik_valjka*cos(ugao_valjka) + centar_valjkaX0, 1, precnik_valjka*sin(ugao_valjka) + centar_valjkaZ0);
				glVertex3f(precnik_valjka*cos(ugao_valjka) + centar_valjkaX0, centar_valjkaY0, precnik_valjka*sin(ugao_valjka) + centar_valjkaZ0);
				glVertex3f(precnik_valjka*cos(ugao_valjka) + centar_valjkaX1, centar_valjkaY1, precnik_valjka*sin(ugao_valjka) + centar_valjkaZ1);
			}
			centar_valjkaX0 = centar_valjkaX1;
			centar_valjkaZ0 = centar_valjkaZ1;
			centar_valjkaY0 = centar_valjkaY1;
			centar_valjkaX1 = precnik_spirale*cos(ugao_spirale + korakUgao);
			centar_valjkaZ1 = precnik_spirale*sin(ugao_spirale + korakUgao);
			centar_valjkaY1 = centar_valjkaY0 - korakY;
		}
		glEnd();

		float korak_precnika = 0.25/90;

/*Kraj spirale pravimo isto kao i ostatak samo sto se poluprecnik krugova smanjuje za korak_precnika (koji je toliki da za pola kruga spirale poluprecnik kruga bude 0)*/
		glBegin(GL_TRIANGLE_STRIP);
		for(ugao_spirale = 0; ugao_spirale <= PI/2 + korakUgao; ugao_spirale += korakUgao)
		{
			for(ugao_valjka = 0; ugao_valjka <= krug + korakUgao; ugao_valjka += korakUgao)
			{
				glNormal3f(precnik_valjka*cos(ugao_valjka) + centar_valjkaX0, 1, precnik_valjka*sin(ugao_valjka) + centar_valjkaZ0);
				glVertex3f(precnik_valjka*cos(ugao_valjka) + centar_valjkaX0, centar_valjkaY0, precnik_valjka*sin(ugao_valjka) + centar_valjkaZ0);
				glVertex3f(precnik_valjka*cos(ugao_valjka) + centar_valjkaX1, centar_valjkaY1, precnik_valjka*sin(ugao_valjka) + centar_valjkaZ1);
			}
			centar_valjkaX0 = centar_valjkaX1;
			centar_valjkaZ0 = centar_valjkaZ1;
			centar_valjkaY0 = centar_valjkaY1;
			centar_valjkaX1 = precnik_spirale*cos(ugao_spirale + korakUgao);
			centar_valjkaZ1 = precnik_spirale*sin(ugao_spirale + korakUgao);
			centar_valjkaY1 = centar_valjkaY0 - korakY;
			precnik_valjka -= korak_precnika;
		}
		glEnd();
	glPopMatrix();

glPopMatrix();

glDisable(GL_LIGHTING);

/*Prave se koordinatne ose radi bolje orijentacije*/
	glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3f(-100, 0, 0);
		glVertex3f(100, 0, 0);
	glEnd();

	glBegin(GL_LINES);
		glColor3f(0, 1, 0);
		glVertex3f(0, -100, 0);
		glVertex3f(0, 100, 0);
	glEnd();

	glBegin(GL_LINES);
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, -100);
		glVertex3f(0, 0, 100);
	glEnd();
   
    glutSwapBuffers();
}
