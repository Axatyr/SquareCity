#include "Lib.h"
#include "ShaderMaker.h"
#include "Geometria.h"
//#include "GestioneTesto.h"
#include "GestioneEventi.h"

#include "objloader.hpp"
//#include "VAO.h"
#include "Materiale.h"
//#include "Texture.h"
//#include "Loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define PI 3.14159265358979323846

// Dimensioni finestre
int width = 1280;
int height = 720;

// Per il resize
int widthRes = width;
int heightRes = height;

// Variabili per vertex shader
static unsigned int programId, programId_text, programId_Sfondo, programIdr;
static unsigned int  MatrixProj, MatrixProj_txt, MatrixProjS;
static unsigned int MatModel, MatModelS, MatView, MatViewS;
// Da guardare queste variabili a cosa servono
static unsigned int loc_time, loc_res, loc_mouse;
// Da capire pure queste
static unsigned int lsh, lscelta, loc_view_pos, lblinn;

// Da cancellare mi sa unsigned int VAO_Text, VBO_Text;
int selected_obj = -1;

//texture
unsigned int legno, pavimento_piazza, muro, luna, foglie, vetro, mosaico, tegole, planet;

// Gestione camera
int vistaCamera = 1; //vista generica, 2 per vista personaggio, 3 dall'alto 
string Operazione;
string stringa_asse;
int idPrincipale, idfi;
float cameraSpeed = 1.0;
vec3 asse = vec3(0.0, 1.0, 0.0);

bool firstMouse = true;
float lastX = (float)width / 2;
float lastY = (float)height / 2;
float yaw_ = -90.0f;
float pitch_ = 0.0f;

vector<vec3> lightPositions = {
	vec3(-13.0, 5.0, 12.0),
	vec3(-13.0, 5.0, -12.0),
	vec3(13.0, 5.0, -12.0),
	vec3(13.0, 5.0, 12.0)
};
LightShaderUniform light_unif[5];
point_light lights[5];
float angolo = 0.0; // Per luce
GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 0.1f };
GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat pos1[] = { -13.0, 5.0, 12.0 };
GLfloat pos2[] = { -13.0, 5.0, -12.0 };
GLfloat pos3[] = { 13.0, 5.0, -12.0 };
GLfloat pos4[] = { 13.0, 5.0, 12.0 };

// Gestione movimento sole
bool mattina = true, pomeriggio = false, sera = false, notte = false;
float est = -100.0, nord = 70.0, ovest = 300.0, sud = 70.0;
float fattoreVelocita = 0.2;

enum {
	NAVIGATION,
	CAMERA_MOVING,
	TRASLATING,
	ROTATING,
	SCALING
} OperationMode;

enum {
	X,
	Y,
	Z
} WorkingAxis;

// Gestione update
float angoloUp = 0;

// Oggetti nella scena
static vector<Mesh> Personaggio, Lampione, Edificio1, Edificio2, Edificio3, Edificio4;
Mesh Cubo, Piano, Piramide, Centri, Pianeta, Panchina, Albero, Fontana3D, Palo, Lampada, Piazza, Navicella;
Mesh Palazzo, Tetto, Porta, Finestra1, Finestra2;
Mesh Palazzo2, Porta2, Finestra3, Finestra4, Finestra5;
Mesh Palazzo3, Porta3, Finestra6, Finestra7;
Mesh Palazzo4, Tetto2, Porta4, Finestra8;
Figura Fontana;
glm::vec4 currentPositionCharacter;
glm::vec4 currentTargetCharacter;

// Da controllare a cosa serve
static float quan = 0;
float posxN, posyN;
vec2 resolution, mousepos;

mat4 Projection, Projection_text, Model, View;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

static vector<Material> materials;
static vector<Shader> shaders;

