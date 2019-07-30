#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

#define	WIRE 0		
#define	SHADE 1		
#define PI 3.141592

GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */
GLfloat	light_specular[] = {1.0, 1.0, 1.0, 1.0}; /* specular light */
GLfloat	light_ambient[] = {0.3, 0.3, 0.3, 1.0};  /* ambient light */

typedef struct {
	float x;
	float y;
	float z;
} Point;

typedef struct {
	unsigned long ip[3];
} Face;

int pnum;
int fnum;
Point *mpoint;
Face *mface;

GLfloat angle1 = -150;   /* in degrees */
GLfloat angle2 = -150;
GLfloat light_angle1 = -150;   /* in degrees */
GLfloat light_angle2 = -150;   /* in degrees */

float lookat_x, lookat_y, lookat_z = 1500;

GLfloat xloc = 0, yloc = 0, zloc = 0;
int moving, beginx, beginy;
int light_moving;
float scalefactor = 1.0;
int scaling = 0;         
int status=0;           // WIRE or SHADE
int cull=0;             // CULLING toggle 
char *fname="cube.dat";

void DrawWire(void)
{
//	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // �ȼ����� ���� �� ����
	//3D �� 2D�� ǥ���ϱ� ���� ���
	glEnable (GL_DEPTH_TEST); // ���� Ȱ��ȭ
	glEnable (GL_LIGHTING); // ���� Ȱ��ȭ
	glEnable (GL_LIGHT0); // ���� ����
	glEnable(GL_COLOR_MATERIAL); // �� Ȱ��ȭ
	if (cull) glEnable(GL_CULL_FACE); // �޸� Ȱ��ȭ
	else glDisable(GL_CULL_FACE); // �޸� ��Ȱ��ȭ

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // �ո�� �޸��� �� ǥ��
	glCallList(1); // ������� ����Ʈ ȣ��
	glutSwapBuffers(); // ��� ���ۿ� ����� ����
}

void DrawShade(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	if (cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(1);
	glutSwapBuffers();
}

Point cnormal(Point a, Point b, Point c)
{
    Point p, q, r;
    double val;
    p.x = a.x-b.x; p.y = a.y-b.y; p.z = a.z-b.z;
    q.x = c.x-b.x; q.y = c.y-b.y; q.z = c.z-b.z;

    r.x = p.y*q.z - p.z*q.y;
    r.y = p.z*q.x - p.x*q.z;
    r.z = p.x*q.y - p.y*q.x;

    val = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);
    r.x = r.x / val;
    r.y = r.y / val;
    r.z = r.z / val;
    return r;
}

void MakeGL_Model(void)
{
	int i;
	Point norm;	//�������� �븻 ����
	glShadeModel(GL_SMOOTH);  // �ε巴�� ���̵�

	glPushMatrix();	// ��� ���ÿ� Ǫ��
	glRotatef(light_angle1, 0.0, 1.0, 0.0); //y������ ���� ȸ��
	glRotatef(light_angle2, 1.0, 0.0, 0.0); //x������ ���� ȸ��

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); //���ݻ� ����
	glLightfv(GL_LIGHT0, GL_POSITION, light_specular);// ���ݻ� ����
	glLightfv(GL_LIGHT0, GL_POSITION, light_ambient);// �ֺ��� ����
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);//���� ��ġ ����
	glEnable(GL_LIGHT0);// ������ LIGHT0 ��� ����
	glEnable(GL_LIGHTING);//������ ��
	glPopMatrix();//��� ���ÿ��� POP

	if (glIsList(1)) glDeleteLists(1, 1); // �� ����Ʈ ����
	glNewList(1, GL_COMPILE);
	glPushMatrix();
	glTranslatef(xloc, yloc, zloc); //��ü �̵� ��ȯ

	glRotatef(angle1, 0.0, 1.0, 0.0);//��ü ȸ�� ��ȯ y��
	glRotatef(angle2, 1.0, 0.0, 0.0);//��ü ȸ�� ��ȯ x��
    glScalef(scalefactor, scalefactor, scalefactor);//��ü ũ�� ��ȯ
	for (i = 0; i < fnum; i++) {
		norm = cnormal(mpoint[mface[i].ip[2]], mpoint[mface[i].ip[1]],
			mpoint[mface[i].ip[0]]);//�� normal ���
		glBegin(GL_TRIANGLES); // �ﰢ������ �� ����
		glNormal3f(norm.x, norm.y, norm.z); // �� normal ����
		glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y,
			mpoint[mface[i].ip[0]].z); // 1��° �� ����
		glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y,
			mpoint[mface[i].ip[1]].z); // 2��° �� ����
		glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y,
			mpoint[mface[i].ip[2]].z); // 3��° �� ����
		glEnd();
	}

	glPopMatrix(); // ��� ���ÿ��� POP
	glEndList();
}

