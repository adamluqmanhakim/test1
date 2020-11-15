// Name : Adam Luqman Hakim Bin Mohamad
// Student ID : 40021709
// Class : Comp 426
// Define states for cells of tbb

#include <windows.h>
#include <time.h>
#include <thread>
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include <tbb/tbb.h>
#include <string>


#define healthy  0
#define cancer   1
#define medicine 2

// 2d grid
const int gridOfWindowWidth = 1024;
const int gridOfWindowHeight = 768;
int grid[gridOfWindowWidth][gridOfWindowHeight];
int stateBefore[1][1];

// Update every 1/30th second
const int g_updateTime = 1.0 / 30.0 * 1000.0;

// At least 25% of cells initialized as cancer cells
const int NumInitialCancer = gridOfWindowWidth * gridOfWindowHeight * 0.25;

const int g_font = (int)GLUT_BITMAP_TIMES_ROMAN_24;

const size_t init_size = 0;


//Desc : Initialization function for glut

void Initialize()
{


	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, gridOfWindowWidth, gridOfWindowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat aspect = (GLfloat)gridOfWindowWidth / gridOfWindowHeight;
	gluPerspective(45, aspect, 0.1f, 10.0f);
	glClearColor(0.0, 0.0, 0.0, 0.0);
}




//Desc: Renders bitmap strings to display text on screen
//Param1 : x position of where text should be displayed
//Param2 : y position of where text should be displayed
//Param 3: Font to be used
//Param 4 :String text to be displayed on screen
void RenderBitmapString(float x, float y, void* font, const char* string)
{
	const char* c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}



//Desc : Displays the cells and text in a window on screen
void Display()
{
	// Display the cells using OpenGL
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, gridOfWindowWidth, gridOfWindowHeight, 0);

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_QUADS);
	int healthyCount = 0;
	int cancerCount = 0;
	int medicineCount = 0;
	for (int x = 0; x < gridOfWindowWidth; x++)
	{
		for (int y = 0; y < gridOfWindowHeight; y++)
		{
			if (grid[x][y] == healthy)
			{
				// Healthy cells are green
				glColor3f(0, 1, 0);
				healthyCount++;
			}
			else if (grid[x][y] == cancer)
			{
				// Cancer cells are red
				glColor3f(1, 0, 0);
				cancerCount++;
			}
			else if (grid[x][y] == medicine)
			{
				// Medicine cells are yellow
				glColor3f(1, 1, 0);
				medicineCount++;
			}
			//it display the cells 

			glVertex2f(x, y);
			glVertex2f(x + 1, y);
			glVertex2f(x + 1, y + 1);
			glVertex2f(x, y + 1);
		}
	}
	glEnd();

	std::string _hCount = std::to_string(healthyCount);
	const char* _hc = _hCount.c_str();
	std::string _cCount = std::to_string(cancerCount);
	const char* _cc = _cCount.c_str();
	std::string _mCount = std::to_string(medicineCount);
	const char* _mc = _mCount.c_str();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(0, 0, 0);
	// Display the number of each type of cell
	RenderBitmapString(0, 30, (void*)g_font, "Healthy: ");
	RenderBitmapString(0, 50, (void*)g_font, _hc);
	RenderBitmapString(0, 100, (void*)g_font, "Cancer: ");
	RenderBitmapString(0, 120, (void*)g_font, _cc);
	RenderBitmapString(0, 170, (void*)g_font, "Medicine: ");
	RenderBitmapString(0, 190, (void*)g_font, _mc);
	glPopMatrix();

	glutSwapBuffers();
}




//Desc : Heals all surrounding medicine cells when a cancer cell turns into a healthy cell
//param1 : x position of current cell
//param2 : y position of current cell

void MedicineCellSurroundingHeal(int x, int y)
{

	grid[x][y] = healthy;
	if (x > 0 && y > 0) {
		if (grid[x - 1][y - 1] == medicine)
			MedicineCellSurroundingHeal(x - 1, y - 1);
	}
	if (y > 0) {
		if (grid[x][y - 1] == medicine)
			MedicineCellSurroundingHeal(x, y - 1);
	}
	if (x < (gridOfWindowWidth - 1) && y > 0) {
		if (grid[x + 1][y - 1] == medicine)
			MedicineCellSurroundingHeal(x + 1, y - 1);
	}
	if (x > 0) {
		if (grid[x - 1][y] == medicine)
			MedicineCellSurroundingHeal(x - 1, y);
	}
	if (x < (gridOfWindowWidth - 1)) {
		if (grid[x + 1][y] == medicine)
			MedicineCellSurroundingHeal(x + 1, y);
	}
	if (x > 0 && y < (gridOfWindowHeight - 1)) {
		if (grid[x - 1][y + 1] == medicine)
			MedicineCellSurroundingHeal(x - 1, y + 1);
	}
	if (y < (gridOfWindowHeight - 1)) {
		if (grid[x][y + 1] == medicine)
			MedicineCellSurroundingHeal(x, y + 1);
	}
	if (x < (gridOfWindowWidth - 1) && y < (gridOfWindowHeight - 1)) {
		if (grid[x + 1][y + 1] == medicine)
			MedicineCellSurroundingHeal(x + 1, y + 1);
	}
}

