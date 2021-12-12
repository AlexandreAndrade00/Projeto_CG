#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "materiais.h"

#define PI 3.14159
#define NUM_TEXTURE 12

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void start();
void display();
void drawAxis();
void drawObjects();
void teclasNotAscii();
void keyboard(unsigned char key, int x, int y);
void drawCircle(int n, float radius, int textureIndex);
void drawButtons(int offset);
void drawBase();
void drawAnalogic();
void drawHomeButton();
void drawSquareButton();
void drawRightJoy();
void drawLeftJoy();
void drawCube();
void drawPlus();
void drawMinus();
void desenhaTexto(char* string);
void Loop();
void texturas();
void initLights();
void materialLightProperties(GLfloat material[4][4]);
void turn_on_off_lights();
void change_light_color();
void draw_grelha_poligonos();
void change_light_pos();
void quad(int a,int b,int c,int d);
void updateLightDirection();
void drawMalhas();
void drawGlassBox();

GLint wScreen = 1920, hScreen = 1080;
GLfloat	xC = 10.0, yC = 10.0, zC = 10.0;
GLfloat rVisao = 10, aVisao = 0.5 * PI, incVisao = 0.05;
GLfloat obsP[3];
GLfloat angZoom = 45;
GLfloat incZoom = 3;
GLfloat rotateLeft1=0, rotateLeft2=0, posx=0.0, posy=0.0;
GLfloat incFocoX=0, incFocoY=0;
GLuint startList;
GLUquadricObj *qobj;
GLuint texture_list[NUM_TEXTURE];
bool light1_pos;
bool light0_flag;
const GLfloat tam = 1;

GLfloat ver[8][3] = 
{
    {-1.0,-1.0,1.0},
    {-1.0,1.0,1.0},
    {1.0,1.0,1.0},
    {1.0,-1.0,1.0},
    {-1.0,-1.0,-1.0},
    {-1.0,1.0,-1.0},
    {1.0,1.0,-1.0},
    {1.0,-1.0,-1.0},
};


int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(wScreen, hScreen);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Comando Nintendo Switch   Teclas: 'W' 'S' 'A' 'D' e setas");

	start();
    glutSpecialFunc(teclasNotAscii);
	glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutTimerFunc(10, Loop, 0);
	glutMainLoop();

	return 0;
}


void start() {
    
    qobj = gluNewQuadric();

    glClearColor(0.75, 0.75, 0.75, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_SMOOTH);

    obsP[0]=rVisao * cos(aVisao);
    obsP[1]=3.0;
    obsP[2]=rVisao * sin(aVisao);

    light1_pos = 0;

    initLights();
    texturas();
}


void texturas() {
    int width, height, nrChannels;
    unsigned char *data;
    char* texture_name[] = {"images/a_button.png", "images/x_button.png", "images/y_button.png", "images/b_button.png",
                            "images/right_button.png", "images/up_button.png", "images/left_button.png", "images/down_button.png",
                            "images/joystick_1.png", "images/blue_plastic.png", "images/red_plastic.png", "images/price_tag.png"};

    for (int i=0; i<NUM_TEXTURE; i++) {
        glGenTextures(1, &texture_list[i]);
        glBindTexture(GL_TEXTURE_2D, texture_list[i]);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // load and generate the texture
        data = stbi_load(texture_name[i], &width, &height, &nrChannels, 0);
        if (data!=NULL) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else {
            printf("Failed to load image %s!\n", texture_name[i]);
        }
        stbi_image_free(data);
    }
}