void GLSetupRC(void) 
{
  /* Enable a single OpenGL light. */
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); // ���ݻ� ����
  glLightfv(GL_LIGHT0, GL_POSITION, light_specular); // ���ݻ� ����
  glLightfv(GL_LIGHT0, GL_POSITION, light_ambient);  // �ֺ��� ����
  glLightfv(GL_LIGHT0, GL_POSITION, light_position); // ������ ��ġ ����
  glEnable(GL_LIGHT0);	// ������ LIGHT0 ��� ����
  glEnable(GL_LIGHTING);// ������ ������� ��


  /* Use depth buffering for hidden surface elimination. */
  glEnable(GL_DEPTH_TEST);	// ���� �׽�Ʈ ��

  /* Setup the view */
  glMatrixMode(GL_PROJECTION);		// �������� ����
  glLoadIdentity();					//�������� ��� �ʱ�ȭ -> ���� ��ķ� ��ġ
  
  gluPerspective(  40.0, // field of view in degree 
  1.0, // aspect ratio 
  1.0, // Z near 
  2000.0); // Z far 
  glMatrixMode(GL_MODELVIEW);	// �ð���ǥ�� UVN ����
  glLoadIdentity();
  gluLookAt(400.0, 400.0, 400.0,  // eye is at (0,0,5) 
    0.0, 0.0, 0.0,      // center is at (0,0,0) 
    0.0, 1.0, 0.);      // up is in positive Y direction 
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	MakeGL_Model();
	if (status==WIRE) DrawWire();
	else DrawShade();
}

void ReadModel()
{
	FILE *f1;
	char s[81];

	int i;

	if ((f1 = fopen(fname, "rt"))== NULL) {
		printf("No file\n");
		exit(0);
	}
	fscanf(f1,"%s",s);     printf( "%s", s ); // VERTEX
	fscanf(f1,"%s",s);     printf( "%s", s ); // =
	fscanf(f1,"%d",&pnum);     printf( "%d\n", pnum); //vertex�� ����

    mpoint = (Point*)malloc(sizeof(Point)*pnum); //vertex ������ŭ Point �Ҵ�

	for (i=0; i<pnum; i++){
		fscanf(f1,"%f",&mpoint[i].x); //x��ǥ ����
		fscanf(f1,"%f",&mpoint[i].y); //y��ǥ ����
		fscanf(f1,"%f",&mpoint[i].z); //z��ǥ ����
	    printf( "%f %f %f\n", mpoint[i].x, mpoint[i].y,mpoint[i].z);
	}

	fscanf(f1,"%s",s);     printf( "%s", s ); // FACE
	fscanf(f1,"%s",s);     printf( "%s", s ); // =
	fscanf(f1,"%d",&fnum);     printf( "%d\n", fnum); // FACE ���� ����

	mface = (Face*)malloc(sizeof(Face)*fnum); // FACE ������ Face �Ҵ�
	for (i=0; i<fnum; i++){
		fscanf(f1,"%d",&mface[i].ip[0]); // 1��° ���� �ε��� ����
		fscanf(f1,"%d",&mface[i].ip[1]); // 2��° ���� �ε��� ����
		fscanf(f1,"%d",&mface[i].ip[2]); // 3��° ���� �ε��� ����
	    printf("%d %d %d\n", mface[i].ip[0], mface[i].ip[1], mface[i].ip[2]);
	}
	fclose(f1);
}

void mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    moving = 1;
	beginx = x;
	beginy = y;
  }
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    moving = 0;
  }

  if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
	scaling = 1;
	beginx = x;
	beginy = y;
  }

  if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
      scaling = 0;
  }
  if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
	  scaling = 1; // �ε巴�� 1�� ����
	  beginx = x;
	  beginy = y;
  }

  if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
	  scaling = 0;
  }

  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
	scaling=0;
	light_moving = 1;
	beginx = x;
	beginy = y;
  }
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
	  light_moving = 0;
  }
}

void motion(int x, int y)
{
  if (scaling) {
	  scalefactor = scalefactor * (1.0 + (beginx - x)*0.0001);
	  scalefactor = scalefactor * (1.0 + (beginy - y)*0.0001);
    glutPostRedisplay();
  }
  if (moving) {
	  angle1 = angle1 + (x - beginx);
	  angle2 = angle2 + (beginy - y);
	  beginx = x;
	  beginy = y;
    glutPostRedisplay();
  }
  if (light_moving) {
	  light_angle1 = light_angle1 + (x - beginx);
	  light_angle2 = light_angle2 + (beginy - y);
	  beginx = x;
	  beginy = y;
    glutPostRedisplay();
  }
}


void transLocation(int key, int x, int y)  //����Ű�Է��� ���� �Լ�
{
	switch (key) {
	case GLUT_KEY_RIGHT:
		xloc += 10;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		zloc -= 10;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		zloc += 10;
		glutPostRedisplay();
		break;
	case GLUT_KEY_PAGE_UP:
		yloc += 10;
		glutPostRedisplay();
		break;
	case GLUT_KEY_PAGE_DOWN:
		yloc -= 10;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		xloc -= 10;
		glutPostRedisplay();
		break;
	}

}



void SORModel()
{
	int i;
	int IncAngle = 1;

	pnum = (360 / IncAngle) * 2;
	mpoint = (Point*)malloc(sizeof(Point)*pnum);

	mpoint[0].x = 100;
	mpoint[0].y = -100;
	mpoint[0].z = 100;
	mpoint[1].x = 100;
	mpoint[1].y = 100;
	mpoint[1].z = 100;

	for (i = 0; i<pnum - 2; i++) {
		mpoint[i + 2].x = cos(IncAngle*PI / 180)*mpoint[i].x - sin(IncAngle*PI / 180)*mpoint[i].z;
		mpoint[i + 2].y = mpoint[i].y;
		mpoint[i + 2].z = sin(IncAngle*PI / 180)*mpoint[i].x + cos(IncAngle*PI / 180)*mpoint[i].z;
	}

	fnum = 2 * 360 / IncAngle;
	mface = (Face*)malloc(sizeof(Face)*fnum);
	for (i = 0; i<pnum; i += 2) {
		mface[i].ip[0] = i%pnum;
		mface[i].ip[1] = (i + 1) % pnum;
		mface[i].ip[2] = (i + 1 + 2) % pnum;

		mface[i + 1].ip[0] = i%pnum;
		mface[i + 1].ip[1] = (i + 1 + 2) % pnum;
		mface[i + 1].ip[2] = (i + 2) % pnum;

	}
}

