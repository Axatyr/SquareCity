#include "Lib.h"
#include "ShaderMaker.h"
//#include "Geometria.h"
//#include "GestioneTesto.h"
//#include "GestioneEventi.h"
#include "objloader.hpp"
//#include "VAO.h"
//#include "Materiale.h"
//#include "Loader.h"

// Dimensioni finestre
int width = 1280;
int height = 720;

// Per il resize
int w_up = width;
int h_up = height;

// Variabili per vertex shader
static unsigned int programId, programId_text, programId1, programIdr;
static unsigned int MatrixProjS, MatrixProj, MatrixProj_txt;
static unsigned int MatModel, MatView, MatViewS;
// Da guardare queste variabili a cosa servono
static unsigned int loc_time, lsh, lscelta, loc_view_pos, lblinn;
unsigned int VAO_Text, VBO_Text, cubemapTexture, idTex;
int selected_obj = -1;

// Da controllare a cosa serve
vec3 asse = vec3(0.0, 1.0, 0.0);

mat4 Projection, Model, View, Projection_text;

static vector<Material> materials;
static vector<Shader> shaders;

// Luce
point_light ext_light;
float angolo = 0.0; // Per luce

//Puntatori alle variabili uniformi per l'impostazione dell'illuminazione
LightShaderUniform light_unif = {};

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void INIT_SHADER(void)
{
}

void INIT_VAO_Text(void)
{
}

void INIT_VAO() {

}

void INIT_Illuminazione() {
	// Da modificare
	ext_light.position = { 0.0,10.0,0.0 };
	ext_light.color = { 1.0,1.0,1.0 };
	ext_light.power = 2.f;

	// Setup dei materiali
	materials.resize(8);
	materials[MaterialType::RED_PLASTIC].name = "Red Plastic";
	materials[MaterialType::RED_PLASTIC].ambient = red_plastic_ambient;
	materials[MaterialType::RED_PLASTIC].diffuse = red_plastic_diffuse;
	materials[MaterialType::RED_PLASTIC].specular = red_plastic_specular;
	materials[MaterialType::RED_PLASTIC].shininess = red_plastic_shininess;

	materials[MaterialType::EMERALD].name = "Emerald";
	materials[MaterialType::EMERALD].ambient = emerald_ambient;
	materials[MaterialType::EMERALD].diffuse = emerald_diffuse;
	materials[MaterialType::EMERALD].specular = emerald_specular;
	materials[MaterialType::EMERALD].shininess = emerald_shininess;

	materials[MaterialType::BRASS].name = "Brass";
	materials[MaterialType::BRASS].ambient = brass_ambient;
	materials[MaterialType::BRASS].diffuse = brass_diffuse;
	materials[MaterialType::BRASS].specular = brass_specular;
	materials[MaterialType::BRASS].shininess = brass_shininess;

	materials[MaterialType::SLATE].name = "Slate";
	materials[MaterialType::SLATE].ambient = slate_ambient;
	materials[MaterialType::SLATE].diffuse = slate_diffuse;
	materials[MaterialType::SLATE].specular = slate_specular;
	materials[MaterialType::SLATE].shininess = slate_shininess;

	materials[MaterialType::YELLOW].name = "Yellow";
	materials[MaterialType::YELLOW].ambient = yellow_ambient;
	materials[MaterialType::YELLOW].diffuse = yellow_diffuse;
	materials[MaterialType::YELLOW].specular = yellow_specular;
	materials[MaterialType::YELLOW].shininess = yellow_shininess;

	materials[MaterialType::ROSA].name = "ROSA";
	materials[MaterialType::ROSA].ambient = rosa_ambient;
	materials[MaterialType::ROSA].diffuse = rosa_diffuse;
	materials[MaterialType::ROSA].specular = rosa_specular;
	materials[MaterialType::ROSA].shininess = rosa_shininess;

	materials[MaterialType::MARRONE].name = "MARRONE";
	materials[MaterialType::MARRONE].ambient = marrone_ambient;
	materials[MaterialType::MARRONE].diffuse = marrone_diffuse;
	materials[MaterialType::MARRONE].specular = marrone_specular;
	materials[MaterialType::MARRONE].shininess = marrone_shininess;
	materials[MaterialType::NO_MATERIAL].name = "NO_MATERIAL";
	materials[MaterialType::NO_MATERIAL].ambient = vec3(1, 1, 1);
	materials[MaterialType::NO_MATERIAL].diffuse = vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].specular = vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].shininess = 1.f;

	// Setup degli shader
	shaders.resize(5);
	shaders[ShaderOption::NONE].value = 0;
	shaders[ShaderOption::NONE].name = "NONE";
	shaders[ShaderOption::GOURAD_SHADING].value = 1;
	shaders[ShaderOption::GOURAD_SHADING].name = "GOURAD SHADING";
	shaders[ShaderOption::PHONG_SHADING].value = 2;
	shaders[ShaderOption::PHONG_SHADING].name = "PHONG SHADING";
	shaders[ShaderOption::ONDE_SHADING].value = 3;
	shaders[ShaderOption::ONDE_SHADING].name = "ONDE SHADING";
	shaders[ShaderOption::BANDIERA_SHADING].value = 4;
	shaders[ShaderOption::BANDIERA_SHADING].name = "BANDIERA SHADING";
}