void Loop() {
    posx-=((GLfloat) rotateLeft1)/300.0;
    if (posx < -3.10)   posx = -3.10;
    if (posx > 2.85)   posx = 2.85;
    posy-=((GLfloat) rotateLeft2)/300.0;
    if (posy < -2)   posy = -2;
    if (posy > 1.6)   posy = 1.6;
    glutPostRedisplay();
    glutTimerFunc(10, Loop, 0);
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angZoom, (float)wScreen / hScreen, 0.1, 3 * zC);
	glMatrixMode(GL_MODELVIEW);

    glViewport(0, 0, wScreen, hScreen);
	glLoadIdentity();
	gluLookAt(obsP[0],obsP[1],obsP[2], 0, 0, 0, 0, 1, 0);

    drawObjects();

    glViewport(0, 0, wScreen/4, hScreen/4);
    glLoadIdentity();
    gluLookAt(-4, 0, 1, -4, 2, 0, 0, 1, 0);
    
    drawObjects();
    glLoadIdentity();

    glutSwapBuffers();
}


void drawAxis() {
    glColor4f(1, 0, 0, 1);
    materialLightProperties(redPlastic);
	glBegin(GL_LINES);
		glVertex3i(0, 0, 0);
		glVertex3i(10, 0, 0);
	glEnd();

	glColor4f(0, 1, 0, 1);
    materialLightProperties(cyanPlastic);
	glBegin(GL_LINES);
		glVertex3i(0, 0, 0);
		glVertex3i(0, 10, 0);
	glEnd();

	glColor4f(0, 0, 1, 1);
    materialLightProperties(blackPlastic);
	glBegin(GL_LINES);
		glVertex3i(0, 0, 0);
		glVertex3i(0, 0, 10);
	glEnd();
}


void drawObjects() {

	drawRightJoy();
	drawLeftJoy();
    drawMalhas();

    glPushMatrix();
    glTranslatef(0, -0.5, 0);
    glScalef(3.7, 2.5, 0.2);
    materialLightProperties(blackPlastic);
    drawCube();
    glPopMatrix();

    glPushMatrix();
    glColor3f(0, 1, 1);
    materialLightProperties(white);
    glTranslatef(posx, posy, 0.21);
    glScalef(0.2, 0.2, 1);
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glPopMatrix();

    drawGlassBox();
}


void teclasNotAscii(int key) {

	if (key == GLUT_KEY_UP)   obsP[1] = (obsP[1] + 0.1);
	if (key == GLUT_KEY_DOWN) obsP[1] = (obsP[1] - 0.1);
	if (key == GLUT_KEY_LEFT)  aVisao = (aVisao + 0.1);
	if (key == GLUT_KEY_RIGHT) aVisao = (aVisao - 0.1);

	if (obsP[1] > yC)   obsP[1] = yC;   // limita altura
	if (obsP[1] < -yC)  obsP[1] = -yC;

	obsP[0] = rVisao * cos(aVisao);      // actualiza posicao (X,Z)
	obsP[2] = rVisao * sin(aVisao);

	glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y) {
   switch (key) {
        case 27:
            exit(0);
            break;

        case 'a':
        case 'A':
            if (rotateLeft1 < 20)
                rotateLeft1+=3;
            break;

        case 'd':
        case 'D':
            if (rotateLeft1 > -20)
                rotateLeft1-=3;
            break;

        case 'w':
        case 'W':
            if (rotateLeft2 > -20)
                rotateLeft2-=3;
            break;

        case 's':
        case 'S':
            if (rotateLeft2 < 20)
                rotateLeft2+=3;
            break;
        case 'i':
        case 'I':
            incFocoY += 0.01;
            updateLightDirection();
            break;
        case 'k':
        case 'K':
            incFocoY -= 0.01;
            updateLightDirection();
            break;
        case 'l':
        case 'L':
            incFocoX += 0.01;
            updateLightDirection();
            break;
        case 'j':
        case 'J':
            incFocoX -= 0.01;
            updateLightDirection();
            break;
        case 'o':
        case 'O':
            turn_on_off_lights();
            break;

        case 'c':
        case 'C':
            change_light_color();
            break;
        case 'm':
        case 'M':
            change_light_pos();
            break;

   }

    glutPostRedisplay();
}