vector<std::string> faces{
	"tegole.jpg",
	"tegole.jpg",
	"tegole.jpg",
	"tegole.jpg",
	//"tegole.jpg"
};

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int widthText, heightText, nrComponents;
	unsigned char* data = stbi_load(path, &widthText, &heightText, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, widthText, heightText, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void genera_texture(void)
{
	glGenTextures(1, &muro);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, muro);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int widthT, heightT, nrChannels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = stbi_load("muro.jpg", &widthT, &heightT, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 3)
			format = GL_RGB;

		if (nrChannels == 4)
			format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, widthT, heightT, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		printf("Tutto OK muro \n");
	}
	else
	{
		printf("Errore nel caricare la texture muro\n");
	}
	stbi_image_free(data);

	//luna
	glGenTextures(1, &luna);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, luna);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	data = stbi_load("luna.jpg", &widthT, &heightT, &nrChannels, 0);
	stbi_set_flip_vertically_on_load(1);
	if (data)
	{
		GLenum format;
		if (nrChannels == 3)
			format = GL_RGB;
		if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, widthT, heightT, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		printf("Tutto OK luna \n");
	}
	else
	{
		printf("Errore nel caricare la texture luna\n");
	}
	stbi_image_free(data);


}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

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
	
	vertexShader = (char*)"vertexShader_riflessione.glsl";
	fragmentShader = (char*)"fragmentShader_riflessione.glsl";

	programIdr = ShaderMaker::createProgram(vertexShader, fragmentShader);

	//Lights initialization and activation
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, pos1);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT2, GL_POSITION, pos2);
	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT3, GL_POSITION, pos3);
	glEnable(GL_LIGHT3);
	glLightfv(GL_LIGHT4, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT4, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT4, GL_POSITION, pos4);
	glEnable(GL_LIGHT4);
	glEnable(GL_LIGHTING);

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

	glGenBuffers(1, &mesh->VBO_normali);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_normali);
	glBufferData(GL_ARRAY_BUFFER, mesh->normali.size() * sizeof(vec3), mesh->normali.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO delle NORMALI nel layer 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &mesh->VBO_coord_texture);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_coord_texture);
	glBufferData(GL_ARRAY_BUFFER, mesh->texCoords.size() * sizeof(vec2), mesh->texCoords.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO delle NORMALI nel layer 2
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(3);

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

	legno = loadTexture("legno.jpg");
	pavimento_piazza = loadTexture("piazza.jpg");
	muro = loadTexture("muro.jpg");
	foglie = loadTexture("alien.jpg");
	luna = loadTexture("luna.jpg");
	vetro = loadTexture("vetro.jpg");
	mosaico = loadTexture("mosaico.jpg");
	tegole = loadCubemap(faces);
	planet = loadTexture("planet.jpg");

	Mesh Sfondo;
	string ObjDir = "object/";
	bool obj;

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
	Sfondo.sceltaVS = 1;
	Sfondo.material = MaterialType::EMERALD;
	Scena.push_back(Sfondo);

	//TERRENO
	crea_piano(&Piano, vec4(1.0, 1.0, 1.0, 1.0));
	crea_VAO_Vector(&Piano);
	Piano.nome = "Piano Terra";
	Piano.Model = mat4(1.0);
	Piano.Model = translate(Piano.Model, vec3(0.0, -1.5, 0.0));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Piano.Model = scale(Piano.Model, vec3(1000.0f, 1.0f, 1000.0f));
	raggi.push_back(1.5);
	Piano.sceltaVS = 0;
	Scena.push_back(Piano);

	//PIAZZA
	crea_piano(&Piazza, vec4(1.0, 105.0/255.0, 180.0/255.0, 1.0));
	crea_VAO_Vector(&Piazza);
	Piazza.nome = "piazza";
	Piazza.Model = mat4(1.0);
	Piazza.Model = translate(Piazza.Model, vec3(0.0, -1.3, 0.0));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Piazza.Model = scale(Piazza.Model, vec3(15.0, 1.0f, 15.0));
	raggi.push_back(1.5);
	Piazza.sceltaVS = 0;
	Piazza.material = MaterialType::EMERALD;
	Scena.push_back(Piazza);

	//SOLE
	crea_sfera(&Pianeta, vec4(148.0/255.0, 0.0, 211.0/255.0, 1.0));
	crea_VAO_Vector(&Pianeta);
	Pianeta.posx = est;
	Pianeta.posy = 0.0;
	Pianeta.posz = 0.0;
	Pianeta.Model = mat4(1.0);
	Pianeta.Model = translate(Pianeta.Model, vec3(Pianeta.posx, Pianeta.posy, Pianeta.posz));
	Pianeta.Model = scale(Pianeta.Model, vec3(2.5, 2.5, 2.5));
	Pianeta.nome = "pianeta";
	centri.push_back(vec3(0.0, 2.0, 0.0));
	raggi.push_back(0.5);
	Pianeta.sceltaVS = 0;
	Pianeta.material = MaterialType::RED_PLASTIC;
	Scena.push_back(Pianeta);



	//PANCHINA	
	obj = loadOBJ(ObjDir + "panchina.obj", Panchina);
	for (int i = 0; i < Panchina.colori.size(); i++) {
		Panchina.colori[i] = vec4(139.0/255.0, 69.0/255.0, 19.0/255.0, 1.0);
	}
	crea_VAO_Vector(&Panchina);
	Panchina.nome = "panchina";
	Panchina.Model = mat4(1.0);
	Panchina.Model = rotate(Panchina.Model, radians(180.0f), vec3(0.0, 1.0, 0.0));
	centri.push_back(vec3(Panchina.Model * vec4(0.0, 0.0, 0.0, 1.0)));
	raggi.push_back(1.0);
	Panchina.sceltaVS = 0;
	Panchina.material = MaterialType::SLATE;
	Scena.push_back(Panchina);
	Panchina.Model = translate(Panchina.Model, vec3(-7.0, -1.0, 10.0));
	Panchina.Model = scale(Panchina.Model, vec3(0.5, 0.5, 0.5));
	centri.push_back(vec3(Panchina.Model * vec4(0.0, 0.0, 0.0, 1.0)));
	raggi.push_back(1.0);
	Panchina.sceltaVS = 0;
	Panchina.material = MaterialType::SLATE;
	Scena.push_back(Panchina);

	//NAVICELLA
	obj = loadOBJ(ObjDir + "navicella.obj", Navicella);
	for (int i = 0; i < Navicella.colori.size(); i++) {
		Navicella.colori[i] = vec4(30.0/255.0, 144.0 / 255.0, 1.0, 1.0);
	}
	crea_VAO_Vector(&Navicella);
	Navicella.nome = "navicella";
	Navicella.Model = mat4(1.0);
	Navicella.Model = translate(Navicella.Model, vec3(-40.0, 0.0, 10.0));
	Navicella.Model = scale(Navicella.Model, vec3(2.0, 2.0, 2.0));
	centri.push_back(vec3(Navicella.Model * vec4(0.0, 0.0, 0.0, 1.0)));
	raggi.push_back(1.0);
	Navicella.sceltaVS = 0;
	Navicella.material = MaterialType::EMERALD;
	Scena.push_back(Navicella);

	//ALBERO
	obj = loadOBJ(ObjDir + "AlienTree.obj", Albero);
	for (int i = 0; i < Albero.colori.size(); i++) {
		Albero.colori[i] = vec4(0.0, 100.0/255.0, 0.0, 1.0);
	}
	crea_VAO_Vector(&Albero);
	Albero.nome = "albero";
	centri.push_back(vec3(Albero.Model * vec4(0.0, 0.0, 0.0, 1.0)));
	raggi.push_back(1.0);
	Scena.push_back(Albero);

	//LAMPIONE
	//Palo
	crea_cilindro(&Palo, vec4(0.5, 0.5, 0.5, 1.0));
	Palo.texCoords.clear();
	crea_VAO_Vector(&Palo);
	Palo.nome = "palo";
	centri.push_back(vec3(0.0, 2.0, 0.0));
	raggi.push_back(0.5);
	Lampione.push_back(Palo);
	//Lampada
	crea_sfera(&Lampada, vec4(1.0, 1.0, 1.0, 1.0));
	crea_VAO_Vector(&Lampada);
	Lampada.nome = "lampada";
	centri.push_back(vec3(0.0, 2.0, 0.0));
	raggi.push_back(0.5);
	Lampione.push_back(Lampada);

	//Edifici
	//1
	//palazzo
	crea_cubo(&Palazzo, vec4(222.0/255.0, 184.0/255.0, 135.0/255.0, 1.0), vec4(222.0 / 255.0, 184.0 / 255.0, 135.0 / 255.0, 1.0));
	Palazzo.texCoords.clear();
	for (int i = 0; i < Palazzo.vertici.size(); i++) {
		Palazzo.texCoords.push_back(Palazzo.vertici[i]);
	}
	crea_VAO_Vector(&Palazzo);
	Palazzo.Model = mat4(1.0);
	Palazzo.Model = translate(Palazzo.Model, vec3(0.0, 1.0, -20.0));
	Palazzo.Model = scale(Palazzo.Model, vec3(5.0, 5.0, 5.0));
	Palazzo.nome = "palazzo";
	centri.push_back(vec3(0.0, 2.0, 0.0));
	raggi.push_back(0.5);
	Palazzo.sceltaVS = 0;
	Palazzo.material = MaterialType::RED_PLASTIC;
	Edificio1.push_back(Palazzo);
	//tetto
	crea_piramide(&Tetto, vec4(1.0, 0.0, 0.0, 1.0));
	for (int i = 0; i < Tetto.vertici.size(); i++) {
		Tetto.texCoords.push_back(Tetto.vertici[i]);
	}
	crea_VAO_Vector(&Tetto);
	Tetto.Model = mat4(1.0);
	Tetto.Model = translate(Tetto.Model, vec3(0.0, 6.0, -20.0));
	Tetto.Model = scale(Tetto.Model, vec3(5.0, 4.0, 5.0));
	Tetto.nome = "tetto";
	centri.push_back(vec3(0.0, 2.0, 0.0));
	raggi.push_back(0.5);
	Tetto.sceltaVS = 0;
	Tetto.material = MaterialType::RED_PLASTIC;
	Edificio1.push_back(Tetto);
	//porta
	crea_piano(&Porta, vec4(139.0/255.0, 69.0/255.0, 19.0/255.0, 1.0));
	crea_VAO_Vector(&Porta);
	Porta.nome = "porta";
	Porta.Model = mat4(1.0);
	Porta.Model = translate(Porta.Model, vec3(0.0, 0.0, -14.9));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Porta.Model = scale(Porta.Model, vec3(1.0, 2.0, 2.0));
	Porta.Model = rotate(Porta.Model, radians(90.0f), vec3(1.0, 0.0, 0.0));
	raggi.push_back(1.5);
	Porta.sceltaVS = 0;
	Porta.material = MaterialType::EMERALD;
	Edificio1.push_back(Porta);
	//finestra1
	crea_piano(&Finestra1, vec4(0.0, 0.0, 1.0, 1.0));
	crea_VAO_Vector(&Finestra1);
	Finestra1.nome = "finestra1";
	Finestra1.Model = mat4(1.0);
	Finestra1.Model = translate(Finestra1.Model, vec3(-2.0, 4.0, -14.9));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Finestra1.Model = scale(Finestra1.Model, vec3(1.0, 1.0, 1.0));
	Finestra1.Model = rotate(Finestra1.Model, radians(90.0f), vec3(1.0, 0.0, 0.0));
	raggi.push_back(1.5);
	Finestra1.sceltaVS = 0;
	Finestra1.material = MaterialType::EMERALD;
	Edificio1.push_back(Finestra1);
	//finestra2
	crea_piano(&Finestra2, vec4(0.0, 0.0, 1.0, 1.0));
	crea_VAO_Vector(&Finestra2);
	Finestra2.nome = "finestra2";
	Finestra2.Model = mat4(1.0);
	Finestra2.Model = translate(Finestra2.Model, vec3(2.0, 4.0, -14.9));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Finestra2.Model = scale(Finestra2.Model, vec3(1.0, 1.0, 1.0));
	Finestra2.Model = rotate(Finestra2.Model, radians(90.0f), vec3(1.0, 0.0, 0.0));
	raggi.push_back(1.5);
	Finestra2.sceltaVS = 0;
	Finestra2.material = MaterialType::EMERALD;
	Edificio1.push_back(Finestra2);

	//2
	//palazzo
	crea_cubo(&Palazzo2, vec4(222.0 / 255.0, 184.0 / 255.0, 135.0 / 255.0, 1.0), vec4(222.0 / 255.0, 184.0 / 255.0, 135.0 / 255.0, 1.0));
	Palazzo2.texCoords.clear();
	for (int i = 0; i < Palazzo2.vertici.size(); i++) {
		Palazzo2.texCoords.push_back(Palazzo2.vertici[i]);
	}
	crea_VAO_Vector(&Palazzo2);
	Palazzo2.Model = mat4(1.0);
	Palazzo2.Model = translate(Palazzo2.Model, vec3(-20, 1.0, 0.0));
	Palazzo2.Model = scale(Palazzo2.Model, vec3(5.0, 10.0, 5.0));
	Palazzo2.nome = "palazzo2";
	centri.push_back(vec3(0.0, 2.0, 0.0));
	raggi.push_back(0.5);
	Palazzo2.sceltaVS = 0;
	Palazzo2.material = MaterialType::RED_PLASTIC;
	Edificio2.push_back(Palazzo2);
	//porta
	crea_piano(&Porta2, vec4(139.0 / 255.0, 69.0 / 255.0, 19.0 / 255.0, 1.0));
	crea_VAO_Vector(&Porta2);
	Porta2.nome = "porta2";
	Porta2.Model = mat4(1.0);
	Porta2.Model = translate(Porta2.Model, vec3(-14.9, 0.0, 0.0));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Porta2.Model = scale(Porta2.Model, vec3(2.0, 2.0, 1.0));
	Porta2.Model = rotate(Porta2.Model, radians(90.0f), vec3(0.0, 0.0, 1.0));
	raggi.push_back(1.5);
	Porta2.sceltaVS = 0;
	Porta2.material = MaterialType::EMERALD;
	Edificio2.push_back(Porta2);
	//finestra3
	crea_piano(&Finestra3, vec4(0.0, 0.0, 1.0, 1.0));
	crea_VAO_Vector(&Finestra3);
	Finestra3.nome = "finestra3";
	Finestra3.Model = mat4(1.0);
	Finestra3.Model = translate(Finestra3.Model, vec3(-14.9, 4.0, -2.0));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Finestra3.Model = scale(Finestra3.Model, vec3(1.0, 1.0, 1.0));
	Finestra3.Model = rotate(Finestra3.Model, radians(90.0f), vec3(0.0, 0.0, 1.0));
	raggi.push_back(1.5);
	Finestra3.sceltaVS = 0;
	Finestra3.material = MaterialType::EMERALD;
	Edificio2.push_back(Finestra3);
	//finestra4
	crea_piano(&Finestra4, vec4(0.0, 0.0, 1.0, 1.0));
	crea_VAO_Vector(&Finestra4);
	Finestra4.nome = "finestra4";
	Finestra4.Model = mat4(1.0);
	Finestra4.Model = translate(Finestra4.Model, vec3(-14.9, 6.0, 2.0));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Finestra4.Model = scale(Finestra4.Model, vec3(1.0, 1.0, 1.0));
	Finestra4.Model = rotate(Finestra4.Model, radians(90.0f), vec3(0.0, 0.0, 1.0));
	raggi.push_back(1.5);
	Finestra4.sceltaVS = 0;
	Finestra4.material = MaterialType::EMERALD;
	Edificio2.push_back(Finestra4);
	//finestra5
	crea_piano(&Finestra5, vec4(0.0, 0.0, 1.0, 1.0));
	crea_VAO_Vector(&Finestra5);
	Finestra5.nome = "finestra5";
	Finestra5.Model = mat4(1.0);
	Finestra5.Model = translate(Finestra5.Model, vec3(-14.9, 9.0, -2.0));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Finestra5.Model = scale(Finestra5.Model, vec3(1.0, 1.0, 1.0));
	Finestra5.Model = rotate(Finestra5.Model, radians(90.0f), vec3(0.0, 0.0, 1.0));
	raggi.push_back(1.5);
	Finestra5.sceltaVS = 0;
	Finestra5.material = MaterialType::EMERALD;
	Edificio2.push_back(Finestra5);
	//3
	//palazzo
	crea_cubo(&Palazzo3, vec4(222.0 / 255.0, 184.0 / 255.0, 135.0 / 255.0, 1.0), vec4(222.0 / 255.0, 184.0 / 255.0, 135.0 / 255.0, 1.0));
	Palazzo3.texCoords.clear();
	for (int i = 0; i < Palazzo3.vertici.size(); i++) {
		Palazzo3.texCoords.push_back(Palazzo3.vertici[i]);
	}
	crea_VAO_Vector(&Palazzo3);
	Palazzo3.Model = mat4(1.0);
	Palazzo3.Model = translate(Palazzo3.Model, vec3(20, 1.0, -5.0));
	Palazzo3.Model = scale(Palazzo3.Model, vec3(5.0, 10.0, 5.0));
	Palazzo3.nome = "palazzo3";
	centri.push_back(vec3(0.0, 2.0, 0.0));
	raggi.push_back(0.5);
	Palazzo3.sceltaVS = 0;
	Palazzo3.material = MaterialType::RED_PLASTIC;
	Edificio3.push_back(Palazzo3);
	//porta
	crea_piano(&Porta3, vec4(139.0 / 255.0, 69.0 / 255.0, 19.0 / 255.0, 1.0));
	crea_VAO_Vector(&Porta3);
	Porta3.nome = "porta3";
	Porta3.Model = mat4(1.0);
	Porta3.Model = translate(Porta3.Model, vec3(14.9, 0.0, -5.0));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Porta3.Model = scale(Porta3.Model, vec3(2.0, 2.0, 1.0));
	Porta3.Model = rotate(Porta3.Model, radians(90.0f), vec3(0.0, 0.0, 1.0));
	raggi.push_back(1.5);
	Porta3.sceltaVS = 0;
	Porta3.material = MaterialType::EMERALD;
	Edificio3.push_back(Porta3);
	//finestra6
	crea_piano(&Finestra6, vec4(0.0, 0.0, 1.0, 1.0));
	crea_VAO_Vector(&Finestra6);
	Finestra6.nome = "finestra6";
	Finestra6.Model = mat4(1.0);
	Finestra6.Model = translate(Finestra6.Model, vec3(14.9, 5.0, -7.0));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Finestra6.Model = scale(Finestra6.Model, vec3(2.0, 2.0, 1.0));
	Finestra6.Model = rotate(Finestra6.Model, radians(90.0f), vec3(0.0, 0.0, 1.0));
	raggi.push_back(1.5);
	Finestra6.sceltaVS = 0;
	Finestra6.material = MaterialType::EMERALD;
	Edificio3.push_back(Finestra6);
	//finestra7
	crea_piano(&Finestra7, vec4(0.0, 0.0, 1.0, 1.0));
	crea_VAO_Vector(&Finestra7);
	Finestra7.nome = "finestra7";
	Finestra7.Model = mat4(1.0);
	Finestra7.Model = translate(Finestra7.Model, vec3(14.9, 7.0, -3.0));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Finestra7.Model = scale(Finestra7.Model, vec3(2.0, 2.0, 1.0));
	Finestra7.Model = rotate(Finestra7.Model, radians(90.0f), vec3(0.0, 0.0, 1.0));
	raggi.push_back(1.5);
	Finestra7.sceltaVS = 0;
	Finestra7.material = MaterialType::EMERALD;
	Edificio3.push_back(Finestra7);
	//4
	//palazzo
	crea_cubo(&Palazzo4, vec4(222.0 / 255.0, 184.0 / 255.0, 135.0 / 255.0, 1.0), vec4(222.0 / 255.0, 184.0 / 255.0, 135.0 / 255.0, 1.0));
	Palazzo4.texCoords.clear();
	for (int i = 0; i < Palazzo4.vertici.size(); i++) {
		Palazzo4.texCoords.push_back(Palazzo4.vertici[i]);
	}
	crea_VAO_Vector(&Palazzo4);
	Palazzo4.Model = mat4(1.0);
	Palazzo4.Model = translate(Palazzo4.Model, vec3(20.0, 1.0, 5.0));
	Palazzo4.Model = scale(Palazzo4.Model, vec3(5.0, 5.0, 5.0));
	Palazzo4.nome = "palazzo4";
	centri.push_back(vec3(0.0, 2.0, 0.0));
	raggi.push_back(0.5);
	Palazzo4.sceltaVS = 0;
	Palazzo4.material = MaterialType::RED_PLASTIC;
	Edificio4.push_back(Palazzo4);
	//tetto
	crea_piramide(&Tetto2, vec4(1.0, 0.0, 0.0, 1.0));
	for (int i = 0; i < Tetto2.vertici.size(); i++) {
		Tetto2.texCoords.push_back(Tetto2.vertici[i]);
	}
	crea_VAO_Vector(&Tetto2);
	Tetto2.Model = mat4(1.0);
	Tetto2.Model = translate(Tetto2.Model, vec3(20.0, 6.0, 5.0));
	Tetto2.Model = scale(Tetto2.Model, vec3(5.0, 4.0, 5.0));
	Tetto2.nome = "tetto2";
	centri.push_back(vec3(0.0, 2.0, 0.0));
	raggi.push_back(0.5);
	Tetto2.sceltaVS = 0;
	Tetto2.material = MaterialType::RED_PLASTIC;
	Edificio4.push_back(Tetto2);
	//porta
	crea_piano(&Porta4, vec4(139.0 / 255.0, 69.0 / 255.0, 19.0 / 255.0, 1.0));
	crea_VAO_Vector(&Porta4);
	Porta4.nome = "porta4";
	Porta4.Model = mat4(1.0);
	Porta4.Model = translate(Porta4.Model, vec3(14.9, 0.0, 5.0));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Porta4.Model = scale(Porta4.Model, vec3(2.0, 2.0, 1.0));
	Porta4.Model = rotate(Porta4.Model, radians(90.0f), vec3(0.0, 0.0, 1.0));
	raggi.push_back(1.5);
	Porta4.sceltaVS = 0;
	Porta4.material = MaterialType::EMERALD;
	Edificio4.push_back(Porta4);
	//finestra8
	crea_piano(&Finestra8, vec4(0.0, 0.0, 1.0, 1.0));
	crea_VAO_Vector(&Finestra8);
	Finestra8.nome = "finestra8";
	Finestra8.Model = mat4(1.0);
	Finestra8.Model = translate(Finestra8.Model, vec3(14.9, 4.0, 5.0));
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Finestra8.Model = scale(Finestra8.Model, vec3(1.0, 1.0, 3.0));
	Finestra8.Model = rotate(Finestra8.Model, radians(90.0f), vec3(0.0, 0.0, 1.0));
	raggi.push_back(1.5);
	Finestra8.sceltaVS = 0;
	Finestra8.material = MaterialType::EMERALD;
	Edificio4.push_back(Finestra8);
	
	//FONTANA
	costruisci_fontana(vec4(0.0, 0.0, 1.0, 1.0) , vec4(0.5, 0.5, 0.5, 1.0), &Fontana);
	rivoluzione(Fontana, &Fontana3D);
	Fontana3D.texCoords.clear();
	for (int i = 0; i < Fontana3D.vertici.size(); i++) {
		Fontana3D.texCoords.push_back(Fontana3D.vertici[i]);
	}
	crea_VAO_Vector(&Fontana3D);
	Fontana3D.nome = "fontana";
	Fontana3D.Model = mat4(1.0);
	Fontana3D.Model = translate(Fontana3D.Model, vec3(0.0, 1.0, 1.0));
	Fontana3D.Model = scale(Fontana3D.Model, vec3(1.3, 1.3, 1.3));
	Fontana3D.sceltaVS = 0;
	Fontana3D.material = MaterialType::EMERALD;
	Scena.push_back(Fontana3D);

	//COSTRZIONE DEL PERSONAGGIO
	Mesh Testa, Corpo, BraccioSx, BraccioDx, GambaSx, GambaDx;

	//TESTA
	crea_sfera(&Testa, vec4(0.0, 0.5, 1.0, 1.0));
	Testa.texCoords.clear();
	crea_VAO_Vector(&Testa);
	Testa.Model = mat4(1.0);
	Testa.Model = translate(Testa.Model, vec3(0.0, 1.75, 0.0));
	Testa.Model = scale(Testa.Model, vec3(0.75, 0.75, 0.75));
	Testa.nome = "Testa";
	centri.push_back(vec3(0.0, 1.75, 0.0));
	raggi.push_back(0.5);
	Personaggio.push_back(Testa);

	//CORPO
	crea_sfera(&Corpo, vec4(1.0, 1.0, 1.0, 1.0));
	Corpo.texCoords.clear();
	crea_VAO_Vector(&Corpo);
	Corpo.Model = mat4(1.0);
	Corpo.Model = translate(Corpo.Model, vec3(0.0, -0.2, 0.0));
	Corpo.Model = scale(Corpo.Model, vec3(1.2, 1.4, 1.0));
	raggi.push_back(0.5);
	Corpo.nome = "Corpo";
	centri.push_back(vec3(0.0, 0.0, 0.0));
	Personaggio.push_back(Corpo);
	raggi.push_back(0.5);
	
	//BRACCIO SINISTRO
	crea_sfera(&BraccioSx, vec4(1.0, 1.0, 1.0, 1.0));
	BraccioSx.texCoords.clear();
	crea_VAO_Vector(&BraccioSx);
	BraccioSx.Model = mat4(1.0);
	BraccioSx.Model = translate(BraccioSx.Model, vec3(-1.0, 0.0, 0.0));
	BraccioSx.Model = rotate(BraccioSx.Model, radians(-65.0f), vec3(0.0, 0.0, 1.0));
	BraccioSx.Model = scale(BraccioSx.Model, vec3(1, 0.25, 0.25));
	BraccioSx.nome = "BraccioSinistro";
	centri.push_back(vec3(-1.0, 0.0, 0.0));
	raggi.push_back(0.5);
	Personaggio.push_back(BraccioSx); 

	//BRACCIO DESTRO
	crea_sfera(&BraccioDx, vec4(1.0, 1.0, 1.0, 1.0));
	BraccioDx.texCoords.clear();
	crea_VAO_Vector(&BraccioDx);
	BraccioDx.Model = mat4(1.0);
	BraccioDx.Model = translate(BraccioDx.Model, vec3(1.0, 0.0, 0.0));
	BraccioDx.Model = rotate(BraccioDx.Model, radians(65.0f), vec3(0.0, 0.0, 1.0));
	BraccioDx.Model = scale(BraccioDx.Model, vec3(1, 0.25, 0.25));
	BraccioDx.nome = "BraccioDestro";
	centri.push_back(vec3(1.0, 0.0, 0.0));
	raggi.push_back(0.5);
	Personaggio.push_back(BraccioDx);
	//Assegno le posizioni iniziali dell'omino
	for (int i = 0; i < Personaggio.size(); i++) {
		Personaggio[i].posx = 0.0;
		Personaggio[i].posy = 0.0;
		Personaggio[i].posz = 10.0;
	}
	currentPositionCharacter = { Personaggio[0].posx, 1.70, Personaggio[0].posz, 0.0 };
	currentTargetCharacter = { Personaggio[0].posx, 1.70, Personaggio[0].posz  + 2.0, 0.0 };
}

