#include "pgr.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Angel.h"
#include "vec.h"

using namespace Angel;
using namespace std;

constexpr auto groundSize = 200;
constexpr auto groundLevel = -1.5f;
constexpr auto mapSize = 5;

float textureScaleFactor = 10.0f;
float buildingTextureScaleFactor = 1.0f;

float maxHeightOfBuilding = 30.0f;

float cellSize = 20.0f;

float sizeOfBuilding = cellSize * 0.6f;


float map[mapSize][mapSize];

struct Point {
	float lmtexcoord[2];
	float texcoord[2];
	float position[3];
}

const ground[] = {
	{ { 0.25, 1.0 },  { 0.0, 0.0 },															{ -groundSize, groundLevel, -groundSize } },
	{ { 0.25, 0.0 },  { 0.0, groundSize / textureScaleFactor },								{ -groundSize, groundLevel,  groundSize } },
	{ { 0.5, 0.0 },   { groundSize / textureScaleFactor, groundSize / textureScaleFactor }, { groundSize, groundLevel,  groundSize } },
	{ { 0.5, 1.0 },   { groundSize / textureScaleFactor, 0.0 },								{ groundSize, groundLevel, -groundSize } },
};

// Ground indices
const unsigned char groundIndices[] = {
	0,  1,  2,
	0,  2,  3
};

mat4 rotate(GLfloat angle, vec4 axis)
{
	angle = DegreesToRadians * angle;

	GLfloat x = axis.x;
	GLfloat y = axis.y;
	GLfloat z = axis.z;
	GLfloat c = cos(angle);
	GLfloat s = sin(angle);
	GLfloat t = 1.0 - c;

	mat4 m;

	m[0][0] = t * pow(x, 2) + c;
	m[0][1] = t * x * y - s * z;
	m[0][2] = t * x * z + s * y;
	m[0][3] = 0;

	m[1][0] = t * x * y + s * z;
	m[1][1] = t * pow(y, 2) + c;
	m[1][2] = t * y * z - s * x;
	m[1][3] = 0;

	m[2][0] = t * x * z - s * y;
	m[2][1] = t * y * z + s * x;
	m[2][2] = t * pow(z, 2) + c;
	m[2][3] = 0;

	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 1;

	return m;
}


class Camera {

private:
	vec4 pos;
	vec4 dir;
	vec4 up;

public:
	Camera(vec4 pos, vec4 at, vec4 up) {
		this->pos = pos;
		this->dir = normalize(at - pos);
		this->up = up;
	}

	void moveForward(double value) {
		pos += value * dir;
	}

	void moveRight(double value) {
		vec4 right = normalize(cross(dir, up));
		pos += value * right;
	}

	void moveUp(double value) {
		vec4 up = vec4(0, 1, 0, 0);
		pos += value * up;
	}

	void turnUp(double angle) {
		vec4 right = normalize(cross(dir, up));
		dir.w = 1.0;
		dir = rotate(angle, right) * dir;
		dir.w = 0.0;
	}

	void turnRight(double angle) {
		dir.w = 1.0;
		dir = rotate(angle, up) * dir;
		dir.w = 0.0;
	}

	mat4 lookAt() {
		return LookAt(pos, pos + dir, up);
	}
};


Camera camera(vec4(0.0, 2.0, 10, 1.0), vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 0.0));

unsigned char buildingIndices[20 * mapSize * mapSize];

Point buildingArrayOfPoints[8 * mapSize * mapSize];

int nextPointIndex = 0;

float randomFloat(float from, float to) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = to - from;
	float r = random * diff;
	return from + r;
}


void fillMapArray() {
	for (size_t cellRow = 0; cellRow < mapSize; cellRow++) {
		for (size_t cellColumn = 0; cellColumn < mapSize; cellColumn++) {
			map[cellRow][cellColumn] = randomFloat(0, 1);
		}
	}
}

void createIndices() {
	unsigned char buildingIndicesSample[] = {
		0, 1, 2, 3,
		0, 4, 7, 3,
		4, 5, 6, 7,
		1, 2, 6, 5,
		3, 7, 6, 2
	};

	int temp = 20 * mapSize * mapSize;

	for (int i = 0; i < temp; i++)
		buildingIndices[i] = buildingIndicesSample[i % 20] + (i / 20 * 8);
}


