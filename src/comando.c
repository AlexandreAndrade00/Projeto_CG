#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
void drawBase(int texture);
void drawAnalogic();
void drawHomeButton();
void drawSquareButton();
void drawRightJoy();
void drawLeftJoy();
void drawCube(int texture);
void drawPlus();
void drawMinus();
void desenhaTexto(char* string);
void Loop();
void texturas();
void initLights();
void materialLightProperties(GLfloat material[4][3]);

GLint wScreen = 1920, hScreen = 1080;
GLfloat	xC = 10.0, yC = 10.0, zC = 10.0;
GLfloat rVisao = 10, aVisao = 0.5 * PI, incVisao = 0.05;
GLfloat obsP[3];
GLfloat angZoom = 45;
GLfloat incZoom = 3;
GLfloat rotateLeft1=0, rotateLeft2=0, posx=0.0, posy=0.0;
GLuint startList;
GLUquadricObj *qobj;
GLuint texture_list[NUM_TEXTURE];

static GLfloat cubo[] = {
    //front
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    // back
    0.0f, 0.0f, -1.0f,
    1.0f, 0.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    0.0f, 1.0f, -1.0f,
    // right
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 0.0f,
    // left
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 1.0f, -1.0f,
    0.0f, 1.0f, 0.0f,
    // top
    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, -1.0f,
    0.0f, 1.0f, -1.0f,
    // bottom
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
};

static GLfloat texturas_cubo[] = {
    0, 0, 
    1, 0, 
    1, 1, 
    0, 1, 
    0, 0,
    1, 0,
    1, 1,
    0, 1,
    0, 0,
    1, 0,
    1, 1,
    0, 1,
    0, 0, 
    1, 0, 
    1, 1, 
    0, 1,
    0, 0, 
    1, 0, 
    1, 1, 
    0, 1,
    0, 0, 
    1, 0, 
    1, 1, 
    0, 1
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

    glClearColor(0.75, 0.75, 0.75, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glShadeModel(GL_SMOOTH);

    glVertexPointer(3, GL_FLOAT, 0, cubo);
	glEnableClientState(GL_VERTEX_ARRAY);

    glTexCoordPointer(2, GL_FLOAT, 0, texturas_cubo);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    obsP[0]=rVisao * cos(aVisao);
    obsP[1]=3.0;
    obsP[2]=rVisao * sin(aVisao);

    texturas();
    initLights();

}


void texturas() {
    int width, height, nrChannels;
    unsigned char *data;
    char* texture_name[] = {"images/a_button.png", "images/x_button.png", "images/y_button.png", "images/b_button.png",
                            "images/right_button.png", "images/up_button.png", "images/left_button.png", "images/down_button.png",
                            "images/joystick_1.png", "images/blue_plastic.png", "images/red_plastic.png", "images/glass.png"};

    for (int i=0; i<NUM_TEXTURE; i++) {
        glGenTextures(1, &texture_list[i]);
        glBindTexture(GL_TEXTURE_2D, texture_list[i]);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
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

    drawAxis();
    drawObjects();

    glViewport(0, 0, wScreen/4, hScreen/4);
    glLoadIdentity();
    gluLookAt(-4, 0, 1, -4, 2, 0, 0, 1, 0);
    
    drawObjects();

    glFlush();
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

	glPushMatrix();
    glTranslatef(4, 0, 0);
	glRotatef(90, 1, 0, 0);
	drawRightJoy();
	glPopMatrix();


    glPushMatrix();
    glTranslatef(-4, 0, 0);
	glRotatef(90, 1, 0, 0);
	drawLeftJoy();
	glPopMatrix();

    glPushMatrix();
    glTranslatef(-3.25, -2.25, 0);
    glScalef(6.5, 4.2, -0.2);
    drawCube(-2);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0, 1, 1);
    glTranslatef(posx, posy, 0.3);
    glScalef(0.2, 0.2, 1);
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glPopMatrix();

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
   }

   glutPostRedisplay();
}

void drawRightJoy() {
	glPushMatrix();

	glTranslatef(0, 0.3, -1.1);
    materialLightProperties(blackPlastic);
    drawButtons(0);
	glPopMatrix();

	glPushMatrix();
	glColor4f(0.85, 0.117647059, 0.094117647, 1);
    materialLightProperties(redPlastic);
	drawBase(-1);
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
}


