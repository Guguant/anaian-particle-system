#pragma
#include "OpenGLDemo.h"


// GLUT header
#include <stdlib.h>
#include <OpenGL\glut.h>    // OpenGL GLUT Library Header

// Open file dialog
#include "LoadFileDlg.h"

// The GLM code for loading and displying OBJ mesh file
#include "glm.h"

// The trackball code for rotating the model
#include "trackball.h"

// The size of the GLUT window
int window_width  = 800;
int window_height = 600;

// The OBJ model
GLMmodel* pModel = NULL;

// The current modelview matrix
double pModelViewMatrix[16];

// If mouse left button is pressed
bool bLeftBntDown = false;

// Old position of the mouse
int OldX = 0;
int OldY = 0;


#define MAX_PARTICLES 	1000//����������Ŀ

float slowdown = 3.0f;
float xspeed;//X���ٶ� 
float yspeed;//Y���ٶ�
float zoom = -0.0f;//z������
int a;
//float xg1 = 0.0f;
//float yg1 = 0.2f;
GLuint loop;//ѭ������
GLuint col = 1;//��ʼ��ɫ
//GLuint texture[1];//��������洢����


typedef struct//�������Խṹ��
{
	bool active;//
	float life;//������������
	float fade;//��ʧ���ٶ�

	float r;//r
	float g;//g
	float b;//b

	float x;//����
	float y;//����
	float z;//����

	float xi;//x�ٶ�
	float yi;//y�ٶ�
	float zi;//z�ٶ�

	float xg;//X����ٶ�
	float yg;//Y����ٶ�
	float zg;//Z����ٶ�
}particles;
particles particle[MAX_PARTICLES];//������������


static GLfloat colors[11][3] = //��ɫ����
{ { 1.0f,0.0f,0.0f },{ 1.0f,0.7f,0.2f },{ 0.0f,1.0f,0.0f },{ 0.0f,0.0f,1.0f },
{ 1.0f,0.5f,0.75f },{ 0.35f,0.8f,0.5f },{ 0.55f,0.25f,0.75f },{ 0.5f,0.5f,0.75f },
{ 1.0f,1.0f,1.0f },{ 1.0f,0.5f,0.8f },{ 0.2f,0.1f,0.9f } };

int main(int argc, char* argv[])
{
	// Initialize the GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("��ߵ���");
	init();

	// Set the callback function
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();

	return 0;
}

/// Initialize the OpenGL
void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluPerspective(45.0f, (float)window_width / (float)window_height, 1.0f, 200.0);

	glClearColor(0.1, 0.1, 0.1, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	/********************************/
	//  �������صĲ���
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_NORMALIZE);
 // 
	//glEnable(GL_CULL_FACE);
 //   
 //   // Setup other misc features.
 //   //glEnable( GL_LIGHTING );   �������ջ�Ӱ�쵽���ӵ���ɫ
 //   glEnable( GL_NORMALIZE );
 //   glShadeModel( GL_SMOOTH );

 //   
 //    //Setup lighting model.
	//GLfloat light_model_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
 //   GLfloat light0_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
 //   GLfloat light0_direction[] = {0.0f, 0.0f, 10.0f, 0.0f};
	//GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	//glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, light_model_ambient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
 //   glEnable( GL_LIGHT0 );
	/*************************************/


	for (loop = 0; loop < MAX_PARTICLES; loop++)
	{
		//���� ����ֽ
		particle[loop].active = true;
		
		//���� ���ʱ��
		particle[loop].life = 2.0f;

		//����˥���ٶ�
		particle[loop].fade = float(rand() % 100) / 200.0f + 0.002f;

		//�ı���ɫ
		particle[loop].r = colors[loop*(4 / MAX_PARTICLES)][0];
		particle[loop].g = colors[loop*(4 / MAX_PARTICLES)][1];
		particle[loop].b = colors[loop*(4 / MAX_PARTICLES)][2];

		//�ı��ٶ�
		particle[loop].xi = float((rand() % 50) - 26.0f)*4.0f;
		particle[loop].yi = float((rand() % 50) - 26.0f)*4.0f;
		particle[loop].zi = float((rand() % 50) - 26.0f)*4.0f;

		//������ٶ�
		particle[loop].xg = 0.1f;
		particle[loop].yg = 0.1f;
		particle[loop].zg = 0.1f;
	}
	// Init the dlg of the open file
	PopFileInitialize( NULL );
}