void createPointsForCity() {

	nextPointIndex = 0;

	for (size_t cellRow = 0; cellRow < mapSize; cellRow++) {
		for (size_t cellColumn = 0; cellColumn < mapSize; cellColumn++) {
			if (map[cellRow][cellColumn] > 0) {

				buildingArrayOfPoints[nextPointIndex++] = { { maxHeightOfBuilding / 4, maxHeightOfBuilding },	{ buildingTextureScaleFactor , 0 },	{ cellSize * cellRow, groundLevel, cellSize * cellColumn } };
				buildingArrayOfPoints[nextPointIndex++] = { { maxHeightOfBuilding / 4, 0.0 },					{ 0, 0 },	{ cellSize * cellRow + sizeOfBuilding, groundLevel, cellSize * cellColumn } };
				buildingArrayOfPoints[nextPointIndex++] = { { maxHeightOfBuilding / 2, 0.0 },					{ 0, buildingTextureScaleFactor  },	{ cellSize * cellRow + sizeOfBuilding, maxHeightOfBuilding * map[cellRow][cellColumn], cellSize * cellColumn } };
				buildingArrayOfPoints[nextPointIndex++] = { { maxHeightOfBuilding / 2, maxHeightOfBuilding },	{ buildingTextureScaleFactor , buildingTextureScaleFactor  },	{ cellSize * cellRow, maxHeightOfBuilding * map[cellRow][cellColumn], cellSize * cellColumn } };

				buildingArrayOfPoints[nextPointIndex++] = { { 0 / 4, maxHeightOfBuilding },					{ 0, 0 },		{ cellSize * cellRow, groundLevel, cellSize * cellColumn + sizeOfBuilding} };
				buildingArrayOfPoints[nextPointIndex++] = { { 0.25, 0.0 },									{ buildingTextureScaleFactor , 0},		{ cellSize * cellRow + sizeOfBuilding, groundLevel,  cellSize * cellColumn + sizeOfBuilding } };
				buildingArrayOfPoints[nextPointIndex++] = { { 0.5, 0.0 },									{ buildingTextureScaleFactor , buildingTextureScaleFactor},		{ cellSize * cellRow + sizeOfBuilding, maxHeightOfBuilding * map[cellRow][cellColumn], cellSize * cellColumn + sizeOfBuilding } };
				buildingArrayOfPoints[nextPointIndex++] = { { 0.5, maxHeightOfBuilding },					{ 0, buildingTextureScaleFactor  },		{ cellSize * cellRow, maxHeightOfBuilding * map[cellRow][cellColumn], cellSize * cellColumn + sizeOfBuilding } };
			}
		}
	}
}

glm::vec3 lightPos = glm::vec3(0, 5.0f, 0);

//Light color properties
glm::vec3 lightAmbientColor = glm::vec3(0.9);
glm::vec3 lightDiffuseColor = glm::vec3(0, 1, 0);
glm::vec3 lightSpecularColor = glm::vec3(0.3, 0.7, 0.3);

GLuint BuildingsVBO, BuildingsEBO;

GLuint GroundVBO, GroundEBO;

GLuint texture;

GLuint buildingTexture;

GLuint samplerTiles, samplerLightmap;

int width, height;
float speed = 0.004f;

bool pressed[256];

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//Gound
GLuint GVS, GFS, GProg;

GLuint GroundPositionAttrib, GroundtcAttrib, GroundmvpUniform;
GLuint GroundTextureUniform;

GLuint lightmapTexture;
GLint  LMUniform;
GLuint LMCoordAttrib;