void INIT_Illuminazione() {
	//luce sole

	lights[0].position = { 0.0, 21.0, 0.0 };
	lights[0].color = { 1.0, 1.0, 1.0 };
	lights[0].power = 1.5f;
	
	//luce lampioni
	for (int i = 1; i < lightPositions.size(); i++) {
		lights[i].position = lightPositions[i];
		lights[i].color = { 1.0, 1.0, 1.0 };
		lights[i].power = .7f;
	}

	//Setup dei materiali
	// Materials setup
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
	materials[MaterialType::NO_MATERIAL].ambient = glm::vec3(1, 1, 1);
	materials[MaterialType::NO_MATERIAL].diffuse = glm::vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].specular = glm::vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].shininess = 1.f;

	//Setup degli shader
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
	//Imposto la telecamera
	ViewSetup = {};
	ViewSetup.position = glm::vec4(0.0, 0.5, 25.0, 0.0);
	ViewSetup.target = glm::vec4(0.0, 0.0, 0.0, 0.0);
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.upVector = glm::vec4(0.0, 1.0, 0.0, 0.0); //Imposto la proiezione prospettica
	PerspectiveSetup = {};
	PerspectiveSetup.aspect = (GLfloat)width / (GLfloat)height;
	PerspectiveSetup.fovY = 45.0f;
	PerspectiveSetup.far_plane = 2000.0f;
	PerspectiveSetup.near_plane = 0.1f;
}