//Desc : The thread calls each cell state fot update
//param1 : x position of current cell
//param2 : y position of current cell
//param3 : state of current cell

void UpdateState(int x, int y, int state, int stateBefores)
{


	if (state == healthy || state == cancer) {
		int numOfCellSurrounded = 0;
		int numBefore = 0;
		int numAfter = 0;

		// If a healthy cell is surrounded by >= 6 cancer cells,
		// it becomes a cancer cell
		if (state == healthy) {
			numBefore = cancer;
			numAfter = cancer;
		}
		// If a cancer cell is surrounded by >= 6 medicine cells,
		// it becomes a healthy cell
		else if (state == cancer) {
			numBefore = medicine;
			numAfter = healthy;
		}


		// Check the states of the surrounding cells
		if (x > 0 && y > 0) {
			if (grid[x - 1][y - 1] == numBefore)
				numOfCellSurrounded++;
		}
		if (y > 0) {
			if (grid[x][y - 1] == numBefore)
				numOfCellSurrounded++;
		}
		if (x < (gridOfWindowWidth - 1) && y > 0) {
			if (grid[x + 1][y - 1] == numBefore)
				numOfCellSurrounded++;
		}
		if (x > 0) {
			if (grid[x - 1][y] == numBefore)
				numOfCellSurrounded++;
		}
		if (x < (gridOfWindowWidth - 1)) {
			if (grid[x + 1][y] == numBefore)
				numOfCellSurrounded++;
		}
		if (x > 0 && y < (gridOfWindowHeight - 1)) {
			if (grid[x - 1][y + 1] == numBefore)
				numOfCellSurrounded++;
		}
		if (y < (gridOfWindowHeight - 1)) {
			if (grid[x][y + 1] == numBefore)
				numOfCellSurrounded++;
		}
		if (x < (gridOfWindowWidth - 1) && y < (gridOfWindowHeight - 1)) {
			if (grid[x + 1][y + 1] == numBefore)
				numOfCellSurrounded++;
		}
		// Change state if surrounded by >= 6 of a certain cell
		if (numOfCellSurrounded >= 6) {
			// When a cancer cell becomes a healthy cell,
			// all the surrounding medicine cells also become healthy cells
			if (state == cancer)
				MedicineCellSurroundingHeal(x, y);
			else
				grid[x][y] = numAfter;
		}

	}





}




class DoUpdate
{
	
	//Desc : Class with overloaded parenthesis () operator
	
	int *startX, *endX, *startY, *endY;
public:
	DoUpdate(int *sX, int *eX, int *sY, int *eY)
		: startX(sX), endX(eX), startY(sY), endY(eY) { }

	// overload () so it starts updating the cell states
	void operator()(const tbb::blocked_range2d<size_t>& r) const
	{
		
		//Desc : Overloaded parenthesis () operator
		//param1 : TBB 2D blocked range
		

		// Update each cell that the current thread manages
		for (size_t i = r.cols().begin(); i != r.cols().end(); ++i)
		{
			for (size_t j = r.rows().begin(); j != r.rows().end(); ++j)
				UpdateState(i, j, grid[i][j],8);
		}
	}
};

void Update(int value)
{
	
	//@Desc : Function that uses TBB to initialize a task scheduler and perform a parallel loop, and then calls itself (to update again)
	//param1 : unused parameter that is passed by the glutTimerFunc
	

	// allocate storage for vectors
	int *startX, *endX, *startY, *endY;
	startX = (int*)malloc(init_size*sizeof(int));
	endX = (int*)malloc(init_size*sizeof(int));
	startY = (int*)malloc(init_size*sizeof(int));
	endY = (int*)malloc(init_size*sizeof(int));

	tbb::task_scheduler_init init;

	*startX = 0, *endX = gridOfWindowWidth, *startY = 0, *endY = gridOfWindowHeight;
	tbb::parallel_for(tbb::blocked_range2d<size_t>(*startX, *endX, 1, *startY, *endY, 1), DoUpdate(startX, endX, startY, endY), tbb::auto_partitioner());

	glutPostRedisplay();
	glutTimerFunc(g_updateTime, Update, 0);
}