void drawRightJoy() {

    glPushMatrix();

    glTranslatef(4.5, 0, 0);

    glPushMatrix();
	glColor4f(0.85, 0.117647059, 0.094117647, 1);
    materialLightProperties(redPlastic);
    glTranslatef(0, -0.5, 0);
	drawBase();
	glPopMatrix();

    glPushMatrix();

    glRotatef(90,1, 0, 0);

    glPushMatrix();
	glTranslatef(0, 0.3, -1.1);
    materialLightProperties(blackPlastic);
    drawButtons(0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, 0.3);
    materialLightProperties(blackPlastic);
	drawAnalogic();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.3, 0.3, 1.05);
	drawHomeButton();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.6, 0.2, -1.6);
    materialLightProperties(blackPlastic);
	drawPlus();
	glPopMatrix();

	glPopMatrix();

    glPopMatrix();
}


void drawLeftJoy() {

    glPushMatrix();

    glTranslatef(-4.5, 0, 0);

    glPushMatrix();
	glColor4f(0, 0.70, 1, 1);
    materialLightProperties(cyanPlastic);
    glTranslatef(0, -0.5, 0);
	drawBase();
	glPopMatrix();
    
    glPushMatrix();

    glRotatef(90, 1, 0, 0);

    glPushMatrix();
	glTranslatef(0, 0.3, 0.3);
    materialLightProperties(blackPlastic);
    drawButtons(4);
    glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, -1.1);
    glRotatef(rotateLeft2, 1, 0, 0);
    glRotatef(rotateLeft1, 0, 0, 1);
	drawAnalogic();
	glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4, 0.2, -1.6);
    drawMinus();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.1, 0.2, 1.34);
    drawSquareButton();
    glPopMatrix();

	glPopMatrix();

    glPopMatrix();
}

void drawMalhas() {

    glPushMatrix();
    materialLightProperties(screen);
    glTranslatef(-3.5, -2.8, 0.205);
    glScalef(3.5, 2.3, 1);
    draw_grelha_poligonos();
    glPopMatrix();

    glPushMatrix();
    materialLightProperties(redPlastic);
    glTranslatef(3.7, -3, 0.205);
    glScalef(0.8, 2.5, 1);
    draw_grelha_poligonos();
    glPopMatrix();

    glPushMatrix();
    materialLightProperties(cyanPlastic);
    glTranslatef(-5.3, -3, 0.205);
    glScalef(0.8, 2.5, 1);
    draw_grelha_poligonos();
    glPopMatrix();

}

void drawCircle(int n, float radius, int textureIndex) {

    if (textureIndex!=-1) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture_list[textureIndex]);
    }

    glPushMatrix();
    glNormal3f(0, 0, -1);
    glBegin(GL_POLYGON);
    for (int i = 0; i < n; i++) {
        float x = radius * cos(i * PI * (360 / (float) n) / 180.0);
        float y = radius * sin(i * PI * (360 / (float) n ) / 180.0);
        float const tx = (x/radius + 1)*0.5;
        float const ty = (y/radius + 1)*0.5;

        if (textureIndex!=-1)
            glTexCoord2f(tx, ty);

        glVertex2f(x, y);
    }
    glEnd();
    //glNormal3f(0, 0, 1);
    glPopMatrix();

    if (textureIndex!=-1)
        glDisable(GL_TEXTURE_2D);
}

void drawButtons(int offset) {

	glPushMatrix();

	glColor3f(0, 0, 0);
    glRotatef(90, 1, 0, 0);

    glPushMatrix();
	glTranslatef(0.3, 0, 0);
    materialLightProperties(blackPlastic);
    gluCylinder(qobj, 0.15, 0.15, 0.1, 30, 5);
    materialLightProperties(white);
	drawCircle(100, 0.15, 0+offset);
    glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.3, 0, 0);
    materialLightProperties(blackPlastic);
    gluCylinder(qobj, 0.15, 0.15, 0.1, 30, 5);
    materialLightProperties(white);
	drawCircle(100, 0.15, 2+offset);
    glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0.3, 0);
    materialLightProperties(blackPlastic);
    gluCylinder(qobj, 0.15, 0.15, 0.1, 30, 5);
    materialLightProperties(white);
	drawCircle(100, 0.15, 3+offset);
    glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -0.3, 0);
    materialLightProperties(blackPlastic);
    gluCylinder(qobj, 0.15, 0.15, 0.1, 30, 5);
    materialLightProperties(white);
	drawCircle(100, 0.15,1+offset);
    glPopMatrix();

	glPopMatrix();
}