//--------------MOVIMENTO CAMERA ------------------
void moveCameraLeft(int vistaCorrente) {
	if (vistaCorrente == 1) { //Standard
		glm::vec3 direzione_scorrimento = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector)); //direzione perpendicolare al piano individuato da direction e upvector
		ViewSetup.position -= glm::vec4(direzione_scorrimento, .0) * cameraSpeed;
		ViewSetup.target = ViewSetup.position + ViewSetup.direction * cameraSpeed;
	}
	else if (vistaCorrente == 2) { // Personaggio
		glm::vec3 direzione_scorrimento = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector));
		ViewSetup.position -= glm::vec4(direzione_scorrimento.x, 0.0, direzione_scorrimento.z, .0) * cameraSpeed;
		ViewSetup.target += glm::vec4(ViewSetup.direction.x, 0.0, ViewSetup.direction.z, 0.0);
	}
	else { //Alto
		ViewSetup.direction = vec4(-1.0, 0.0, 0.0, 0.0);
		ViewSetup.position += ViewSetup.direction * cameraSpeed;
		ViewSetup.target += ViewSetup.direction;
	}
}
void moveCameraRight(int vistaCorrente) {
	if (vistaCorrente == 1) { //Standard
		glm::vec3 direzione_scorrimento = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector));
		ViewSetup.position += glm::vec4(direzione_scorrimento, .0) * cameraSpeed;
		ViewSetup.target = ViewSetup.position + ViewSetup.direction;
	}
	else if (vistaCorrente == 2) { // Personaggio
		glm::vec3 direzione_scorrimento = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector));
		ViewSetup.position += glm::vec4(direzione_scorrimento.x, 0.0, direzione_scorrimento.z, .0) * cameraSpeed;
		ViewSetup.target += glm::vec4(ViewSetup.direction.x, 0.0, ViewSetup.direction.z, 0.0);
	}
	else { //Alto
		ViewSetup.direction = vec4(1.0, 0.0, 0.0, 0.0);
		ViewSetup.position += ViewSetup.direction * cameraSpeed;
		ViewSetup.target += ViewSetup.direction;
	}
}
void moveCameraForward(int vistaCorrente) {
	if (vistaCorrente == 1) { //Standard
		ViewSetup.position += ViewSetup.direction * cameraSpeed;
		ViewSetup.target = ViewSetup.position + ViewSetup.direction;
	}
	else if (vistaCorrente == 2) { // Personaggio
		ViewSetup.position += glm::vec4(ViewSetup.direction.x, 0.0, ViewSetup.direction.z, .0) * cameraSpeed;
		ViewSetup.target = ViewSetup.position + glm::vec4(ViewSetup.direction.x, 0.0, ViewSetup.direction.z, 0.0);
	}
	else { //Alto
		ViewSetup.direction = vec4(0.0, 0.0, -1.0, 0.0);
		ViewSetup.position += ViewSetup.direction * cameraSpeed;
		ViewSetup.target += ViewSetup.direction;
	}
}
void moveCameraBack(int vistaCorrente) {
	if (vistaCorrente == 1) { //Standard
		ViewSetup.position -= ViewSetup.direction * cameraSpeed;
		ViewSetup.target = ViewSetup.position + ViewSetup.direction;
	}
	else if (vistaCorrente == 2) { //Personaggio
		ViewSetup.position -= glm::vec4(ViewSetup.direction.x, 0.0, ViewSetup.direction.z, .0) * cameraSpeed;
		ViewSetup.target = ViewSetup.position + glm::vec4(ViewSetup.direction.x, 0.0, ViewSetup.direction.z, 0.0);
	}
	else { //Alto
		ViewSetup.direction = vec4(0.0, 0.0, 1.0, 0.0);
		ViewSetup.position += ViewSetup.direction * cameraSpeed;
		ViewSetup.target += ViewSetup.direction;
	}
}
void zoomIn() { // Abbassarsi lungo l'asse y
	ViewSetup.direction = vec4(0.0, 1.0, 0.0, 0.0);
	ViewSetup.position += ViewSetup.direction * cameraSpeed;
	ViewSetup.target += ViewSetup.direction;
}
void zoomOut() { // Alzarsi lungo l'asse y
	ViewSetup.direction = vec4(0.0, -1.0, 0.0, 0.0);
	ViewSetup.position += ViewSetup.direction * cameraSpeed;
	ViewSetup.target += ViewSetup.direction;
}
void cambioScena(int vistaCorrente) {
	/* In teoria sarà necessaio cambiare automaticamente la scena passando da fissa a mobile*/
	if (vistaCorrente == 1) {
		vistaCamera = 2;
		//Ancoro la camera al personaggio
		ViewSetup.position = currentPositionCharacter;
		ViewSetup.target = currentTargetCharacter;
	}
	else if (vistaCorrente == 2) {
		//Salvo la posizione corrente del personaggio
		currentPositionCharacter = ViewSetup.position;
		currentTargetCharacter = ViewSetup.target;
		vistaCamera = 3;
		//Posiziono la telecamera in alto
		ViewSetup.position = glm::vec4(0.0, 30, 25.0, 0.0);
		ViewSetup.target = glm::vec4(0.0, 0.0, 0.0, 0.0);
	}
	else {
		vistaCamera = 1;
		//Riposiziono la camera nella posizione e modalità iniziale
		ViewSetup.position = glm::vec4(0.0, 0.5, 25.0, 0.0);
		ViewSetup.target = glm::vec4(0.0, 0.0, 0.0, 0.0);
	}
}
//--------------FINE MOVIMENTO CAMERA ------------------