void drawLeftJoy() {
    glPushMatrix();
    
	glTranslatef(0, 0.3, 0.3);
    materialLightProperties(blackPlastic);
    drawButtons(4);
	glPopMatrix();

	glPushMatrix();
	glColor4f(0, 0.70, 1, 1);
    materialLightProperties(cyanPlastic);
	drawBase(-1);
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
    glTranslatef(0.1, 0.2, 1.16);
    drawSquareButton();
    glPopMatrix();

	glPopMatrix();
}

void drawCircle(int n, float radius, int textureIndex) {

    if (textureIndex!=-1) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture_list[textureIndex]);
    }

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

    if (textureIndex!=-1)
        glDisable(GL_TEXTURE_2D);
}

void drawButtons(int offset) {

	glPushMatrix();

	glColor3f(0, 0, 0);
    materialLightProperties(blackPlastic);
    glRotatef(90, 1, 0, 0);

    glPushMatrix();
	glTranslatef(0.3, 0, 0);
    gluCylinder(qobj, 0.15, 0.15, 0.1, 30, 5);
	drawCircle(100, 0.15, 0+offset);
    glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.3, 0, 0);
    gluCylinder(qobj, 0.15, 0.15, 0.1, 30, 5);
	drawCircle(100, 0.15, 2+offset);
    glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0.3, 0);
    gluCylinder(qobj, 0.15, 0.15, 0.1, 30, 5);
	drawCircle(100, 0.15, 3+offset);
    glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -0.3, 0);
    gluCylinder(qobj, 0.15, 0.15, 0.1, 30, 5);
	drawCircle(100, 0.15,1+offset);
    glPopMatrix();

	glPopMatrix();
}

void drawBase(int texture) {
	glPushMatrix();
	glTranslatef(-0.75, 0, 2.25);
	glScalef(1.5, 0.2, 4.2);
	drawCube(texture);
	glPopMatrix();
}

void drawAnalogic() {

    materialLightProperties(blackPlastic);

	glPushMatrix();
	glColor3f(0, 0, 0);
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, -0.55);
    gluCylinder(qobj, 0.3, 0.1, 0.1, 30, 5);
	drawCircle(100, 0.3, 8);
    glPopMatrix();

	glPushMatrix();
	glColor3f(0, 0, 0);
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
    glScalef(0.25, 0.05, 0.25);
    drawCube(-1);
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

void drawCube(int texture) {
    GLuint front[] =  {3, 2, 1, 0};
    GLuint back[] =  {7, 6, 5, 4};
    GLuint right[] =  {11, 10, 9, 8};
    GLuint left[] =  {15, 14, 13, 12};
    GLuint top[] =  {19, 18, 17, 16};
    GLuint bottom[] =  {23, 22, 21, 20};

    if (texture>-1) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture_list[texture]);
    }

    if (texture==-2) {
        materialLightProperties(screen);
        glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, back);
        materialLightProperties(blackPlastic);
    } else 
        glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, back);
    glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, front);
    glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, right);
    glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, left);
    glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, top);
    glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, bottom);

    if (texture>-1 && texture !=11)
        glDisable(GL_TEXTURE_2D);
}

void drawPlus() {
	glPushMatrix();
	glColor3f(0, 0, 0);
    materialLightProperties(blackPlastic);

	glPushMatrix();
	glScalef(0.2, 0.05, 0.05);
	drawCube(-1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.075, 0, 0.075);
	glScalef(0.05, 0.05, 0.2);
	drawCube(-1);
	glPopMatrix();
}


void drawMinus() {
	glPushMatrix();
	glColor3f(0, 0, 0);
    materialLightProperties(blackPlastic);

	glPushMatrix();
	glScalef(0.2, 0.05, 0.05);
	drawCube(-1);
	glPopMatrix();

	glPopMatrix();
}


void initLights() {
    GLfloat ambientColor[] = {0.1, 0.1, 0.1, 0.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    GLfloat light0_pos[] = {2.0, -2.0, 5.0, 1.0};
    GLfloat light0_colorAmb[] = {0, 0, 0, 0.0};
    GLfloat light0_colorDif[] = {1.0, 1.0, 1.0, 0};
    GLfloat light0_colorSpec[] = {1.0, 1.0, 1.0, 0};
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_colorAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_colorDif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_colorSpec);
}


void materialLightProperties(GLfloat material[4][3]) {
    glMaterialfv(GL_FRONT, GL_AMBIENT, material[0]);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material[1]);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material[2]);
    glMaterialf(GL_FRONT, GL_SHININESS, material[3][0]);
}