void waterBottle()
{
	int i, j;
	int IncAngle = 5;			// ȸ�� ���� ���� (5���� ȸ��)
	pnum = (360 / IncAngle) * 17; 		// ���� ����
	mpoint = (Point*)malloc(sizeof(Point)*pnum); // ���� ���� �Ҵ�
	mpoint[0].x = 0; mpoint[0].y = 100; mpoint[0].z = 0;    // 1��° ���� �ʱ� ��ġ
	mpoint[1].x = 15;	mpoint[1].y = 100; mpoint[1].z = 0; // 2��° 
	mpoint[2].x = 20;	mpoint[2].y = 95;	mpoint[2].z = 10;  // 3��° 
	mpoint[3].x = 20;	mpoint[3].y = 75;	mpoint[3].z = 10;  // 4��° 
	mpoint[4].x = 15;	mpoint[4].y = 75; mpoint[4].z = 0;  // 5��°
	mpoint[5].x = 15;	mpoint[5].y = 70; mpoint[5].z = 0;  // 6��°
	mpoint[6].x = 20; mpoint[6].y = 35; mpoint[6].z = 10;   // 7��°
	mpoint[7].x = 23; mpoint[7].y = 30; mpoint[7].z = 20;  // 8��° 
	mpoint[8].x = 28; mpoint[8].y = 25; mpoint[8].z = 25;  // 9��°
	mpoint[9].x = 35; mpoint[9].y = 0; mpoint[9].z = 35;  // 10��°
	mpoint[10].x = 40; mpoint[10].y = -40;	mpoint[10].z = 40; // 11��° 
	mpoint[11].x = 40; mpoint[11].y = -90;	mpoint[11].z = 35; // 12��° 
	mpoint[12].x = 35; mpoint[12].y = -190;	mpoint[12].z = 40; // 13��° 
	mpoint[13].x = 32; mpoint[13].y = -193;	mpoint[13].z = 35; // 14��°
	mpoint[14].x = 28; mpoint[14].y = -197;	mpoint[14].z = 30; // 15��°
	mpoint[15].x = 26; mpoint[15].y = -200;	mpoint[15].z = 28; // 16��° 
	mpoint[16].x = 0; mpoint[16].y = -200;	mpoint[16].z = 0; // 17��° 


	for (i = 0; i<pnum - 17; i++) {		// ȸ���� ���� �� ��ġ ���
		mpoint[i + 17].x = cos(IncAngle*PI / 180)*mpoint[i].x - sin(IncAngle*PI / 180)*mpoint[i].z;
		mpoint[i + 17].y = mpoint[i].y;
		mpoint[i + 17].z = sin(IncAngle*PI / 180)*mpoint[i].x + cos(IncAngle*PI / 180)*mpoint[i].z;
	}
	fnum = 32 * 360 / IncAngle;		// ���� ����
	mface = (Face*)malloc(sizeof(Face)*fnum);      // ���� ���� �Ҵ�
	for (i = 0, j = 0; i<pnum - 1; i++, j += 2) {		// ���� ����� �� ����
		if (i % 17 == 16) i++;
		mface[j].ip[0] = i%pnum;
		mface[j].ip[1] = (i + 1) % pnum;
		mface[j].ip[2] = (i + 1 + 17) % pnum;
		mface[j + 1].ip[0] = i%pnum;
		mface[j + 1].ip[1] = (i + 1 + 17) % pnum;
		mface[j + 1].ip[2] = (i + 17) % pnum;
	}

}