/// Display the Object
void display()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 5, 0, 1, 0, 0, 0, 1);  //�����
	//position of the eye point; position of the reference point; direction of the up vector
                
	glTranslated( 0.3, 0.5, 1.0);    //ƽ�� x,y,z
	glMultMatrixd( pModelViewMatrix );

	if ( pModel )
	{
		glmDraw( pModel, GLM_FLAT );  //����ģ��
	}
	for (loop = 0; loop < MAX_PARTICLES; loop++)
	{
		if (particle[loop].active)
		{
			float x = particle[loop].x;
			float y = particle[loop].y;
			float z = particle[loop].z + zoom;
			glColor4f(particle[loop].r, particle[loop].g, particle[loop].b, particle[loop].life);
			//���ӵ���ɫ��lifeԽС��Խ������ 

			glBegin(GL_QUADS);												//���������ı���
			glVertex3f(x +0.0f, y + 0.0f, z);
			glVertex3f(x + 0.03f, y + 0.0f, z);
			glVertex3f(x + 0.03f, y + 0.03f, z);
			glVertex3f(x + 0.0f, y + 0.03f, z);
			glEnd();
			//glFlush();   �����е㿨


			particle[loop].x += particle[loop].xi / (slowdown * 500);  //�������ٶ� �仯      
			particle[loop].y += particle[loop].yi / (slowdown * 500);
			particle[loop].z += particle[loop].zi / (slowdown * 500);

			particle[loop].xi += particle[loop].xg;     // ���ٶȱ仯  
			particle[loop].yi += particle[loop].yg;
			particle[loop].zi += particle[loop].zg;

			particle[loop].life -= particle[loop].fade;   // ����˥��         
			if (particle[loop].life < 0.0f)  //��ʼ������
			{
				particle[loop].life = 2.0f;
				particle[loop].fade = float(rand() % 100) / 60.0f + 0.002f;//˥���ٶ�
				
				//��ʼ������λ��
				particle[loop].x = 0.70f;
				particle[loop].y = 0.80f;
				particle[loop].z = 0.00f;

				//��ʼ�������ٶ�
				particle[loop].xi = float((rand() % 10) - 5.0f);
				particle[loop].yi = float((rand() % 10) - 5.0f);
				particle[loop].zi = float((rand() % 10) - 5.0f);

				//��ʼ����ɫ
				particle[loop].r = colors[rand()%10][0];
				particle[loop].g = colors[rand() % 10][1];
				particle[loop].b = colors[rand() % 10][2];

				//��ʼ�����ٶ�  ����
				particle[loop].xg = 0.001f;
				particle[loop].yg = 0.001f;
				particle[loop].zg = 0.001f;
			}
		}
	}
	glutSwapBuffers(); //˫����
}


/// Reshape the Window
void reshape(int w, int h)
{
	// Update the window's width and height
	window_width  = w;
	window_height = h;

	// Reset the viewport
	glViewport(0, 0, window_width, window_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)window_width/(float)window_height, 1.0f, 200.0);
	
	glutPostRedisplay();
}

/// Keyboard Messenge
void keyboard(unsigned char key, int x, int y)
{
	// The obj file will be loaded
	char FileName[128] = "";
	char TitleName[128]= "";

	// Center of the model
	float modelCenter[] = {0.0f, 0.0f, 0.0f};
	//float modelCenter[] = { 0.0f, 10.0f, 10.0f };

	switch ( key )
	{
	case 'o':
	case 'O':
		PopFileOpenDlg( NULL, FileName, TitleName );

		// If there is a obj model has been loaded, destroy it
		if ( pModel )
		{
			glmDelete( pModel );
			pModel = NULL;
		}

		// Load the new obj model
		pModel = glmReadOBJ( FileName );
		
		// Generate normal for the model
		glmFacetNormals( pModel );

		// Scale the model to fit the screen
		glmUnitize( pModel, modelCenter );

		// Init the modelview matrix as an identity matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glGetDoublev( GL_MODELVIEW_MATRIX, pModelViewMatrix );	

		break;

	case '+':
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		glLoadMatrixd( pModelViewMatrix );
		glScaled( 1.05, 1.05, 1.05 );
		glGetDoublev( GL_MODELVIEW_MATRIX, pModelViewMatrix );
		break;

	case '-':
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		glLoadMatrixd( pModelViewMatrix );
		glScaled( 0.95, 0.95, 0.95 );
		glGetDoublev( GL_MODELVIEW_MATRIX, pModelViewMatrix );
		break;

	case '1':
	{
		col = 1;
		glutPostRedisplay();//�ػ溯��
	}break;
	case '2':
	{
		col = 2;
		glutPostRedisplay();
	}break;
	case '3':
	{
		col = 3;
		glutPostRedisplay();
	}break;
	case '4':
	{
		col = 4;
		glutPostRedisplay();
	}break;
	case '5':
	{
		col = 5;
		glutPostRedisplay();
	}break;
	case '6':
	{
		col = 6;
		glutPostRedisplay();
	}break;
	case '7':
	{
		col = 7;
		glutPostRedisplay();
	}break;
	case '8':
	{
		col = 8;
		glutPostRedisplay();
	}break;
	case '9':
	{
		col = 9;
		glutPostRedisplay();
	}break;

	case 'Q':
	{
		col = 10;
		glutPostRedisplay();
	}break;

	case 'W':
	{
		col = 11;
		glutPostRedisplay();
	}break;

	default:
		break;
	}

	glutPostRedisplay();
}

/// Mouse Messenge
void mouse(int button, int state, int x, int y)
{
	if ( pModel )
	{
		if ( state==GLUT_DOWN && button==GLUT_LEFT_BUTTON )
		{
			OldX = x;
			OldY = y;
			bLeftBntDown = true;
		}
		else if ( state==GLUT_DOWN && button==GLUT_LEFT_BUTTON )
		{
			bLeftBntDown = false;
		}
	}
}

/// Motion Function
void motion(int x, int y)
{
	if ( bLeftBntDown && pModel )
	{
		float fOldX =  3.0f*OldX/(float)window_width  - 2.0f;
		float fOldY = -3.0f*OldY/(float)window_height + 2.0f;
		float fNewX =  3.0f*x/(float)window_width  - 2.0f;
		float fNewY = -3.0f*y/(float)window_height + 2.0f;

		double pMatrix[16];
		trackball_opengl_matrix( pMatrix, fOldX, fOldY, fNewX, fNewY);

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		glLoadMatrixd( pMatrix );
		glMultMatrixd( pModelViewMatrix );
		glGetDoublev( GL_MODELVIEW_MATRIX, pModelViewMatrix );

		OldX = x;
		OldY = y;
		glutPostRedisplay();
	}
}

/// Idle function
void idle(void)
{
	glutPostRedisplay();
}