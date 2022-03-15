#include "Lib.h"
#include "ShaderMaker.h"
#include "Geometria.h"
//#include "GestioneTesto.h"
#include "GestioneEventi.h"

//#include "objloader.hpp"
//#include "VAO.h"
//#include "Materiale.h"
//#include "Loader.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// Dimensioni finestre
int width = 1280;
int height = 720;

// Per il resize
int widthRes = width;
int heightRes = height;

// Variabili per vertex shader
static unsigned int programId, programId_text, programId_Sfondo;
static unsigned int  MatrixProj, MatrixProj_txt, MatrixProjS;
static unsigned int MatModel, MatModelS, MatView, MatViewS;
// Da guardare queste variabili a cosa servono
static unsigned int loc_time, loc_res, loc_mouse;
// Da capire pure queste
static unsigned int lsh, lscelta, loc_view_pos, lblinn;

unsigned int VAO_Text, VBO_Text;
int selected_obj = -1;

// Gestione camera
string Operazione;
string stringa_asse;
int idfg, idfi;
float cameraSpeed = 0.05;
vec3 asse = vec3(0.0, 1.0, 0.0);



// Gestione update
float angoloUp = 0;


// Da controllare a cosa serve
static float quan = 0;
Mesh Cubo, Piano, Piramide, Centri, Sfera;
float posxN, posyN;
vec2 resolution, mousepos;

mat4 Projection, Projection_text, Model, View;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

static vector<Material> materials;
static vector<Shader> shaders;

// Luce
point_light ext_light;
float angolo = 0.0; // Per luce

//Puntatori alle variabili uniformi per l'impostazione dell'illuminazione
LightShaderUniform light_unif = {};

void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_C.glsl";
	char* fragmentShader = (char*)"fragmentShader_C.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	//Generazione del program shader per la gestione del testo
	/*
	vertexShader = (char*)"VertexShader_Text.glsl";
	fragmentShader = (char*)"FragmentShader_Text.glsl";

	programId_text = ShaderMaker::createProgram(vertexShader, fragmentShader);
	*/

	vertexShader = (char*)"vertexShader_C.glsl";
	fragmentShader = (char*)"fragmentShader_Sfondo.glsl";

	programId_Sfondo = ShaderMaker::createProgram(vertexShader, fragmentShader);

}

void crea_VAO_Vector(Mesh* mesh)
{
	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);
	//Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &mesh->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertici.size() * sizeof(vec3), mesh->vertici.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &mesh->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, mesh->colori.size() * sizeof(vec4), mesh->colori.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	//EBO di tipo indici
	glGenBuffers(1, &mesh->EBO_indici);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO_indici);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indici.size() * sizeof(GLuint), mesh->indici.data(), GL_STATIC_DRAW);
}

void INIT_VAO_Text(void)
{
	// configure VAO/VBO for texture quads
	/* ---------------------------------- -
	glGenVertexArrays(1, &VAO_Text);
	glGenBuffers(1, &VBO_Text);
	glBindVertexArray(VAO_Text);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Text);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	*/
}

void INIT_VAO() {

	Mesh Sfondo;

	//COSTRUZIONE AMBIENTE: STRUTTURA Scena

	//SFONDO
	crea_piano(&Sfondo, vec4(0.2, 0.2, 0.9, 1.0));
	crea_VAO_Vector(&Sfondo);
	Sfondo.nome = "Sfondo";
	Sfondo.Model = mat4(1.0);
	Sfondo.Model = translate(Sfondo.Model, vec3(0.0, 0.0, -100.0));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Sfondo.Model = scale(Sfondo.Model, vec3(1000.0f, 1000.00f, 1.0f));
	Sfondo.Model = rotate(Sfondo.Model, radians(90.0f), vec3(1.0, 0.0, 0.0));
	raggi.push_back(1.5);
	Scena.push_back(Sfondo);

	//TERRENO
	crea_piano(&Piano, vec4(0.9, 0.9, 0.9, 1.0));
	crea_VAO_Vector(&Piano);
	Piano.nome = "Piano Terra";
	Piano.Model = mat4(1.0);
	Piano.Model = translate(Piano.Model, vec3(0.0, -1.5, 0.0));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Piano.Model = scale(Piano.Model, vec3(1000.0f, 1.0f, 1000.0f));
	raggi.push_back(1.5);
	Scena.push_back(Piano);
}

void INIT_Illuminazione() {
	
}

void INIT_CAMERA_PROJECTION()
{
	//Imposto la telecamera
	ViewSetup = {};
	ViewSetup.position = glm::vec4(0.0, 0.5, 25.0, 0.0);
	ViewSetup.target = glm::vec4(0.0, 0.0, 0.0, 0.0);
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);

	//Imposto la proiezione prospettica
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
	widthRes = w;
	heightRes = h;
}

