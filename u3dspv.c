/* Copyright (c) 2013, Bartosz Foder, (bartosz@foder.pl)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice,
*   this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "u3dspv.h"

#if defined (WIN32)
 
#include <windows.h>
typedef void* u3d_thread_t;
typedef void u3d_th_ret_t;

#else
 
#include <pthread.h>
typedef pthread_t* u3d_thread_t;
typedef void* u3d_th_ret_t;

#include <unistd.h>
void Sleep(int ms) {
usleep(ms*1000); //convert to microseconds
return;
}

#endif

#include <GL/glut.h> 
#include <GL/gl.h>	
#include <GL/glu.h>	

u3d_thread_t visthread;

// window title
char title[256];
//ss{X} - space size in direction {X}
float ssx, ssy, ssz;
//os{X} - offset ind direction {X}  
float osx, osy, osz;
//rot{x|y} - rotation in direction {x|y}
//zzom - zoom factor in Z direction
float xrot,yrot,zzoom;
//array for data
double*** tab;
//non - number of nodes
int non;
//itr - number of iterations
int itr;
//window - windows descr
int window;
//tab_itr - current iteration;
int tab_itr;
//done - indicate end of visualization
int done;
//data_changed - indicate changes in data between threads
int* data_changed;
//scal - scalling factor
int scal;
//lists - array of openGL list descriptors
GLuint* lists;
//t1 - timestamp for computing fps
time_t t1;
//frames frames counter
int frames;
//delay - delay beetween frames changes in play_loop mode
double delay;
//play_loop - pLay_loop mode indicator
int play_loop;
//dtime - time od last frame change in play_loop mode
clock_t dtime;
// thread function
u3d_th_ret_t window_worker(void* args);
//initial lock for load data
int* initlock;

int DLL_EXPORT U3D_INIT_(int* _non,int* _itr)
{
    int i, j;
	
	initlock = (int*)malloc(sizeof(int));
	(*initlock) = 1;
    
	data_changed = (int*)malloc(sizeof(int));
	(*data_changed) = 0;

	non = (*_non);
    itr = (*_itr);
    tab_itr = -1;

    done = 0;
    scal = 1;

    xrot= 0.0f;
    yrot =0.0f;
    zzoom = 0.0f;

	play_loop = 0;

	U3D_SET_TITLE_("Universal 3D Space Properties Visualizator",255);
    //table allocation
    tab = (double***)malloc(itr * sizeof(double**));
    if(tab == NULL)
    {
        fprintf(stderr, "out of memory\n");
        return -1;
    }
    
    for ( i = 0 ; i < itr ; i++ )
    {
        tab[i] = (double**)malloc(8 * sizeof(double*));
        if(tab[i] == NULL)
        {
            fprintf(stderr, "out of memory\n");
            return -1;
        }
        for (j = 0 ; j < 7 ; j++)
        {
            tab[i][j] = (double*)malloc(non * sizeof(double));
            if(tab[i][j] == NULL)
            {
                fprintf(stderr, "out of memory\n");
                return -1;
            }
			memset(tab[i][j],0.0,sizeof(tab[i][j]));
        }
        //metadata min & max for each tab[i][0..6]
        tab[i][7] = (double*)malloc(14 * sizeof(double));
        if(tab[i][7] == NULL)
        {
            fprintf(stderr, "out of memory\n");
            return -1;
        }
        memset(tab[i][7],0.0,sizeof(tab[i][7]));
    }

    lists = (GLuint*)malloc(itr * sizeof(GLuint));
    if(lists == NULL)
    {
        fprintf(stderr, "out of memory\n");
        return -1;
    }
    
#if defined (WIN32)
	visthread = _beginthread(window_worker,0,NULL);

	while ((*initlock) == 1){Sleep(100);}

	return 0;
#else
	visthread = (u3d_thread_t) malloc(sizeof(u3d_thread_t));
    return pthread_create(visthread,NULL,window_worker,NULL);
#endif
}

void DLL_EXPORT U3D_JOIN_()
{
    done = 1;
#if defined (WIN32)
	WaitForSingleObject( visthread, INFINITE );
#else
    pthread_join((*visthread),NULL);
#endif
}

void DLL_EXPORT U3D_ADD_DATA_(double* x1, double* x2, double* x3, double* v1, double* v2, double* v3, double* prop)
{
    int i,ti;
	while((*data_changed) != 0){Sleep(500);}

	ti = tab_itr+1;
    
	tab[ti][7][0] = x1[0];
	tab[ti][7][1] = x1[0];
	
	tab[ti][7][2] = x2[0];
	tab[ti][7][3] = x2[0];

	tab[ti][7][4] = x3[0];
	tab[ti][7][5] = x3[0];
	
	tab[ti][7][6] = v1[0];
	tab[ti][7][7] = v1[0];

	tab[ti][7][8] = v2[0];
	tab[ti][7][9] = v2[0];

	tab[ti][7][10] = v3[0];
	tab[ti][7][11] = v3[0];
	
	tab[ti][7][12] = prop[0];
	tab[ti][7][13] = prop[0];


    for(i = 1 ; i < non ; i++)
    {
        tab[ti][0][i] = x1[i];
        if(tab[ti][7][0] > x1[i]) tab[ti][7][0] = x1[i];
        if(tab[ti][7][1] < x1[i]) tab[ti][7][1] = x1[i];

        tab[ti][1][i] = x2[i];
        if(tab[ti][7][2] > x2[i]) tab[ti][7][2] = x2[i];
        if(tab[ti][7][3] < x2[i]) tab[ti][7][3] = x2[i];

        tab[ti][2][i] = x3[i];
        if(tab[ti][7][4] > x3[i]) tab[ti][7][4] = x3[i];
        if(tab[ti][7][5] < x3[i]) tab[ti][7][5] = x3[i];

        tab[ti][3][i] = v1[i];
        if(tab[ti][7][6] > v1[i]) tab[ti][7][6] = v1[i];
        if(tab[ti][7][7] < v1[i]) tab[ti][7][7] = v1[i];

        tab[ti][4][i] = v2[i];
        if(tab[ti][7][8] > v2[i]) tab[ti][7][8] = v2[i];
        if(tab[ti][7][9] < v2[i]) tab[ti][7][9] = v2[i];

        tab[ti][5][i] = v3[i];
        if(tab[ti][7][10] > v3[i]) tab[ti][7][10] = v3[i];
        if(tab[ti][7][11] < v3[i]) tab[ti][7][11] = v3[i];

        tab[ti][6][i] = prop[i];
        if(tab[ti][7][12] > prop[i]) tab[ti][7][12] = prop[i];
        if(tab[ti][7][13] < prop[i]) tab[ti][7][13] = prop[i];
    }
	//after 1st itr set space properties
	if (ti == 0)
	{
		ssx = (float) tab[ti][7][1] - tab[ti][7][0];
		osx = tab[ti][7][0];
		
		ssy = (float) tab[ti][7][3] - tab[ti][7][2];
		osy = tab[ti][7][2];
		
		ssz = (float) tab[ti][7][5] - tab[ti][7][4];
		osz = tab[ti][7][4];
		
	}

    tab_itr = (++tab_itr == itr)?0:tab_itr;
    (*data_changed) = 1;
	
}

void DLL_EXPORT U3D_SET_TITLE_(const char* str, unsigned int size)
{
    strncpy(title, str, (size > 256)? 256:size+1);
}

void DLL_EXPORT U3D_PLAY_LOOP_(double* _delay)
{
	delay = (*_delay);
	dtime = clock();
	play_loop = 1;
}

void DLL_EXPORT U3D_STORE_DATA_(const char* fname, unsigned int size)
{
	FILE* f;
	int i,j;
	
	f = fopen(fname,"wb");
	
	if (f == NULL) return ;
	//header
	fwrite("u3d:",sizeof(char),3,f);
	//dimensions
	fwrite(&itr,sizeof(int),1,f);
	fwrite(&non,sizeof(int),1,f);
	//data
	for(i=0;i<itr;i++)
	{
		for(j=0;j<7;j++)
		{
			fwrite(tab[i][j],sizeof(double),non,f);
		}
		fwrite(tab[i][7],sizeof(double),14,f);
	}
	fclose(f);
}

void set_gl_color(double min, double max, double v)
{
    double hue, sat, val;
    double red, grn, blu;

    double f,p,q,t;
    int i;
    sat = 0.9;
    val = 0.9;

    hue = (v-min)/(max-min)*200;
    if(val==0) {red=0; grn=0; blu=0;}
    else{
        hue/=60;
        i = (int) hue;
        f = hue-i;
        p = val*(1-sat);
        q = val*(1-(sat*f));
        t = val*(1-(sat*(1-f)));
        if (i==0) {red=val; grn=t; blu=p;}
        else if (i==1) {red=q; grn=val; blu=p;}
        else if (i==2) {red=p; grn=val; blu=t;}
        else if (i==3) {red=p; grn=q; blu=val;}
        else if (i==4) {red=t; grn=p; blu=val;}
        else if (i==5) {red=val; grn=p; blu=q;}
        glColor3d(red,grn,blu);
    }

}

void resize_scene(int width, int height)
{
    if (height==0)// prevent divide by zero err
        height=1;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void key_pressed(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'a':
        yrot +=1.0;
        break;
    case 'd':
        yrot -=1.0;
        break;
    case 'w':
        xrot -=1.0;
        break;
    case 's':
        xrot +=1.0;
        break;
    case 'r':
        xrot = 0.0f;
        yrot = 0.0f;
        break;
    case 'q':
        zzoom -=1.0;
        break;
    case 'e':
        zzoom +=1.0;
        break;

    }
}

void mouse_pressed(int button, int state, int x, int y)
{
}

void mouse_motion(int x, int y)
{
}

void init_GL(int width, int height)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);

    glEnable( GL_POINT_SMOOTH );
    glPointSize( 10.0 );

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

    glMatrixMode(GL_MODELVIEW);
}

void draw_scene()
{	
	time_t now;
    int i;
    //end visualization        
	if(done != 0 )
    {
#if defined (WIN32)
		glutDestroyWindow(window);
#else
        glutLeaveMainLoop();
#endif
	}
	// compute fps & set tiltle
    now = time(NULL);
    if(difftime(now,t1) > 1)
    {
        char buf[300];
        sprintf(buf,"%s : (%f fps) ",title,(frames/difftime(now,t1)));
        t1 = now;
        frames = 0;
        glutSetWindowTitle(buf);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f,0.0f,-25.0f-zzoom);
    glRotatef(xrot,1.0f,0.0f,0.0f);
    glRotatef(yrot,0.0f,1.0f,0.0f);
    glTranslatef(-ssx/2.0f,
                 -ssz/2.0f,
                 -ssy/2.0f);
    
	if(play_loop > 0)
	{
		double dt;
		clock_t dnow;
		
		dnow = clock();
		dt = ((double)dnow-dtime)/CLOCKS_PER_SEC;
		
		if (dt >= delay)
		{
			tab_itr = (++tab_itr == itr)?0:tab_itr;
			dtime = dnow;
		}
	}

	if((*data_changed) == 0)
    {
        if (tab_itr >= 0) glCallList(lists[tab_itr]);
    } else {
        lists[tab_itr] = glGenLists(1);
        glNewList(lists[tab_itr],GL_COMPILE);


        glBegin(GL_LINES);
        glColor3f(1.0f,0.0,0.0);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(1.0,0.0,0.0);
        glColor3f(0.0,1.0f,0.0);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(0.0,1.0,0.0);
        glColor3f(0.0,0.0,1.0f);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(0.0,0.0,1.0);
        glEnd();

        glBegin(GL_POINTS);
        {
            for (i = 0 ; i < non ; i++)
            {
                set_gl_color(tab[tab_itr][7][12], tab[tab_itr][7][13], tab[tab_itr][6][i]);
                glVertex3d( tab[tab_itr][0][i], tab[tab_itr][2][i], tab[tab_itr][1][i]);

            }
        }
        glEnd();

        glBegin(GL_LINES);
        {
            glColor3f(0.0f,1.0f,1.0f);
            for (i = 0 ; i < non ; i++)
            {
                glVertex3d( tab[tab_itr][0][i], tab[tab_itr][2][i], tab[tab_itr][1][i]);
                glVertex3d( tab[tab_itr][0][i]+tab[tab_itr][3][i]/scal, tab[tab_itr][2][i]+tab[tab_itr][5][i]/scal, tab[tab_itr][1][i]+tab[tab_itr][4][i]/scal);
            }
        }
        glEnd();

        glEndList();
        glCallList(lists[tab_itr]);

        (*data_changed) = 0;
    }
    glFinish();
    glutSwapBuffers();
    frames++;
}

u3d_th_ret_t window_worker(void* args)
{
	    int argc;
    char* argv[1] = {"ff3d"};
    argc = 1;
    t1 = time(NULL);
    frames = 0;
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutInitWindowSize(640, 480);
    glutInitWindowPosition(0, 0);

    window = glutCreateWindow(title);

    glutDisplayFunc(&draw_scene);
    glutReshapeFunc(&resize_scene);

    glutKeyboardFunc(&key_pressed);
    glutMouseFunc(&mouse_pressed);
    glutMotionFunc(&mouse_motion);

    glutIdleFunc(&draw_scene);
    init_GL(640,480);

	(*initlock) = 0;
    glutMainLoop();
    return NULL;
}