void INIT_CAMERA_PROJECTION()
{
	// Da modificare posizioni
	// Imposto la telecamera
	ViewSetup = {};
	ViewSetup.position = vec4(0.0, 0.5, 25.0, 0.0);
	ViewSetup.target = vec4(0.0, 0.0, 0.0, 0.0);
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.upVector = vec4(0.0, 1.0, 0.0, 0.0);

	// Imposto la proiezione prospettica
	PerspectiveSetup = {};
	PerspectiveSetup.aspect = (GLfloat)width / (GLfloat)height;
	PerspectiveSetup.fovY = 45.0f;
	PerspectiveSetup.far_plane = 2000.0f;
	PerspectiveSetup.near_plane = 0.1f;
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	PerspectiveSetup.aspect = (GLfloat)w / (GLfloat)h;
	Projection = perspective(radians(PerspectiveSetup.fovY), PerspectiveSetup.aspect, PerspectiveSetup.near_plane, PerspectiveSetup.far_plane);
	w_up = w;
	h_up = h;
}

// Windows info
void resizeSecWind(int w, int h)
{
	glViewport(0, 0, w, h);
	Projection_text = ortho(0.0f, (float)w, 0.0f, (float)h);

	w_up = w;
	h_up = h;
}

// Da sistemare
void drawScene(void)
{

}

// Windows info
void drawSceneSecWind(void)
{

}

int main(int argc, char* argv[])
{
	// Initial setting
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	// Display setting scene 3d
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(100, 100);
	// se da errori attribuire ad una variabile int
	glutCreateWindow("SquareCity");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	cout << "Benvenuto su SquareCity! ";

	// Inserimento periferiche esterne usate
	// Gestione tastiera 
	glutKeyboardFunc(keyboardPressedEvent);
	// Gestione mouse
	glutMouseFunc(mouse);
	glutMotionFunc(mouseActiveMotion); // Evento tasto premuto

	glutTimerFunc(66, update, 0);

	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	INIT_Illuminazione();
	// Menu
	//buildOpenGLMenu();
	INIT_CAMERA_PROJECTION();
	// non so cosa fa
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Display setting info
	// eventualmente da modificare i campi
	glutInitWindowSize(WIDTH_SEC, HEIGHT_SEC);
	glutInitWindowPosition(500, 350);
	// se da errori attribuire ad una variabile int
	glutCreateWindow("Informazioni");
	glutDisplayFunc(drawSceneSecWind);
	glutReshapeFunc(resizeSecWind);

	INIT_SHADER();
	INIT_VAO_Text();
	Init_Freetype();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Per normale
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Projection (in vertex shader).
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Projection al Vertex Shader
	MatrixProj = glGetUniformLocation(programId, "Projection");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatModel = glGetUniformLocation(programId, "Model");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 View (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice View al Vertex Shader
	MatView = glGetUniformLocation(programId, "View");
	loc_time = glGetUniformLocation(programId, "time");
	loc_view_pos = glGetUniformLocation(programId, "ViewPos");

	lscelta = glGetUniformLocation(programId, "sceltaVs");

	// Da sistemare questione luci
	light_unif.light_position_pointer = glGetUniformLocation(programId, "light.position");
	light_unif.light_color_pointer = glGetUniformLocation(programId, "light.color");
	light_unif.light_power_pointer = glGetUniformLocation(programId, "light.power");
	light_unif.material_ambient = glGetUniformLocation(programId, "material.ambient");
	light_unif.material_diffuse = glGetUniformLocation(programId, "material.diffuse");
	light_unif.material_specular = glGetUniformLocation(programId, "material.specular");
	light_unif.material_shininess = glGetUniformLocation(programId, "material.shininess");
	idTex = glGetUniformLocation(programId, "tex0");

	// Per cubemap
	MatrixProjS = glGetUniformLocation(programId1, "Projection");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader

	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatViewS = glGetUniformLocation(programId1, "View");

	// Per riflessiva
	/*
	MatModelR = glGetUniformLocation(programIdr, "Model");
	MatViewR = glGetUniformLocation(programIdr, "View");
	MatrixProjR = glGetUniformLocation(programIdr, "Projection");
	loc_view_posR = glGetUniformLocation(programIdr, "ViewPos");
	loc_cubemapR = glGetUniformLocation(programIdr, "cubemap");
	*/

	glutMainLoop();
}