void drawBase() {
	glPushMatrix();
	glScalef(0.8, 2.5, 0.2);
	drawCube();
	glPopMatrix();
}

void drawAnalogic() {

    materialLightProperties(blackPlastic);

	glPushMatrix();
	glColor3f(0, 0, 0);
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, -0.55);
    gluCylinder(qobj, 0.3, 0.1, 0.1, 30, 5);
    materialLightProperties(white);
	drawCircle(100, 0.3, 8);
    glPopMatrix();

	glPushMatrix();
	glColor3f(0, 0, 0);
    materialLightProperties(blackPlastic);
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, -0.45);
	gluCylinder(qobj, 0.09, 0.09, 0.2, 30, 5);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslatef(0, 0.2, 0);
	glScalef(1, 0.5, 1);
	gluSphere(qobj, 0.2, 30, 5);
	glPopMatrix();

}

void drawHomeButton() {

	glPushMatrix();
	glColor3f(0, 0, 0);
    materialLightProperties(blackPlastic);
    glRotatef(90, 1, 0, 0);
    gluCylinder(qobj, 0.15, 0.15, 0.1, 30, 5);
	glColor4f(0.5, 0.5, 0.5, 1);
    materialLightProperties(greyPlastic);
	drawCircle(100, 0.15, -1);
	glTranslatef(0, 0, -0.001);
	glColor3f(0, 0, 0);
    materialLightProperties(blackPlastic);
	drawCircle(100, 0.11, -1);
    glPopMatrix();
}

void drawSquareButton() {

    glPushMatrix();

    glPushMatrix();
    glTranslatef(0.11, 0, -0.11);
    glScalef(0.12, 0.05, 0.12);
    drawCube();
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.2, 0.2, 0.2);
    materialLightProperties(blackPlastic);
    glTranslatef(0.12, 0.06, -0.12);
    glRotatef(90, 1, 0, 0);
    drawCircle(100, 0.09, -1);
    glPopMatrix();

    glPopMatrix();
}

void drawCube() {
    glPushMatrix();
    
    glNormal3f(0, 0, 1);
    quad(0,3,2,1);
    //glNormal3f(1, 0, 0);
    quad(2,3,7,6);
    //glNormal3f(0, -1, 0);
    quad(0,4,7,3);
    //glNormal3f(0, 1, 0);
    quad(1,2,6,5);
    //glNormal3f(0, 0, -1);
    quad(4,5,6,7);
    //glNormal3f(-1, 0, 0);
    quad(0,1,5,4);
    
    glPopMatrix();
}

void drawPlus() {
	glPushMatrix();
        glColor3f(0, 0, 0);
        materialLightProperties(blackPlastic);

        glPushMatrix();
            glScalef(0.1, 0.05, 0.03);
            drawCube();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.01, 0, 0.01);
            glScalef(0.03, 0.05, 0.1);
            drawCube();
        glPopMatrix();
    glPopMatrix();
}


void drawMinus() {
	glPushMatrix();
        glColor3f(0, 0, 0);
        materialLightProperties(blackPlastic);

        glPushMatrix();
            glScalef(0.1, 0.05, 0.03);
            drawCube();
        glPopMatrix();

	glPopMatrix();
}

void drawGlassBox() {

    glPushMatrix();
    materialLightProperties(trans);
    glTranslatef(0, -0.5, 0);
    glScalef(6, 3, 1);
    drawCube();
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_list[11]);
    glPushMatrix();
    glNormal3f(0, 0, 1);
    materialLightProperties(white);
    glTranslatef(-5.4, 2, 0.01);
    glScalef(0.6, 0.5, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3fv(ver[0]);
    glTexCoord2f(1, 0);
    glVertex3fv(ver[3]);
    glTexCoord2f(1, 1);
    glVertex3fv(ver[2]);
    glTexCoord2f(0, 1);
    glVertex3fv(ver[1]);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

}

