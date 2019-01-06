#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define PI 3.14159265358979323846
/*Promenljiva koja oznacava da li se poziva tajmer i promenljiva koja govori tajmeru koji otvarac koristimo (jer su animacije drugacije)*/
static int timer_active, timer_id = 0;

static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_display(void);

/*Funkcije koje redom crtaju flasu, prvi i drugi otvarac*/
void flasa(GLUquadric* quad);
void otvarac1(GLUquadric* quad);
void otvarac2(GLUquadric* quad);

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

/*spust- pomeraj otvaraca duz y-osu kasnije i pampura, vazi za prvi otvarac
spust_tela, korak_tela- pomeraj i korak za pomeraj tela otvaraca
spust_spirale, korak_spirale- pomeraj i korak za pomeraj spirale otvaraca
spust_pampura, korak_pampura- pomeraj i korak za pomeraj pampura; vaze za drugi otvarac*/
float spust = 0, spust_tela = 0, spust_spirale = 0, spust_pampura = 0, korak_pampura = 0, korak_spirale = -0.5, korak_tela = -0.5;

/*rastojanje_flase_i_otvaraca- rastojanje flase i drugog otvaraca*/
float rastojanje_flase_i_otvaraca = 10;

/*Promenljive za svetlo inicijalizovane i promenljive za materijal deklarisane; koriste se u funkcijama za iscrtavanje zato su globalne*/
GLfloat light_position[] = {0.5, 0.5, 0.5, 0};
GLfloat light_ambient[] = {0.5, 0.5, 0.5, 1};
GLfloat light_diffuse[] = {0.7, 0.7, 0.7, 1};
GLfloat light_specular[] = {0.9, 0.9, 0.9, 1};

GLfloat ambient_coeffs[4];
GLfloat diffuse_coeffs[4];
GLfloat specular_coeffs[4];
GLfloat shininess;

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

    glClearColor(247/255.0, 200.0/255, 143/255.0, 0);

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
            glutTimerFunc(10, on_timer, timer_id);
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

	case '0':
/*Bira se iscrtavanje i animacija za prvi otvarac*/
		if(!timer_active)
		{
			korak_ugao = 10;
			timer_id = 0;
			glutPostRedisplay();
        }
        break;

	case '1':
/*Bira se iscrtavanje i animacija za drugi otvarac*/
		if(!timer_active)
		{
			korak_ugao = 0;
			timer_id = 1;
			glutPostRedisplay();
        }
        break;
    }
}

