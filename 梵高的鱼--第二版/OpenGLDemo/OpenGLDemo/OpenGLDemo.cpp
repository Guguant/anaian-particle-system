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


#define MAX_PARTICLES 	1000//粒子数的数目

float slowdown = 3.0f;
float xspeed;//X轴速度 
float yspeed;//Y轴速度
float zoom = -0.0f;//z轴缩放
int a;
//float xg1 = 0.0f;
//float yg1 = 0.2f;
GLuint loop;//循环变量
GLuint col = 1;//初始颜色
//GLuint texture[1];//创建数组存储纹理


typedef struct//粒子属性结构体
{
	bool active;//
	float life;//粒子生命周期
	float fade;//消失的速度

	float r;//r
	float g;//g
	float b;//b

	float x;//坐标
	float y;//坐标
	float z;//坐标

	float xi;//x速度
	float yi;//y速度
	float zi;//z速度

	float xg;//X轴加速度
	float yg;//Y轴加速度
	float zg;//Z轴加速度
}particles;
particles particle[MAX_PARTICLES];//创建粒子数组


static GLfloat colors[11][3] = //颜色数组
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
	glutCreateWindow("梵高的鱼");
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
	//  与光照相关的操作
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_NORMALIZE);
 // 
	//glEnable(GL_CULL_FACE);
 //   
 //   // Setup other misc features.
 //   //glEnable( GL_LIGHTING );   开启光照会影响到粒子的颜色
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
		//粒子 生命纸
		particle[loop].active = true;
		
		//粒子 存活时间
		particle[loop].life = 2.0f;

		//粒子衰减速度
		particle[loop].fade = float(rand() % 100) / 200.0f + 0.002f;

		//改变颜色
		particle[loop].r = colors[loop*(4 / MAX_PARTICLES)][0];
		particle[loop].g = colors[loop*(4 / MAX_PARTICLES)][1];
		particle[loop].b = colors[loop*(4 / MAX_PARTICLES)][2];

		//改变速度
		particle[loop].xi = float((rand() % 50) - 26.0f)*4.0f;
		particle[loop].yi = float((rand() % 50) - 26.0f)*4.0f;
		particle[loop].zi = float((rand() % 50) - 26.0f)*4.0f;

		//赋予加速度
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
	gluLookAt(0, 0, 5, 0, 1, 0, 0, 0, 1);  //照相机
	//position of the eye point; position of the reference point; direction of the up vector
                
	glTranslated( 0.3, 0.5, 1.0);    //平移 x,y,z
	glMultMatrixd( pModelViewMatrix );

	if ( pModel )
	{
		glmDraw( pModel, GLM_FLAT );  //绘制模型
	}
	for (loop = 0; loop < MAX_PARTICLES; loop++)
	{
		if (particle[loop].active)
		{
			float x = particle[loop].x;
			float y = particle[loop].y;
			float z = particle[loop].z + zoom;
			glColor4f(particle[loop].r, particle[loop].g, particle[loop].b, particle[loop].life);
			//粒子的颜色，life越小，越不明显 

			glBegin(GL_QUADS);												//线性连续四边形
			glVertex3f(x +0.0f, y + 0.0f, z);
			glVertex3f(x + 0.03f, y + 0.0f, z);
			glVertex3f(x + 0.03f, y + 0.03f, z);
			glVertex3f(x + 0.0f, y + 0.03f, z);
			glEnd();
			//glFlush();   加上有点卡


			particle[loop].x += particle[loop].xi / (slowdown * 500);  //坐标随速度 变化      
			particle[loop].y += particle[loop].yi / (slowdown * 500);
			particle[loop].z += particle[loop].zi / (slowdown * 500);

			particle[loop].xi += particle[loop].xg;     // 加速度变化  
			particle[loop].yi += particle[loop].yg;
			particle[loop].zi += particle[loop].zg;

			particle[loop].life -= particle[loop].fade;   // 周期衰减         
			if (particle[loop].life < 0.0f)  //初始化粒子
			{
				particle[loop].life = 2.0f;
				particle[loop].fade = float(rand() % 100) / 60.0f + 0.002f;//衰减速度
				
				//初始化粒子位置
				particle[loop].x = 0.70f;
				particle[loop].y = 0.80f;
				particle[loop].z = 0.00f;

				//初始化粒子速度
				particle[loop].xi = float((rand() % 10) - 5.0f);
				particle[loop].yi = float((rand() % 10) - 5.0f);
				particle[loop].zi = float((rand() % 10) - 5.0f);

				//初始化颜色
				particle[loop].r = colors[rand()%10][0];
				particle[loop].g = colors[rand() % 10][1];
				particle[loop].b = colors[rand() % 10][2];

				//初始化加速度  匀速
				particle[loop].xg = 0.001f;
				particle[loop].yg = 0.001f;
				particle[loop].zg = 0.001f;
			}
		}
	}
	glutSwapBuffers(); //双缓冲
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
		glutPostRedisplay();//重绘函数
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