// Windows info
void resizeSecWind(int w, int h)
{
	glViewport(0, 0, w, h);
	Projection_text = ortho(0.0f, (float)w, 0.0f, (float)h);

	widthRes = w;
	heightRes = h;
}

// Da sistemare
void drawScene(void)
{
	resolution.x = (float)width;
	resolution.y = (float)height;

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Projection = perspective(radians(PerspectiveSetup.fovY), PerspectiveSetup.aspect, PerspectiveSetup.near_plane, PerspectiveSetup.far_plane);
	//Costruisco la matrice di Vista che applicata ai vertici in coordinate del mondo li trasforma nel sistema di riferimento della camera.
	View = lookAt(vec3(ViewSetup.position), vec3(ViewSetup.target), vec3(ViewSetup.upVector));
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

	//Disegno Sfondo: uso il program shder per lo sfondo
	glUseProgram(programId_Sfondo);
	//Trasferisco le variabili uniformi allo shader sfondo: tempo, risoluzione e posizione del mouse, Matrice di Proiezione e Matrice di Vista, Matrice di Modellazione
	glUniform1f(loc_time, time);
	glUniform2f(loc_res, resolution.x, resolution.y);
	glUniform2f(loc_mouse, mousepos.x, mousepos.y);

	glUniformMatrix4fv(MatrixProjS, 1, GL_FALSE, value_ptr(Projection));
	glUniformMatrix4fv(MatViewS, 1, GL_FALSE, value_ptr(View));

	glUniformMatrix4fv(MatModelS, 1, GL_FALSE, value_ptr(Scena[0].Model));
	glBindVertexArray(Scena[0].VAO);
	glDrawElements(GL_TRIANGLES, (Scena[0].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//Da adesso in avanti userò il program shader per gli altri oggetti
	glUseProgram(programId);
	glUniformMatrix4fv(MatrixProj, 1, GL_FALSE, value_ptr(Projection));
	glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));

	//Disegno piano terra
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[1].Model));
	glBindVertexArray(Scena[1].VAO);
	glDrawElements(GL_TRIANGLES, (Scena[1].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);



	glutSwapBuffers();
}

// Windows info
void drawSceneSecWind(void)
{
	/*
	glClearColor(0.0, 1.0, 0.0, 0.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Projection_text = ortho(0.0f, (float)widthRes, 0.0f, (float)heightRes);

	RenderText(programId_text, Projection_text, Operazione, VAO_Text, VBO_Text, 10.0f, 100.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));

	RenderText(programId_text, Projection_text, stringa_asse, VAO_Text, VBO_Text, 10.0f, 70.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));

	RenderText(programId_text, Projection_text, "Oggetto selezionato", VAO_Text, VBO_Text, 10.0f, 50.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
	if (selected_obj > -1)
		RenderText(programId_text, Projection_text, Scena[selected_obj].nome.c_str(), VAO_Text, VBO_Text, 10.0f, 10.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));

	glutSwapBuffers();
	*/
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	//Inizializzo finestra per il rendering della scena 3d con tutti i suoi eventi le sue inizializzazioni e le sue impostazioni

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	idfg = glutCreateWindow("SquareCity");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutMouseFunc(mouse);
	glutTimerFunc(10, update, 0);
	glutPassiveMotionFunc(my_passive_mouse);

	glutKeyboardFunc(keyboardPressedEvent);
	glutTimerFunc(10, update, 0);

	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	INIT_CAMERA_PROJECTION();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Inizializzo finestra per il rendering delle informazioni con tutti i suoi eventi le sue inizializzazioni e le sue impostazioni
	/*
	glutInitWindowSize(width_i, height_i);
	glutInitWindowPosition(500, 350);
	idfi = glutCreateWindow("Informazioni");
	glutDisplayFunc(drawScene1);
	glutReshapeFunc(resize1);

	INIT_SHADER();
	INIT_VAO_Text();
	Init_Freetype();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	*/

	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Projection (in vertex shader).
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Projection al Vertex Shader
	MatrixProj = glGetUniformLocation(programId, "Projection");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatModel = glGetUniformLocation(programId, "Model");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 View (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice View al Vertex Shader
	MatView = glGetUniformLocation(programId, "View");


	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Projection (in vertex shader).
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Projection al Vertex Shader
	MatrixProjS = glGetUniformLocation(programId_Sfondo, "Projection");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatModelS = glGetUniformLocation(programId_Sfondo, "Model");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 View (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice View al Vertex Shader
	MatViewS = glGetUniformLocation(programId_Sfondo, "View");

	loc_time = glGetUniformLocation(programId_Sfondo, "timeS");

	loc_res = glGetUniformLocation(programId_Sfondo, "resolution");

	loc_mouse = glGetUniformLocation(programId_Sfondo, "mouse");
	glutMainLoop();
}