static void on_timer(int value)
{
/* Proverava se da li poziv dolazi od naseg tajmera i koja animacija za otvarac se trazi*/
    if (value == 0)
	{
/*Izabran je prvi otvarac*/
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
		spust_pampura = spust += korak;
	}
	else if(value == 1)
	{
/*Izabran je prvi otvarac*/
		if(fabs(spust_spirale + rastojanje_flase_i_otvaraca + 11) <= 0.5 && fabs(spust_tela + rastojanje_flase_i_otvaraca + 11) <= 0.5)
		{
/*Prvo se i telo i spirala otvaraca krecu ka flasi istovremeno. Kada dodju do flase, spirala se zaustavlja, a telo se spusta jos malo. Zato stavljamo korak spirale na 0*/
			korak_spirale = 0;
		}
		else if(fabs(spust_spirale + rastojanje_flase_i_otvaraca + 11) <= 0.5 && fabs(spust_tela + rastojanje_flase_i_otvaraca + duzina_pampura + 11) <= 0.5)
		{
/*Kada se telo otvaraca spustilo dovoljno zaustavljamo telo, a rotiramo i spustamo spiralu. 
Postavljamo korak za ugao rotacije i korak spustanja spirale na odgovarajuce vrednosti, a korak tela stavljamo na 0*/
			korak_tela = 0;
			korak_spirale = -1.0/18;
			korak_ugao = 10;
		}
		else if(fabs(spust_spirale + rastojanje_flase_i_otvaraca + duzina_pampura + 11) <= 1.0/18)
		{
/*Kada se spirala otvaraca spustila dovoljno prestajemo da je spustamo, ali nastavljamo da je rotiramo. Pampur se zbog toga dize uz spiralu.
Postavljamo korak spirale na nula i korak podizanja pampura na odgovarajucu vrednost.*/
			korak_spirale = 0;
			korak_pampura = 1.0/18;
		}
		else if(fabs(spust_pampura - duzina_pampura) <= 1.0/18)
		{
/*Kada je pampur izasao iz flase i podigao dovoljno uz spiralu zaustaviti kretanje.
TODO: naci gresku ovde i popraviti je*/
			korak_ugao = 0;
			korak_pampura = 0;
			printf("%f %f\n", korak_ugao, korak_pampura);
		}
/*Menjamo sve date promenljive*/
		ugao += korak_ugao;
		spust_spirale += korak_spirale;
		spust_tela += korak_tela;
		spust_pampura += korak_pampura;
	}
	else
		return;

/*Prikazujemo sliku opet i ponavljamo tajmer opet ako treba*/
		glutPostRedisplay();

		if (timer_active)
		{
		    glutTimerFunc(10, on_timer, timer_id);
		}
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
/*Postavlja se svetlo i polozaj kamere*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 70, 0, 0, 0, 0, 1, 0);

glPushMatrix();

/*Transliramo sve objekte gore po y-osi*/
	glTranslatef(0, 30, 0);

	glPushMatrix();

	GLUquadric* quad = gluNewQuadric();

/*Ako je timer_id = 1, zovemo drugi otvarac; odvajamo otvarac i flasu na vec definisano rastojanje.
Ako je timer_id = 0, zovemo prvi otvarac*/
	if(timer_id)
	{
		glTranslatef(0, -rastojanje_flase_i_otvaraca, 0);
		flasa(quad);
		glTranslatef(0, rastojanje_flase_i_otvaraca, 0);
		otvarac2(quad);
	}
	else
	{
		flasa(quad);
		otvarac1(quad);
	}
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

void flasa(GLUquadric* quad)
{
/*Postavlja se materijal pampura*/
		ambient_coeffs[0] = 0.6;
		ambient_coeffs[1] = 200.0/438;
		ambient_coeffs[2] = 153.0/438;
		ambient_coeffs[3] = 1;

		diffuse_coeffs[0] = 0.3;
		diffuse_coeffs[1] = 0.4;
		diffuse_coeffs[2] = 0.2;
		diffuse_coeffs[3] = 1;

		shininess = 5;

		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);

/*Pravi se pampur: translira se ispod spirale otvaraca (sto je 22 za oba otvaraca) i rotira se u odgovarajuci polozaj*/
		glTranslatef(0, -22, 0);

/*Translira se na gore ako je korak pozitivan odnosno ako je otvarac krenuo da ga vadi (radimo disjunkciju da bi vazilo za obe animacije)*/
		if(korak >= 0 || korak_pampura > 0)
		{
			glTranslatef(0, duzina_pampura + spust_pampura, 0);
		}
		glRotatef(90, 1, 0, 0);
		gluCylinder(quad, 1, 1, duzina_pampura, 20, 10);
		glRotatef(-90, 1, 0, 0);
		if(korak >= 0 || korak_pampura > 0)
		{
			glTranslatef(0, -(duzina_pampura + spust_pampura), 0);
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
}

void otvarac1(GLUquadric* quad)
{
	/*Postavlja se materijal za drveni deo odnosno drska otvaraca*/
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
		gluCylinder(quad, 1.5, 1.5, 9, 20, 10);
		glRotatef(-90, 0, 1, 0);
		glTranslatef(4.5, 0, 0);

/*prave se dva kruga da zatvore valjak*/
		float ugao_krug;

		glBegin(GL_POLYGON);
			for(ugao_krug = 0; ugao_krug <= 2*PI + PI/20; ugao_krug += PI/20)
			{
				glNormal3f(-1, 0, 0);
				glVertex3f(-4.5, 1.5*cos(ugao_krug),1.5*sin(ugao_krug));
			}
		glEnd();

		glBegin(GL_POLYGON);
			for(ugao_krug = 0; ugao_krug <= 2*PI + PI/20; ugao_krug += PI/20)
			{
				glNormal3f(1, 0, 0);
				glVertex3f(4.5, 1.5*cos(ugao_krug), 1.5*sin(ugao_krug));
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
		gluCylinder(quad, 0.7, 0.7, 2, 20, 10);

	glPopMatrix();

/*Pravi se spirala*/
	glPushMatrix();

/*Transliramo je ispod sivog valjka i za spust koji menja tajmer i rotiramo za ugao koji se menja tajmerom*/
		glTranslatef(0, -1.5+spust, 0);
		glRotatef(-ugao, 0, 1, 0);

		float ugao_spirale, ugao_valjka, precnik_valjka = 0.25, precnik_spirale = 0.4, krug = 2*PI;
		float centar_valjkaX0 = precnik_spirale, centar_valjkaY0 = 0, centar_valjkaZ0 = 0;
		float centar_valjkaX1 = precnik_spirale*cos(korakUgao), centar_valjkaY1 = -korakY, centar_valjkaZ1 = precnik_spirale*sin(korakUgao);

/*Spiralu pravimo tako sto pravimo krugove poluprecnika = precnik_valjka koji su paralelni sa y = 0 ravni i 
njihovi centri kad se povezu prave spiralu poluprecnika = precnik spirale i spusta po y-osi korakY. Tacke tih krugova povezujemo sa GL_TRIANGLE_STRIP*/
		glBegin(GL_TRIANGLE_STRIP);
		for(ugao_spirale = 0; ugao_spirale <= 10*krug + korakUgao; ugao_spirale += korakUgao)
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
}

void otvarac2(GLUquadric* quad)
{
/*Disejblujemo blend da bi otvarac bio dobar*/
	glDisable(GL_BLEND);

	/*Postavlja se materijal za plasticnu drsku otvaraca spirale*/
	ambient_coeffs[0] = 0.5;
	ambient_coeffs[1] = 0.5;
	ambient_coeffs[2] = 0.5;
	ambient_coeffs[3] = 1;

	diffuse_coeffs[0] = 0.8;
	diffuse_coeffs[1] = 0.8;
	diffuse_coeffs[2] = 0.8;
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

/*Drska spirale se translira na dole po y-osi za spust spirale koji se menja tajmerom i rotira za ugao koji se menja tajmerom*/
		glTranslatef(0, spust_spirale, 0);
		glRotatef(-ugao, 0, 1, 0);

/*Pravi se drska spirale koja je kvadar*/
	float duzina = 9.0, visina = 3.0, sirina = 2.0;
		glBegin(GL_POLYGON);
			glNormal3f(0, 0, sirina/2);
			glVertex3f(-duzina/2, -visina/2, sirina/2);
			glVertex3f(duzina/2, -visina/2, sirina/2);
			glVertex3f(duzina/2, visina/2, sirina/2);
			glVertex3f(-duzina/2, visina/2, sirina/2);
		glEnd();

		glBegin(GL_POLYGON);
			glNormal3f(0, 0, -sirina/2);
			glVertex3f(-duzina/2, -visina/2, -sirina/2);
			glVertex3f(-duzina/2, visina/2, -sirina/2);
			glVertex3f(duzina/2, visina/2, -sirina/2);
			glVertex3f(duzina/2, -visina/2, -sirina/2);
		glEnd();

		glBegin(GL_POLYGON);
			glNormal3f(duzina/2, 0, 0);
			glVertex3f(duzina/2, -visina/2, sirina/2);
			glVertex3f(duzina/2, visina/2, sirina/2);
			glVertex3f(duzina/2, visina/2, -sirina/2);
			glVertex3f(duzina/2, -visina/2, -sirina/2);
		glEnd();

		glBegin(GL_POLYGON);
			glNormal3f(-duzina/2, 0, 0);
			glVertex3f(-duzina/2, -visina/2, sirina/2);
			glVertex3f(-duzina/2, visina/2, sirina/2);
			glVertex3f(-duzina/2, visina/2, -sirina/2);
			glVertex3f(-duzina/2, -visina/2, -sirina/2);
		glEnd();

		glBegin(GL_POLYGON);
			glNormal3f(0, -visina/2, 0);
			glVertex3f(-duzina/2, -visina/2, sirina/2);
			glVertex3f(duzina/2, -visina/2, sirina/2);
			glVertex3f(duzina/2, -visina/2, -sirina/2);
			glVertex3f(-duzina/2, -visina/2, -sirina/2);
		glEnd();

		glBegin(GL_POLYGON);
			glNormal3f(0, visina/2, 0);
			glVertex3f(-duzina/2, visina/2, sirina/2);
			glVertex3f(duzina/2, visina/2, sirina/2);
			glVertex3f(duzina/2, visina/2, -sirina/2);
			glVertex3f(-duzina/2, visina/2, -sirina/2);
		glEnd();

/*Pravi se baza za spiralu prikacena za drsku u obliku pljosnatog valjka; prave se i dva kruga da zatvore valjak; rotira se i translira u polozaj i za ugao rotacije i za spust spirale*/
	glTranslatef(0, -visina/2+0.5, 0);
	float ugao_krug;

		glBegin(GL_POLYGON);
			for(ugao_krug = 0; ugao_krug <= 2*PI + PI/20; ugao_krug += PI/20)
			{
				glNormal3f(0, 1, 0);
				glVertex3f(duzina*cos(ugao_krug)/3, 0, duzina*sin(ugao_krug)/3);
			}
		glEnd();

		glBegin(GL_POLYGON);
			for(ugao_krug = 0; ugao_krug <= 2*PI + PI/20; ugao_krug += PI/20)
			{
				glNormal3f(0, -1, 0);
				glVertex3f(duzina*cos(ugao_krug)/3, -0.5, duzina*sin(ugao_krug)/3);
			}
		glEnd();
	glRotatef(90, 1, 0, 0);
	gluCylinder(quad, duzina/3, duzina/3, 0.5, 20, 10);

	glPopMatrix();

/*Pravi se spirala; postavlja se materijal za spiralu*/
	ambient_coeffs[0] = 0.1;
	ambient_coeffs[1] = 0.1;
	ambient_coeffs[2] = 0.1;
	ambient_coeffs[3] = 1;

	diffuse_coeffs[0] = 0.3;
	diffuse_coeffs[1] = 0.3;
	diffuse_coeffs[2] = 0.3;
	diffuse_coeffs[3] = 1;

	specular_coeffs[0] = 0.9;
	specular_coeffs[1] = 0.9;
	specular_coeffs[2] = 0.9;
	specular_coeffs[3] = 0.9;

	shininess = 5;

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	glPushMatrix();

/*Transliramo je ispod baznog valjka drske i za spust spirale koji menja tajmer i rotiramo za ugao koji se menja tajmerom*/
		glTranslatef(0, -visina/2+spust_spirale, 0);
		glRotatef(-ugao, 0, 1, 0);

		float ugao_spirale, ugao_valjka, precnik_valjka = 0.25, precnik_spirale = 0.4, krug = 2*PI;
		float centar_valjkaX0 = precnik_spirale, centar_valjkaY0 = 0, centar_valjkaZ0 = 0;
		float centar_valjkaX1 = precnik_spirale*cos(korakUgao), centar_valjkaY1 = -korakY, centar_valjkaZ1 = precnik_spirale*sin(korakUgao);

/*Spiralu pravimo tako sto pravimo krugove poluprecnika = precnik_valjka koji su paralelni sa y = 0 ravni i 
njihovi centri kad se povezu prave spiralu poluprecnika = precnik spirale i spusta po y-osi korakY. Tacke tih krugova povezujemo sa GL_TRIANGLE_STRIP*/
		glBegin(GL_TRIANGLE_STRIP);
		for(ugao_spirale = 0; ugao_spirale <= 10*krug + korakUgao; ugao_spirale += korakUgao)
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


/*Postavlja se isti materijal kao za drsku spirale, samo za telo otvaraca*/
	ambient_coeffs[0] = 0.5;
	ambient_coeffs[1] = 0.5;
	ambient_coeffs[2] = 0.5;
	ambient_coeffs[3] = 1;

	diffuse_coeffs[0] = 0.8;
	diffuse_coeffs[1] = 0.8;
	diffuse_coeffs[2] = 0.8;
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

/*Pravi se telo otvaraca: malo je kompleksnijeg oblika, bice dostupna slika pravog*/
		glTranslatef(0, -visina/2+spust_tela, 0);
		glBegin(GL_POLYGON);
			for(ugao_krug = 0; ugao_krug <= 2*PI + PI/20; ugao_krug += PI/20)
			{
				glNormal3f(0, 1, 0);
				glVertex3f(duzina*cos(ugao_krug)/3, 0, duzina*sin(ugao_krug)/3);
			}
		glEnd();

		glBegin(GL_POLYGON);
			for(ugao_krug = 0; ugao_krug <= 2*PI + PI/20; ugao_krug += PI/20)
			{
				glNormal3f(0, -1, 0);
				glVertex3f(duzina*cos(ugao_krug)/3, -1, duzina*sin(ugao_krug)/3);
			}
		glEnd();

		glRotatef(90, 1, 0, 0);
			gluCylinder(quad, duzina/3, duzina/3, 1, 20, 10);
		glRotatef(-90, 1, 0, 0);

		glTranslatef(0, -sqrt(2)*duzina/6, 0);
		glutSolidCube(sqrt(2)*duzina/3);
		glTranslatef(0, sqrt(2)*duzina/6, 0);

		double clip_plane0[] = {1, 0, 0, -sqrt(2)*duzina/6};
		double clip_plane1[] = {-1, 0, 0, -sqrt(2)*duzina/6};

		glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, clip_plane0);
			glRotatef(90, 1, 0, 0);
			gluCylinder(quad, duzina/3, duzina/3, fabs(centar_valjkaY1), 20, 10);
			glRotatef(-90, 1, 0, 0);
   		glDisable(GL_CLIP_PLANE0);

		glEnable(GL_CLIP_PLANE1);
			glClipPlane(GL_CLIP_PLANE1, clip_plane1);
			glTranslatef(-sqrt(2)*duzina/6, -1, 0);
			glRotatef(90, 1, 0, 0);
			gluCylinder(quad, duzina/6, duzina/6, fabs(centar_valjkaY1)-1, 20, 10);
			glRotatef(-90, 1, 0, 0);
			glTranslatef(sqrt(2)*duzina/6, 1, 0);
   		glDisable(GL_CLIP_PLANE1);

		glTranslatef(0, -2.5*sqrt(2)*duzina/3, 0);
			glRotatef(90, 1, 0, 0);
			gluCylinder(quad, duzina/3, duzina/3, fabs(centar_valjkaY1)-2.5*sqrt(2)*duzina/3, 20, 10);
			glRotatef(-90, 1, 0, 0);

	glPopMatrix();

glEnable(GL_BLEND);
}