void modifyModelMatrix(glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor)
{
	//ricordare che mat4(1) costruisce una matrice identità di ordine 4
	mat4 traslation = glm::translate(glm::mat4(1), translation_vector);
	mat4 scale = glm::scale(glm::mat4(1), glm::vec3(scale_factor, scale_factor, scale_factor));
	mat4 rotation = glm::rotate(glm::mat4(1), angle, rotation_vector);

	//Modifica la matrice di Modellazione dell'oggetto della scena selezionato postmolitplicando per le matrici scale*rotation*traslation
	if (selected_obj > -1)
	{
		Scena[selected_obj].Model = Scena[selected_obj].Model * scale * rotation * traslation;
		centri[selected_obj] = centri[selected_obj] + translation_vector;
		raggi[selected_obj] = raggi[selected_obj] * scale_factor;
	}
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
/*
void resizeSecWind(int w, int h)
{
	glViewport(0, 0, w, h);
	Projection_text = ortho(0.0f, (float)w, 0.0f, (float)h);

	widthRes = w;
	heightRes = h;
}
*/

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

	//Definizione colore luce, posizione ed intensita'
	for (int i = 0; i < 5; i++) {
		glUniform3f(light_unif[i].light_position_pointer, lights[i].position.x + 50 * cos(radians(angolo)), lights[i].position.y, lights[i].position.z + 50 * sin(radians(angolo)));
		glUniform3f(light_unif[i].light_color_pointer, lights[i].color.r, lights[i].color.g, lights[i].color.b);
		glUniform1f(light_unif[i].light_power_pointer, lights[i].power);
	}

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
	glUniform1i(lscelta, Scena[1].sceltaVS);
	glBindTexture(GL_TEXTURE_2D, luna);
	glBindVertexArray(Scena[1].VAO); 
	glDrawElements(GL_TRIANGLES, (Scena[1].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//Disegno piazza
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[2].Model));
	glUniform1i(lscelta, Scena[2].sceltaVS);
	glBindTexture(GL_TEXTURE_2D, pavimento_piazza);
	glBindVertexArray(Scena[2].VAO);
	glDrawElements(GL_TRIANGLES, (Scena[2].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//Disegno Pianeta
	glUniform1i(lscelta, Scena[3].sceltaVS);
	mat4 Model = mat4(1.0);
	Model = translate(Model, vec3(Scena[3].posx, Scena[3].posy, Scena[3].posz)) * Scena[3].Model; 
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glBindTexture(GL_TEXTURE_2D, planet);
	glBindVertexArray(Scena[3].VAO);
	glDrawElements(GL_TRIANGLES, (Scena[3].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//Disegno oggetti
	for (int k = 5; k < Scena.size(); k++) {
		if (Scena[k].nome == "panchina") {
			glBindTexture(GL_TEXTURE_2D, legno);
		}
		else if (Scena[k].nome == "navicella") {
			glBindTexture(GL_TEXTURE_2D, foglie);
		}
		else if (Scena[k].nome == "fontana") {
			glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));
			glPointSize(2.0);
			glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Fontana3D.Model));
			glBindTexture(GL_TEXTURE_2D, mosaico);
			glBindVertexArray(Fontana3D.VAO);
			glDrawElements(GL_TRIANGLES, Fontana3D.indici.size() * sizeof(GLuint), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		else if (Scena[k].nome == "albero") {
			for (int i = 0; i < 7; i++) {
				Albero.Model = mat4(1.0);
				Albero.posy = -1;
				if (i == 0) {
					Albero.posx = -10.0;
					Albero.posz = -15.0;
				}
				else if (i == 1) {
					Albero.posx = 10.0;
					Albero.posz = -15.0;
				}
				else if (i == 2) {
					Albero.posx = 15.0;
					Albero.posz = -15.0;
				}
				else if (i == 3) {
					Albero.posx = 10.0;
					Albero.posz = 17.0;
				}
				else if (i == 4) {
					Albero.posx = -10.0;
					Albero.posz = 17.0;
				}
				else if (i == 5) {
					Albero.posx = -16.0;
					Albero.posz = 12.0;
				}
				else if (i == 6) {
					Albero.posx = -16.0;
					Albero.posz = -12.0;
				}
				Albero.Model = translate(Albero.Model, vec3(Albero.posx, Albero.posy, Albero.posz));
				Albero.Model = rotate(Albero.Model, radians(180.0f), vec3(0.0, 1.0, 0.0));
				Albero.Model = scale(Albero.Model, vec3(0.15, 0.15, 0.15));
				Albero.sceltaVS = 0;
				Albero.material = MaterialType::BRASS;
				glBindTexture(GL_TEXTURE_2D, foglie);
				glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Albero.Model));
				glUniform1i(lscelta, Albero.sceltaVS);
				glBindVertexArray(Albero.VAO);
				glDrawArrays(GL_TRIANGLES, 0, Albero.vertici.size());
			}
		}
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[k].Model));
		glUniform1i(lscelta, Scena[k].sceltaVS);
		glBindVertexArray(Scena[k].VAO);		
		glDrawArrays(GL_TRIANGLES, 0, Scena[k].vertici.size());	
		glBindVertexArray(0);
	}
	//Disegno Personaggio
	for (int k = 0; k < Personaggio.size(); k++)
	{
		mat4 Model = mat4(1.0);
		Model = translate(Model, vec3(Personaggio[k].posx, Personaggio[k].posy, Personaggio[k].posz)) * Personaggio[k].Model; glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
		glBindVertexArray(Personaggio[k].VAO);
		glDrawElements(GL_TRIANGLES, (Personaggio[k].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	//Disegno Lampione
	for (int k = 0; k < 4; k++) {
		Palo.Model = mat4(1.0);
		Lampada.Model = mat4(1.0);
		Palo.posy = -2.0;
		Lampada.posy = 4.0;
		if (k == 0) {
			Palo.posx = -13.0;
			Palo.posz = 12.0;
			Lampada.posx = -13.0;
			Lampada.posz = 12.0;
		}
		else if (k == 1) {
			Palo.posx = -13.0;
			Palo.posz = -12.0;
			Lampada.posx = -13.0;
			Lampada.posz = -12.0;
		}
		else if (k == 2) {
			Palo.posx = 13.0;
			Palo.posz = -12.0;
			Lampada.posx = 13.0;
			Lampada.posz = -12.0;
		}
		else if (k == 3) {
			Palo.posx = 13.0;
			Palo.posz = 12.0;
			Lampada.posx = 13.0;
			Lampada.posz = 12.0;
		}
		Palo.Model = translate(Palo.Model, vec3(Palo.posx, Palo.posy, Palo.posz));
		Palo.Model = scale(Palo.Model, vec3(0.1, 6.0, 0.1));
		Palo.sceltaVS = 0;
		Palo.material = MaterialType::RED_PLASTIC;
		Lampada.Model = translate(Lampada.Model, vec3(Lampada.posx, Lampada.posy, Lampada.posz));
		Lampada.Model = scale(Lampada.Model, vec3(0.5, 0.5, 0.5));
		Lampada.sceltaVS = 0;
		Lampada.material = MaterialType::RED_PLASTIC;
		Lampione[0] = Palo;
		Lampione[1] = Lampada;
		for (int i = 0; i < 2; i++) {
			glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Lampione[i].Model));
			glUniform1i(lscelta, Lampione[i].sceltaVS);
			glBindTexture(GL_TEXTURE_2D, vetro);
			glBindVertexArray(Lampione[i].VAO);
			glDrawElements(GL_TRIANGLES, (Lampione[i].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
	
	//Disegno case
	//1
	for (int i = 0; i < Edificio1.size(); i++) {
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Edificio1[i].Model));
		glUniform1i(lscelta, Edificio1[i].sceltaVS);
		if (Edificio1[i].nome == "palazzo") {
			glBindTexture(GL_TEXTURE_2D, muro);
		}
		else if (Edificio1[i].nome == "porta") {
			glBindTexture(GL_TEXTURE_2D, legno);
		}
		else if (Edificio1[i].nome == "tetto") {
			glBindTexture(GL_TEXTURE_CUBE_MAP, tegole);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, vetro);
		}
		glBindVertexArray(Edificio1[i].VAO); 
		glDrawElements(GL_TRIANGLES, (Edificio1[i].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	//2
	for (int i = 0; i < Edificio2.size(); i++) {
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Edificio2[i].Model));
		glUniform1i(lscelta, Edificio2[i].sceltaVS);
		if (Edificio2[i].nome == "palazzo2") {
			glBindTexture(GL_TEXTURE_2D, muro);
		}
		else if (Edificio2[i].nome == "porta2") {
			glBindTexture(GL_TEXTURE_2D, legno);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, vetro);
		}
		glBindVertexArray(Edificio2[i].VAO);
		glDrawElements(GL_TRIANGLES, (Edificio2[i].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	//3
	for (int i = 0; i < Edificio3.size(); i++) {
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Edificio3[i].Model));
		glUniform1i(lscelta, Edificio3[i].sceltaVS);
		if (Edificio3[i].nome == "palazzo3") {
			glBindTexture(GL_TEXTURE_2D, muro);
		}
		else if (Edificio3[i].nome == "porta3") {
			glBindTexture(GL_TEXTURE_2D, legno);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, vetro);
		}
		glBindVertexArray(Edificio3[i].VAO);
		glDrawElements(GL_TRIANGLES, (Edificio3[i].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	//4
	for (int i = 0; i < Edificio4.size(); i++) {
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Edificio4[i].Model));
		glUniform1i(lscelta, Edificio4[i].sceltaVS);
		if (Edificio4[i].nome == "palazzo4") {
			glBindTexture(GL_TEXTURE_2D, muro);
		}
		else if (Edificio4[i].nome == "porta4") {
			glBindTexture(GL_TEXTURE_2D, legno);
		}
		else if (Edificio4[i].nome == "tetto2") {
			glBindTexture(GL_TEXTURE_CUBE_MAP, tegole);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, vetro);
		}
		glBindVertexArray(Edificio4[i].VAO);
		glDrawElements(GL_TRIANGLES, (Edificio4[i].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	/*
	glEnd();
	glFlush(); // This force the execution of OpenGL commands
	*/
	glutSwapBuffers();
}

// Windows info
/*
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
	
}
*/

void keyboardPressedEvent(unsigned char key, int x, int y) {
	
	switch (key)
	{
	case 'a':
		moveCameraLeft(vistaCamera);
		break;
	case 'd':
		moveCameraRight(vistaCamera);
		break;
	case 'w':
		moveCameraForward(vistaCamera);
		break;
	case 's':
		moveCameraBack(vistaCamera);
		break;
	case 'q':
		zoomIn();
		break;
	case 'e':
		zoomOut();
		break;
	case 'v':
		cout << "cambio scena" << endl;
		cambioScena(vistaCamera);
		break;
	case 'g':  //Si entra in modalità di operazione traslazione
		OperationMode = TRASLATING;
		Operazione = "TRASLAZIONE";
		break;
	case 'r': //Si entra in modalità di operazione rotazione
		OperationMode = ROTATING;
		Operazione = "ROTAZIONE";
		break;
	case 'S':
		OperationMode = SCALING; //Si entra in modalità di operazione scalatura
		Operazione = "SCALATURA";
		break;
	case 27:
		glutLeaveMainLoop();
		break;
		// Selezione dell'asse
	case 'x':
		WorkingAxis = X;  //Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		stringa_asse = " Asse X";
		break;
	case 'y':
		WorkingAxis = Y;  //Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		stringa_asse = " Asse Y";
		break;
	case 'z':
		WorkingAxis = Z;
		stringa_asse = " Asse Z";  //Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		break;

	default:
		break;
	}

	// Selezione dell'asse per le trasformazioni
	switch (WorkingAxis) {
	case X:	asse = glm::vec3(1.0, 0.0, 0.0);

		break;
	case Y: asse = glm::vec3(0.0, 1.0, 0.0);

		break;
	case Z: asse = glm::vec3(0.0, 0.0, 1.0);

		break;
	default:
		break;
	}

	/*
	glutSetWindow(idfi);
	glutPostRedisplay();
	*/

	// I tasti + e -  aggiornano lo spostamento a destra o a sinistra, la rotazione in segno antiorario o in senso orario, la scalatura come amplificazione o diminuizione delle dimensioni

	float amount = .1;
	if (key == '+')
		amount *= 1;

	if (key == '-')
		amount *= -1;

	switch (OperationMode) {

		//la funzione modifyModelMatrix(glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor) 
		// definisce la matrice di modellazione che si vuole postmoltiplicare alla matrice di modellazione dell'oggetto selezionato, per poterlo traslare, ruotare scalare.

	case TRASLATING:
		// si passa angle 0 e scale factor =1, 
		modifyModelMatrix(asse * amount, asse, 0.0f, 1.0f);
		break;
	case ROTATING:
		// SI mette a zero il vettore di traslazione (vec3(0) e ad 1 il fattore di scale
		modifyModelMatrix(glm::vec3(0), asse, amount * 2.0f, 1.0f);
		break;
	case SCALING:
		// SI mette a zero il vettore di traslazione (vec3(0), angolo di rotazione a 0 e ad 1 il fattore di scala 1+amount.
		modifyModelMatrix(glm::vec3(0), asse, 0.0f, 1.0f + amount);
		break;
	}

	/*
	glutSetWindow(idfi);
	glutPostRedisplay();
	*/

	glutSetWindow(idPrincipale);
	glutPostRedisplay();
}

// Gestione selezione oggetti schermata secondaria
vec3 get_ray_from_mouse(float mouse_x, float mouse_y) {


	// mappiamo le coordinate di viewport del mouse [0,width], [height,0] in coordinate normalizzate [-1,1]  
	float x = (2.0f * mouse_x) / width - 1.0;
	float y = 1.0f - (2.0f * mouse_y) / height;
	float z = 1.0f;
	vec3  ray_nds = vec3(x, y, z);
	// clip space
	vec4 ray_clip = vec4(x, y, -1.0, 1.0);

	// eye space

	vec4 ray_eye = inverse(Projection) * ray_clip;


	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	// world space
	vec3 ray_wor = vec3(inverse(View) * ray_eye);

	ray_wor = normalize(ray_wor);

	return ray_wor;
}

/*controlla se un raggio interseca una sfera. In caso negativo, restituisce false. Rigetta
le intersezioni dietro l'origine del raggio, e pone  intersection_distance all'intersezione p iù vicina.
*/

bool ray_sphere(vec3 ray_origin_wor, vec3 ray_direction_wor, vec3 sphere_centre_wor, float sphere_radius, float* intersection_distance) {

	//Calcoliamo O-C
	vec3 dist_sfera = ray_origin_wor - sphere_centre_wor;
	float b = dot(dist_sfera, ray_direction_wor);
	float f = dot(dist_sfera, dist_sfera) - sphere_radius * sphere_radius;

	float delta = b * b - f;

	if (delta < 0)
		return false;

	if (delta > 0.0f) {
		//calcola le due intersezioni
		float t_a = -b + sqrt(delta);
		float t_b = -b - sqrt(delta);
		*intersection_distance = t_b;

		if (t_a < 0.0) {
			if (t_b < 0)
				return false;
		}

		return true;
	}

	if (delta == 0) {
		float t = -b + sqrt(delta);
		if (t < 0)
			return false;
		*intersection_distance = t;
		return true;
	}

	return false;
}

void mouse(int button, int state, int x, int y) {
	float xmouse = x;
	float ymouse = y;
	vec3 ray_wor = get_ray_from_mouse(xmouse, ymouse);

	selected_obj = -1;
	float closest_intersection = 0.0f;
	for (int i = 0; i < Scena.size(); i++)
	{
		float t_dist = 0.0f;

		if (ray_sphere(ViewSetup.position, ray_wor, centri[i], raggi[i], &t_dist))
		{
			if (selected_obj == -1 || t_dist < closest_intersection)
			{
				selected_obj = i;
				closest_intersection = t_dist;
			}
		}
	}

	printf("Oggetto selezionato %d \n", selected_obj);
	/*
	glutSetWindow(idfi);
	glutPostRedisplay();
	*/
	glutSetWindow(idPrincipale);
	glutPostRedisplay();

}

void my_passive_mouse(int xpos, int ypos)
{
	float alfa = 0.16; //serve ridimensionare l'offset tra due posizioni successive del mosue
	ypos = height - ypos;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	} float xoffset = xpos - lastX;
	float yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos; xoffset *= alfa;
	yoffset *= alfa;
	yaw_ += xoffset; //aggiorno l'angolo yaw
	pitch_ += yoffset; // aggiorno l'angolo Pitch // Facciamo si' che l'angolo di Picht vari tra -360 e 360.
	if (pitch_ > 359.0f)
		pitch_ = 359.0f;
	if (pitch_ < -359.0f)
		pitch_ = -359.0f; glm::vec3 front;
	front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front.y = sin(glm::radians(pitch_));
	front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	// Movimento della camera in base al movimento del mouse
	if (vistaCamera == 1) {
		ViewSetup.direction = vec4(normalize(front), 0.0);
		ViewSetup.target = ViewSetup.position + ViewSetup.direction;
	}
	else if (vistaCamera == 2) {
		ViewSetup.direction = vec4(normalize(front), 0.0);
		ViewSetup.target = ViewSetup.position + ViewSetup.direction;
	}
	glutSetWindow(idPrincipale);
	glutPostRedisplay();
}

void crescitaGiorno() {
	if (Scena[3].posy >= nord && Scena[3].posx >= 0.0) {
		mattina = false;
		pomeriggio = true;
	}
	else {
		Scena[3].posx += fattoreVelocita;
		Scena[3].posy += fattoreVelocita;
	}
}
void decrescitaGiorno() {
	if (Scena[3].posx >= ovest && Scena[3].posy <= 0.0) {
		pomeriggio = false;
		sera = true;
	}
	else {
		Scena[3].posx += fattoreVelocita;
		Scena[3].posy -= fattoreVelocita;
	}
}
void decrescitaNotte() {
	if (Scena[3].posy <= sud && Scena[3].posx <= 0.0) {
		sera = false;
		notte = true;
	}
	else {
		Scena[3].posx -= fattoreVelocita;
		Scena[3].posy -= fattoreVelocita;
	}
}
void crescitaNotte() {
	if (Scena[3].posx <= est && Scena[3].posy >= 0.0) {
		notte = false;
		mattina = true;
	}
	else {
		Scena[3].posx -= fattoreVelocita;
		Scena[3].posy += fattoreVelocita;
	}
}

void update(int a)
{
	if (vistaCamera == 2) {
		//Aggiorno il personaggio
		for (int i = 0; i < Personaggio.size(); i++) {
			Personaggio[i].posx = ViewSetup.position.x;
			Personaggio[i].posz = ViewSetup.position.z + 1;
		}
	} if (mattina) {
		crescitaGiorno();
	}
	else if (pomeriggio) {
		decrescitaGiorno();
	}
	else if (sera) {
		decrescitaNotte();
	}
	else if (notte) {
		crescitaNotte();
	} angoloUp += 0.05;
	if (angoloUp >= 0 && angoloUp < 25.0)
		angoloUp += 0.05;
	if (angoloUp > 25)
		angoloUp = 0; glutTimerFunc(10, update, 0);
	/*
	glutSetWindow(idfi);
	glutPostRedisplay();
	*/
	glutSetWindow(idPrincipale);
	glutPostRedisplay();
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
	idPrincipale = glutCreateWindow("SquareCity");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutMouseFunc(mouse);
	glutTimerFunc(10, update, 0);
	glutPassiveMotionFunc(my_passive_mouse);

	glutKeyboardFunc(keyboardPressedEvent);
	glutTimerFunc(66, update, 0);

	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	INIT_Illuminazione();
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
	loc_time = glGetUniformLocation(programId, "time");
	loc_view_pos = glGetUniformLocation(programId, "ViewPos");


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

	lscelta = glGetUniformLocation(programId, "sceltaVs");

	for (int i = 0; i < 5; i++) {
		string positionStr = "lights[" + to_string(i) + "].position";
		light_unif[i].light_position_pointer = glGetUniformLocation(programId, positionStr.c_str());
		string colorStr = "lights[" + to_string(i) + "].color";
		light_unif[i].light_color_pointer = glGetUniformLocation(programId, colorStr.c_str());
		string powerStr = "lights[" + to_string(i) + "].power";
		light_unif[i].light_power_pointer = glGetUniformLocation(programId, powerStr.c_str());
		light_unif[i].material_ambient = glGetUniformLocation(programId, "material.ambient");
		light_unif[i].material_diffuse = glGetUniformLocation(programId, "material.diffuse");
		light_unif[i].material_specular = glGetUniformLocation(programId, "material.specular");
		light_unif[i].material_shininess = glGetUniformLocation(programId, "material.shininess");
	}
	loc_res = glGetUniformLocation(programId_Sfondo, "resolution");

	loc_mouse = glGetUniformLocation(programId_Sfondo, "mouse");
	glutMainLoop();
}