//Desc : Function that handles mouse clicks
//param1 : mouse button that was clicked
//param2 : state of button that was clicked
//param3 : x position of pointer when mouse was clicked
//param4 : y position of pointer when mouse was clicked
void MouseClicks(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// If medicine is injected on a cancer cell,
		// the medicine is absorbed and the cell turns into a healthy cell
		if (grid[x][y] == cancer) {
			grid[x][y] = healthy;
		}
		// If medicine is injected on a healthy or medicine cell,
		// the medicine is not absorbed and propagates radially outwards by one cell
		/*
				//up
				grid[x][y+i] = medicine;
				//down
				grid[x][y-i] = medicine;
				//left
				grid[x-i][y] = medicine;
				//right
				grid[x+i][y] = medicine;
				//top left
				grid[x-i][y+i] = medicine;
				//top right
				grid[x+i][y+i] = medicine;
				//bottom left
				grid[x-i][y-i] = medicine;
				//bottom right
				grid[x+i][y-i] = medicine;

				*/
		else {
			//stateBefore[x][y] = grid[x][y];

			grid[x][y] = medicine;




			/*	if (x > 0 && y > 0)
					grid[x - 1][y - 1] = medicine;
				if (y > 0)
					grid[x][y - 1] = medicine;
				if (x < (gridOfWindowWidth - 1) && y > 0)
					grid[x + 1][y - 1] = medicine;
				if (x > 0)
					grid[x - 1][y] = medicine;
				if (x < (gridOfWindowWidth - 1))
					grid[x + 1][y] = medicine;
				if (x > 0 && y < (gridOfWindowHeight - 1))
					grid[x - 1][y + 1] = medicine;
				if (y < (gridOfWindowHeight - 1))
					grid[x][y + 1] = medicine;
				if (x < (gridOfWindowWidth - 1) && y < (gridOfWindowHeight - 1))
					grid[x + 1][y + 1] = medicine;*/
			int max;
			if ((gridOfWindowWidth - x) > (gridOfWindowHeight - y)) {
				max = gridOfWindowHeight - y;
			}

			else if ((gridOfWindowWidth - x) < (gridOfWindowHeight - y)) {
				max = gridOfWindowWidth - x;
			}

			else {
				max = gridOfWindowHeight - y;
			}


			for (int i = 0; i < max; i++) {
				if (x > 0 && y > 0)
					grid[x - i][y - i] = medicine;
				if (y > 0)
					grid[x][y - i] = medicine;
				if (x < (gridOfWindowWidth - 1) && y > 0)
					grid[x + i][y - i] = medicine;
				if (x > 0)
					grid[x - i][y] = medicine;
				if (x < (gridOfWindowWidth - 1))
					grid[x + i][y] = medicine;
				if (x > 0 && y < (gridOfWindowHeight - 1))
					grid[x - i][y + i] = medicine;
				if (y < (gridOfWindowHeight - 1))
					grid[x][y + i] = medicine;
				if (x < (gridOfWindowWidth - 1) && y < (gridOfWindowHeight - 1))
					grid[x + i][y + i] = medicine;

			}
		}
	}
}




int main(int argc, char** argv)
{

	// initialize
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(gridOfWindowWidth, gridOfWindowHeight);
	glutCreateWindow("2D Cell Growth Simulation");

	// Initialize all cells as healthy cells
	for (int i = 0; i < gridOfWindowWidth; i++)
	{
		for (int j = 0; j < gridOfWindowHeight; j++)
		{
			grid[i][j] = healthy;
		}
	}

	// Initialize random seed
	srand(time(NULL));

	// Cancer cells need to change to atleast 25%
	for (int i = 0; i <= NumInitialCancer; i++)
	{
		int x = rand() % gridOfWindowWidth;
		int y = rand() % gridOfWindowHeight;
		if (grid[x][y] == cancer)
			i--;
		else
			grid[x][y] = cancer;
	}

	glutDisplayFunc(Display);
	glutIdleFunc(Display);
	glutMouseFunc(MouseClicks);

	glutTimerFunc(g_updateTime, Update, 0);
	Initialize();

	glutMainLoop();
	return 0;
}
