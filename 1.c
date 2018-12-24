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

/*Ugao za rotiranje otvaraca*/
float ugao = 0;

int main(int argc, char **argv)
{
/*Inicijalizujem GLUT-a*/
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
/*Pravim prozor*/
    glutInitWindowSize(1200, 1200);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
/*Postavljanje callback funkcija*/
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);

    timer_active = 0;

    glClearColor(1, 1, 1, 0);
    glEnable(GL_DEPTH_TEST);

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
        if (!timer_active) {
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
/*Simulacija se vraca na pocetak (r->RESET)*/
		ugao = 0;
    	glutPostRedisplay();
		break;
    }
}

static void on_timer(int value)
{
/* Proverava se da li poziv dolazi od naseg tajmera*/
    if (value != 0)
        return;

	if(ugao >= 360)
	{
		ugao = ugao - 360;
	}
	ugao = ugao + 1;

    glutPostRedisplay();

    if (timer_active)
        glutTimerFunc(10, on_timer, 0);
}

static void on_reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 30);
}

static void on_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(1, 1, 15, 0, 0, 0, 0, 1, 0);

	glPushMatrix();
/*Pravi se braonkast valjak koji treba da predstavlja drveni drzac otvaraca*/
		GLUquadric* quad = gluNewQuadric();
		glColor3f(0.5, 0.2, 0.2);
		glRotatef(ugao, 0, 1, 0);
		glTranslatef(-4.5, 0, 0);
		glRotatef(90, 0, 1, 0);
		gluCylinder(quad, 2, 2, 9, 20, 10);
		glRotatef(-90, 0, 1, 0);
		glTranslatef(4.5, 0, 0);
/*prave se dva kruga da zatvore valjak*/
		float ugao_krug;
		glColor3f(0.5, 0.2, 0.2);
		glBegin(GL_POLYGON);
			for(ugao_krug = 0; ugao_krug <= 2*PI + PI/20; ugao_krug += PI/20)
			{
				glVertex3f(-4.5, 2*cos(ugao_krug), 2*sin(ugao_krug));
			}
		glEnd();

		glColor3f(0.5, 0.2, 0.2);
		glBegin(GL_POLYGON);
			for(ugao_krug = 0; ugao_krug <= 2*PI + PI/20; ugao_krug += PI/20)
			{
				glVertex3f(4.5, 2*cos(ugao_krug), 2*sin(ugao_krug));
			}
		glEnd();
/*Pravi se sivi valjak koji je pocetak metalnog dela, pre spirale*/
		glColor3f(0.5, 0.5, 0.5);
		glTranslatef(0, -1.5, 0);
		glRotatef(90, 1, 0, 0);
		gluCylinder(quad, 0.5, 0.5, 4, 20, 10);

	glPopMatrix();
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