void cupModel()
{
	int i, j;
	int IncAngle = 5;			// ȸ�� ���� ���� (5���� ȸ��)
	pnum = (360 / IncAngle) * 7; 		// ���� ����
	mpoint = (Point*)malloc(sizeof(Point)*pnum); // ���� ���� �Ҵ�
	mpoint[0].x = 0;	mpoint[0].y = 25;	mpoint[0].z = 0;
	mpoint[1].x = 100;	mpoint[1].y = 25;	mpoint[1].z = 0;
	mpoint[2].x = 100;	mpoint[2].y = 150;	mpoint[2].z = 0;
	mpoint[3].x = 110;	mpoint[3].y = 165;	mpoint[3].z = 0; //���� ���� ����
	mpoint[4].x = 110;	mpoint[4].y = 25;	mpoint[4].z = 0;
	mpoint[5].x = 105;	mpoint[5].y = 0;	mpoint[5].z = 0;
	mpoint[6].x = 0;	mpoint[6].y = 0;	mpoint[6].z = 0;


	for (i = 0; i<pnum - 7; i++) {		// ȸ���� ���� �� ��ġ ���
		mpoint[i + 7].x = cos(IncAngle*PI / 180)*mpoint[i].x - sin(IncAngle*PI / 180)*mpoint[i].z;
		mpoint[i + 7].y = mpoint[i].y;
		mpoint[i + 7].z = sin(IncAngle*PI / 180)*mpoint[i].x + cos(IncAngle*PI / 180)*mpoint[i].z;
	}
	fnum = 12 * 360 / IncAngle;		// ���� ����
	mface = (Face*)malloc(sizeof(Face)*fnum);      // ���� ���� �Ҵ�
	for (i = 0, j = 0; i<pnum - 1; i++, j += 2) {		// ���� ����� �� ����
		if (i % 7 == 6) i++;
		mface[j].ip[0] = i%pnum;
		mface[j].ip[1] = (i + 1) % pnum;
		mface[j].ip[2] = (i + 1 + 7) % pnum;
		mface[j + 1].ip[0] = i%pnum;
		mface[j + 1].ip[1] = (i + 1 + 7) % pnum;
		mface[j + 1].ip[2] = (i + 7) % pnum;
	}



}

void keyboard(unsigned char key, int x, int y)
{
	printf("key %d\n", key);
	switch (key)
	{
	case 'w':
		status = WIRE;
		glutPostRedisplay();
		break;
	case 's':
		status = SHADE;
		glutPostRedisplay();
		break;
	case 'c':
		if (cull) cull = 0;
		else cull = 1;
		glutPostRedisplay();
		break;
	case '1':
		fname = "cube.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '2':
		fname = "sphere.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '3':
		fname = "teapot.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '4':
		fname = "plane.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '5':
		SORModel();
		glutPostRedisplay();
		break;
	case '6':
		waterBottle();
		glutPostRedisplay();
		break;
	case '7':
		fname = "chair.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '8':
		cupModel();
		glutPostRedisplay();
		break;
	case 'h':
		lookat_x -= 100;
		glLoadIdentity();
		gluLookAt(lookat_x, lookat_y, lookat_z, 0, 0, 0, 0, 1, 0);
		glutPostRedisplay();
		break;
	case 'k':
		lookat_x += 100;
		glLoadIdentity();
		gluLookAt(lookat_x, lookat_y, lookat_z, 0, 0, 0, 0, 1, 0);
		glutPostRedisplay();
		break;
	case 'u':
		lookat_z -= 100;
		glLoadIdentity();
		gluLookAt(lookat_x, lookat_y, lookat_z, 0, 0, 0, 0, 1, 0);
		glutPostRedisplay();
		break;
	case 'j':
		lookat_z += 100;
		glLoadIdentity();
		gluLookAt(lookat_x, lookat_y, lookat_z, 0, 0, 0, 0, 1, 0);
		glutPostRedisplay();
		break;
	case 'y':
		lookat_y -= 100;
		glLoadIdentity();
		gluLookAt(lookat_x, lookat_y, lookat_z, 0, 0, 0, 0, 1, 0);
		glutPostRedisplay();
		break;
	case 'i':
		lookat_y += 100;
		glLoadIdentity();
		gluLookAt(lookat_x, lookat_y, lookat_z, 0, 0, 0, 0, 1, 0);
		glutPostRedisplay();
		break;

	}
}



int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(400,400);
	glutInitWindowPosition(400,400);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // ���� ���� ���
	glutCreateWindow("Simple Modeling");
	glutDisplayFunc(display);	// display() : Rendering Loop ���� ��� �Ҹ�
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutSpecialFunc(transLocation);
	ReadModel();
	GLSetupRC();
	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}