//Building
GLuint BuildingVS, BuildingFS, BuildingProg;
GLuint BuildingPositionAttrib, BuildingtcAttrib, BuildingmvpUniform;
GLuint BuildingTextureUniform;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Event handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void onInit()
{
	fillMapArray();
	createIndices();
	createPointsForCity();

	// Shaders
	GVS = compileShader(GL_VERTEX_SHADER, loadFile("data\\ground.vs").c_str());
	GFS = compileShader(GL_FRAGMENT_SHADER, loadFile("data\\ground.fs").c_str());
	GProg = linkShader(2, GVS, GFS);

	GroundPositionAttrib = glGetAttribLocation(GProg, "position");
	GroundtcAttrib = glGetAttribLocation(GProg, "tc");

	LMCoordAttrib = glGetAttribLocation(GProg, "lm_tc");

	GroundmvpUniform = glGetUniformLocation(GProg, "mvp");

	GroundTextureUniform = glGetUniformLocation(GProg, "tex");

	LMUniform = glGetUniformLocation(GProg, "lightMap");

	BuildingVS = compileShader(GL_VERTEX_SHADER, loadFile("data\\building.vs").c_str());
	BuildingFS = compileShader(GL_FRAGMENT_SHADER, loadFile("data\\building.fs").c_str());
	BuildingProg = linkShader(2, BuildingVS, BuildingFS);

	BuildingPositionAttrib = glGetAttribLocation(BuildingProg, "position");

	BuildingtcAttrib = glGetAttribLocation(GProg, "tc");

	LMCoordAttrib = glGetAttribLocation(GProg, "lm_tc");

	BuildingmvpUniform = glGetUniformLocation(GProg, "mvp");

	BuildingTextureUniform = glGetUniformLocation(GProg, "tex");

	// Copy ground to graphics card
	glGenBuffers(1, &GroundVBO);
	glBindBuffer(GL_ARRAY_BUFFER, GroundVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);

	glGenBuffers(1, &GroundEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GroundEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);


	// Buiding points
	glGenBuffers(1, &BuildingsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, BuildingsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buildingArrayOfPoints), buildingArrayOfPoints, GL_STATIC_DRAW);

	// Building indices
	glGenBuffers(1, &BuildingsEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BuildingsEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(buildingIndices), buildingIndices, GL_STATIC_DRAW);


	//Load texture from file
	SDL_PixelFormat pixFormat;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	memset(&pixFormat, 0, sizeof(SDL_PixelFormat));
	pixFormat.BitsPerPixel = 24;
	pixFormat.BytesPerPixel = 3;
	pixFormat.Rmask = 0xff;
	pixFormat.Gmask = 0xff00;
	pixFormat.Bmask = 0xff0000;

	SDL_Surface* surface = SDL_ConvertSurface(SDL_LoadBMP("data/asphalt.bmp"), &pixFormat, 0);
	if (surface == NULL) throw SDL_Exception();

	SDL_Surface* buildingSurface = SDL_ConvertSurface(SDL_LoadBMP("data/building.bmp"), &pixFormat, 0);
	if (surface == NULL) throw SDL_Exception();

	//Creating diffuse texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &buildingTexture);
	glBindTexture(GL_TEXTURE_2D, buildingTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, buildingSurface->w, buildingSurface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, buildingSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenSamplers(1, &samplerTiles);
	glSamplerParameteri(samplerTiles, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(samplerTiles, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(samplerTiles, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(samplerTiles, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//Load lightmap from file
	SDL_Surface* surface1 = SDL_ConvertSurface(SDL_LoadBMP("data/lightmap.bmp"), &pixFormat, 0);
	if (surface1 == NULL) throw SDL_Exception();

	glGenTextures(0, &lightmapTexture);
	glBindTexture(GL_TEXTURE_2D, lightmapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface1->w, surface1->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface1->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenSamplers(1, &samplerLightmap);
	glSamplerParameteri(samplerLightmap, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(samplerLightmap, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(samplerLightmap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(samplerLightmap, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void onWindowRedraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(.8, .8, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	mat4 persp = Perspective(90.0f, 16 / 9, 0.1f, 1000.0f);

	mat4 mpv = persp * camera.lookAt();

	//Draw ground
	glUseProgram(GProg);

	glUniformMatrix4fv(GroundmvpUniform, 1, GL_TRUE, mpv);

	//Diffuse texture - bound to texturing unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(0, samplerTiles);
	glUniform1i(GroundTextureUniform, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightmapTexture);
	glBindSampler(1, samplerLightmap);
	glUniform1i(LMUniform, 1);


	glEnableVertexAttribArray(GroundPositionAttrib);
	glEnableVertexAttribArray(GroundtcAttrib);
	if (LMUniform >= 0)
		glEnableVertexAttribArray(LMCoordAttrib);

	//Specifying data input, assigning buffers to attributes
	glBindBuffer(GL_ARRAY_BUFFER, GroundVBO);
	glVertexAttribPointer(GroundPositionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, position));
	glVertexAttribPointer(GroundtcAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, texcoord));


	if (LMUniform >= 0)
		glVertexAttribPointer(LMCoordAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, lmtexcoord));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GroundEBO);
	glDrawElements(GL_TRIANGLES, sizeof(groundIndices) / sizeof(*groundIndices), GL_UNSIGNED_BYTE, NULL);

	glDisableVertexAttribArray(GroundPositionAttrib);
	glDisableVertexAttribArray(GroundtcAttrib);

	glDisableVertexAttribArray(LMCoordAttrib);

	// Draw building
	glUseProgram(BuildingProg);

	glUniformMatrix4fv(BuildingmvpUniform, 1, GL_TRUE, mpv);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, buildingTexture);
	glBindSampler(0, samplerTiles);
	glUniform1i(BuildingTextureUniform, 0);

	glEnableVertexAttribArray(BuildingPositionAttrib);
	glEnableVertexAttribArray(BuildingtcAttrib);
	if (LMUniform >= 0)
		glEnableVertexAttribArray(LMCoordAttrib);

	//Specifying data input, assigning buffers to attributes
	glBindBuffer(GL_ARRAY_BUFFER, BuildingsVBO);
	glVertexAttribPointer(BuildingPositionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, position));
	glVertexAttribPointer(BuildingtcAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, texcoord));


	if (LMUniform >= 0)
		glVertexAttribPointer(LMCoordAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, lmtexcoord));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BuildingsEBO);
	glDrawElements(GL_QUADS, sizeof(buildingIndices) / sizeof(*buildingIndices), GL_UNSIGNED_BYTE, NULL);

	glDisableVertexAttribArray(BuildingPositionAttrib);
	glDisableVertexAttribArray(BuildingtcAttrib);

	glDisableVertexAttribArray(LMCoordAttrib);

	SDL_GL_SwapBuffers();

	if (pressed['w'] || pressed['W']) camera.moveForward(speed);
	if (pressed['s'] || pressed['S']) camera.moveForward(-speed);
	if (pressed['a'] || pressed['A']) camera.moveRight(-speed);
	if (pressed['d'] || pressed['D']) camera.moveRight(+speed);
	if (pressed[' ']) camera.moveUp(speed);
}

void onWindowResized(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w; height = h;
}

void onKeyDown(SDLKey key, Uint16 /*mod*/)
{
	pressed[key] = true;
	switch (key) {
	case SDLK_ESCAPE: quit(); return;
	default: return;
	}
}

void onKeyUp(SDLKey key, Uint16 /*mod*/)
{
	pressed[key] = false;
}

void onMouseMove(unsigned x, unsigned y, int xrel, int yrel, Uint8 buttons)
{
	camera.turnRight(-xrel / 3);
	camera.turnUp(-yrel / 3);
	redraw();
}

void onMouseDown(Uint8 /*button*/, unsigned /*x*/, unsigned /*y*/)
{
}

void onMouseUp(Uint8 /*button*/, unsigned /*x*/, unsigned /*y*/)
{
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int /*argc*/, char** /*argv*/)
{
	try {
		// Init SDL - only video subsystem will be used
		if (SDL_Init(SDL_INIT_VIDEO) < 0) throw SDL_Exception();

		// Shutdown SDL when program ends
		atexit(SDL_Quit);

		init(800, 600, 24, 16, 0);
		mainLoop();

	}
	catch (exception & ex) {
		cout << "ERROR : " << ex.what() << endl;
		cout << "Press any key to exit...\n";
		cin.get();

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