void quad(int a,int b,int c,int d) {
    glBegin(GL_QUADS);
    glVertex3fv(ver[a]);
    glVertex3fv(ver[b]);
    glVertex3fv(ver[c]);
    glVertex3fv(ver[d]);
    glEnd();
}


void draw_grelha_poligonos() {
    glPushMatrix();
    glNormal3f(0, 0, 1);
    GLint dim = 512;   //numero divisoes da grelha
    int	i, j;
	float med_dim = (float)dim / 2;
	glBegin(GL_QUADS);
	for (i = 0; i < dim; i++)
		for (j = 0; j < dim; j++) {
			glVertex3d((float)j / med_dim, (float)i / med_dim, 0);
			glVertex3d((float)(j + 1) / med_dim, (float)i / med_dim, 0);
			glVertex3d((float)(j + 1) / med_dim, (float)(i + 1) / med_dim, 0);
			glVertex3d((float)j / med_dim, (float)(i + 1) / med_dim, 0);
		}
	glEnd();
    glPopMatrix();
}


void initLights() {
    GLfloat ambientColor[] = {0.0, 0.0, 0.0, 0.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    GLfloat light0_pos[] = {0.0, 0.0, 20.0, 0.0};
    GLfloat light0_colorDif[] = {0.5, 0.5, 0.5, 1};
    GLfloat light0_colorSpec[] = {1.0, 1.0, 1.0, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_colorDif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_colorSpec);

    GLfloat light1_pos[] = {0.0, 0.0, 1, 1.0};
    GLfloat light1_colorDif[] = {1.0, 1.0, 1.0, 1};
    GLfloat Foco_direccao[] = {incFocoX, incFocoY, -1, 0 };		
	GLfloat Foco_ak = 1.0;
	GLfloat Foco_al = 0.05f;
	GLfloat Foco_aq = 0.0f;
	GLfloat Foco_Expon = 2.0;		// Foco, SPOT_Exponent
    GLfloat	aberturaFoco = 10.0;		//.. angulo inicial do foco
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_colorDif);
    /* glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, Foco_ak);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, Foco_al);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, Foco_aq); */
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, aberturaFoco);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, Foco_direccao);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, Foco_Expon);


    /* glEnable(GL_LIGHT0);
    light0_flag = 1; */
    glEnable(GL_LIGHT1); 
}


void materialLightProperties(GLfloat material[4][4]) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material[0]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material[1]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material[2]);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material[3][0]);
}

void turn_on_off_lights() {
    if (light0_flag == 1) {
        glDisable(GL_LIGHT0);
        light0_flag = 0;
    } else {
        glEnable(GL_LIGHT0);
        light0_flag = 1;
    }
}


void change_light_color() {
    GLfloat light1_colorDif[] = {random()/(float)(RAND_MAX), random()/(float)(RAND_MAX), random()/(float)(RAND_MAX), 1};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_colorDif);
}

void change_light_pos() {
    
    if (light1_pos == 0) {
        GLfloat light1_poss[] = {0.0, 0.0, -20.0, 0};
        light1_pos = 1;
        glLightfv(GL_LIGHT0, GL_POSITION, light1_poss);
    } else {
        GLfloat light1_poss[] = {0.0, 0.0, 20.0, 0};
        glLightfv(GL_LIGHT0, GL_POSITION, light1_poss);
        light1_pos = 0;
    }

    GLfloat light0_colorDif[] = {0.5, 0.5, 0.5, 1};
    GLfloat light0_colorSpec[] = {1.0, 1.0, 1.0, 1};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_colorDif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_colorSpec);

}


void updateLightDirection() {
    GLfloat Foco_direccao[] = {incFocoX, incFocoY, -1, 0 };		
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, Foco_direccao);
}