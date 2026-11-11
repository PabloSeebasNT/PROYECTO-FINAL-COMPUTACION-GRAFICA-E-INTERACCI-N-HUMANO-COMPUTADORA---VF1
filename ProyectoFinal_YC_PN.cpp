/*
Animación:
Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
Compleja: Por medio de funciones y algoritmos.
Textura Animada
APUNTES PROFE
ANIMACION BASICA
- MODIFICAR  MAS DE 1 TRANSFORMACION GEOMETRICA

- CONTEXTO DE LA ANIMACION

// LA JERARQUIA DEL ROVER
SOLO MODIFICAR EL TRANSLATE Y EL ROTATE


TECLAS: P cambia de vista | N lamparas de Peanuts | H dia/noche | C despega el cohete (vista universo)
*/
//para cargar imagen


#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//cada planeta vive en su propio archivo
#include "Escena.h"
#include "Peanuts.h"
#include "Formas.h"
#include "Avatar.h"
#include "Hilex.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
bool avanza;

float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;
float movCoche = 0.0f;
float movOffset = 0.01f;


//VISTAS: con la tecla P se cambia entre el universo completo, el planeta Peanuts y el planeta Hilex
enum { VISTA_UNIVERSO, VISTA_PEANUTS, VISTA_HILEX };
int vista = VISTA_UNIVERSO;
bool deNoche = false;			//tecla H: dia / noche

Window mainWindow;

//detecta cuando se PRESIONA una tecla (true solo en el cuadro en que se presiona, aunque la mantengas)
static bool teclaPrevia[1024];
static bool TeclaNueva(int tecla) { return mainWindow.getsKeys()[tecla] && !teclaPrevia[tecla]; }
std::vector<MeshModel*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;
Texture pistaTexture;
Texture muroTexture;			//muros de la pista

//piso rectangular de la estacion espacial (en unidades). La imagen se repite 10 veces a lo largo de cada lado
const float PISO_ANCHO = 160.0f;		//eje X
const float PISO_LARGO = 120.0f;		//eje Z

//base de lanzamiento y cohete (tecla C)
const glm::vec3 BASE_COHETE(45.0f, -2.0f, -10.0f);	//centro de la base, sobre el piso (y = -2)
const float ESCALA_COHETE = 20.0f;					//tamano del modelo
const float ALTURA_BASE_COHETE = 2.5f;				//cuanto sube el origen del modelo para quedar parado sobre la base
const float LARGO_COLA = 3.0f;						//distancia del origen del modelo a la salida del fuego
bool coheteVolando = false;
float coheteT = 0.0f;								//segundos desde que se presiono C

Model Rover_M;
Model nave_carrera;

Model satelite;
Model Cohete_M;

//avio
Model Blackhawk_M;
//ELEMENTOS DEL ESPACIO (los modelos de cada planeta se cargan en Peanuts.cpp y Hilex.cpp)
Model lunaSnoopy;
Model estrellaWoodstock;
Skybox skybox;
Skybox skyboxPlaneta;//sky nox planeta
Skybox skyboxPeanuts;			//cielo del planeta Peanuts

//materiales
Material Material_brillante;
Material Material_opaco;
Material Material_emisivo; //


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";
// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	MeshModel* obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	MeshModel* obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	MeshModel* obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	MeshModel* obj5 = new MeshModel();
	obj5->CreateMeshModel(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	MeshModel* obj6 = new MeshModel();
	obj6->CreateMeshModel(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los números

	MeshModel* obj7 = new MeshModel();
	obj7->CreateMeshModel(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{

	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 25.0f, 70.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso_estacion.png");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();
	pistaTexture = Texture("Textures/pista_colorida.png");
	pistaTexture.LoadTextureA();
	muroTexture = Texture("Textures/muro_colorido.png");
	muroTexture.LoadTextureA();



	nave_carrera = Model();
	nave_carrera.LoadModel("Models/nave_carrera.obj");

	satelite = Model();
	satelite.LoadModel("Models/satelite.obj");

	Cohete_M = Model();
	Cohete_M.LoadModel("Models/cohete.obj");		//c

	//carga modelos snoopy
	lunaSnoopy = Model();
	lunaSnoopy.LoadModel("Models/lunaSnoopy.obj");

	estrellaWoodstock = Model();
	estrellaWoodstock.LoadModel("Models/estrellaWoodstock.obj");
	//modelos de adentro de cada planeta (estan en Peanuts.cpp y Hilex.cpp)
	CrearFormas();		//esfera, cilindro, cono y plano propios (ya con la ventana abierta)
	CargarPeanuts();
	CargarHilex();
	//avion 


	//skybox

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/bkg1_right.png");
	skyboxFaces.push_back("Textures/bkg1_left.png");
	skyboxFaces.push_back("Textures/bkg1_bot.png");
	skyboxFaces.push_back("Textures/bkg1_top.png");
	skyboxFaces.push_back("Textures/bkg1_front.png");
	skyboxFaces.push_back("Textures/bkg1_back.png");

	skybox = Skybox(skyboxFaces);

	//skybox del planeta Peanuts (mismo orden que el del espacio: derecha, izquierda, abajo, arriba, atras, frente)
	std::vector<std::string> skyboxPeanutsFaces;
	skyboxPeanutsFaces.push_back("Textures/Skybox/peanuts_px.png");		// derecha  (+X)
	skyboxPeanutsFaces.push_back("Textures/Skybox/peanuts_nx.png");		// izquierda(-X)
	skyboxPeanutsFaces.push_back("Textures/Skybox/peanuts_ny.png");		// abajo    (-Y)
	skyboxPeanutsFaces.push_back("Textures/Skybox/peanuts_py.png");		// arriba   (+Y)
	skyboxPeanutsFaces.push_back("Textures/Skybox/peanuts_pz.png");		// atras    (+Z)
	skyboxPeanutsFaces.push_back("Textures/Skybox/peanuts_nz.png");		// frente   (-Z)
	//solo se carga si las 6 imagenes existen (si falta una, el Skybox se queda a medias y el programa truena al dibujarlo)
	bool tieneCieloPeanuts = true;
	for (size_t i = 0; i < skyboxPeanutsFaces.size(); i++)
	{
		std::ifstream archivo(skyboxPeanutsFaces[i].c_str());
		if (!archivo.good()) { printf("NO se encontro %s\n", skyboxPeanutsFaces[i].c_str()); tieneCieloPeanuts = false; }
	}
	if (tieneCieloPeanuts) skyboxPeanuts = Skybox(skyboxPeanutsFaces);
	else printf("Peanuts usara el cielo del espacio hasta que pongas las imagenes en Textures/Skybox\n");


	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);
	Material_emisivo = Material(10.0f, 512); // Intensidad muy alta y exponente cerrado

	//--------------------------------------------------
	// 
	// LUCES
	// --------------------------------------
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	//luz dentro del planeta (mismo centro que centroPlaneta, un poco arriba) para que se vea el interior
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		0.4f, 1.0f,
		-40.0f, 53.0f, -30.0f,
		1.0f, 0.05f, 0.01f);
	pointLightCount++;


	// luna
	pointLights[2] = PointLight(0.8f, 0.85f, 1.0f,
		0.5f, 3.0f,                     // Intensidad ambiental y difusa
		30.0f, 50.0f, 0.0f,
		1.0f, 0.014f, 0.0007f);         // Atenuación suave (constante, lineal, cuadrática)
	pointLightCount++;

	// estrella 
	float brilloEstrella = 4.0f + 1.5f * sin(angulovaria * 3.0f);
	pointLights[3] = PointLight(1.0f, 0.9f, 0.4f,
		0.8f, brilloEstrella,
		90.0f, 50.0f, 0.0f,
		1.0f, 0.014f, 0.0007f);         // Atenuación suave para que ilumine todo el entorno
	pointLightCount++;

	PointLight pointLightsPlaneta[MAX_POINT_LIGHTS];   // luces de adentro de Peanuts o de Hilex

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//avion ejex
	spotLights[2] = SpotLight(
		0.0f, 0.0f, 1.0f,  // color  
		0.0f,               // sin ambiente
		8.0f,               // intensidad difusa
		0.0f, 5.0f, 6.0f, // posición inicial 
		0.0f, 0.0f, -1.0f, // dirección inicial 
		1.0f, 0.0f, 0.0f, // atenua 
		20.0f               // 
	);
	spotLightCount++; // 4

	//avion ejey
	spotLights[3] = SpotLight(
		0.5f, 0.0f, 0.8f,  // color  
		0.0f,               // sin ambiente
		8.0f,               // intensidad difusa
		0.0f, 5.0f, 6.0f, // pos
		0.0f, 0.0f, -1.0f, // dir
		1.0f, 0.0f, 0.0f, // atenuac
		20.0f               // 
	);
	spotLightCount++; // 

	//-------------------------------------------------------------------
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	//---------------------------------
	//CIRCUITO DE LAS NAVES (rectas, curvas y una rampa)
	std::vector<glm::vec3> puntosPista;            // todos los puntos por donde pasan las naves
	float pasoPista = 0.25f;                       // distancia entre un punto y el siguiente
	glm::vec3 origenPista(-60.0f, 1.0f, -32.0f);   // donde esta la pista en el mundo (sobre el piso, a la izquierda de la base del cohete)
	float anchoPista = 9.0f;                       // ancho de la pista
	float velocidadNave = 0.3f;                    // velocidad base de las naves
	float alturaNave = 0.6f;                       // que tan arriba del asfalto flotan las naves
	float giroModelo = 90.0f;                      // para que la nave vaya de frente (prueba 90, -90, 180)

	//La pista es una lista de tramos y se recorre 2 veces: ida y regreso, asi cierra el circuito.
	//Para que cierre, cada vuelta debe girar 180 grados en total y subir lo mismo que baja.
	// tipoTramo:   0 = recta, 1 = curva
	// medidaTramo: largo si es recta, radio si es curva
	// gradosTramo: solo curvas (+ derecha, - izquierda)
	// subidaTramo: cuanto sube (+) o baja (-) en ese tramo
	//                      recta  curva  recta  curva  curva  recta  curva
	const int NUM_TRAMOS = 7;
	int tipoTramo[NUM_TRAMOS] = { 0,     1,     0,     1,     1,     0,     1 };
	float medidaTramo[NUM_TRAMOS] = { 18.0f, 12.0f, 14.0f, 9.0f,  9.0f,  6.0f,  12.0f };
	float gradosTramo[NUM_TRAMOS] = { 0.0f,  90.0f, 0.0f,  -60.0f, 60.0f, 0.0f,  90.0f };
	float subidaTramo[NUM_TRAMOS] = { 4.0f,  0.0f,  -4.0f, 0.0f,  0.0f,  0.0f,  0.0f };

	//se arma la lista de puntos con una "tortuga" que camina por la pista
	float pistaX = 0.0f, pistaY = 0.0f, pistaZ = 0.0f; // donde va la tortuga
	float rumbo = 0.0f;                                // hacia donde mira (0 = hacia +X)
	for (int vuelta = 0; vuelta < 2; vuelta++)
	{
		for (int tramo = 0; tramo < NUM_TRAMOS; tramo++)
		{
			int pasos;             // cuantos puntos tiene el tramo
			float giroPaso = 0.0f; // cuanto gira en cada paso
			float cuerda;          // cuanto avanza en cada paso
			if (tipoTramo[tramo] == 0) // recta
			{
				pasos = (int)(medidaTramo[tramo] / pasoPista + 0.5f);
				cuerda = medidaTramo[tramo] / pasos;
			}
			else // curva
			{
				float giroTotal = gradosTramo[tramo] * toRadians;
				pasos = (int)(medidaTramo[tramo] * fabs(giroTotal) / pasoPista + 0.5f);
				giroPaso = giroTotal / pasos;
				cuerda = 2.0f * medidaTramo[tramo] * sin(fabs(giroPaso) / 2.0f);
			}
			float yInicial = pistaY;
			for (int k = 0; k < pasos; k++)
			{
				float t = (float)k / pasos;
				float suave = t * t * (3.0f - 2.0f * t); // para que la rampa empiece y termine suave
				puntosPista.push_back(glm::vec3(pistaX, yInicial + subidaTramo[tramo] * suave, pistaZ));
				pistaX += cos(rumbo + giroPaso / 2.0f) * cuerda;
				pistaZ += sin(rumbo + giroPaso / 2.0f) * cuerda;
				rumbo += giroPaso;
			}
			pistaY = yInicial + subidaTramo[tramo];
		}
	}
	int totalPuntos = puntosPista.size();

	//MALLA DE LA PISTA: el asfalto y los muros que la sostienen
	float sueloY = -2.0f - origenPista.y + 0.01f; // el piso del mundo esta en y = -2
	float largoPista = totalPuntos * pasoPista;
	float largoTextura = largoPista / floor(largoPista / 10.0f + 0.5f); // la textura se repite cada ~10 unidades
	std::vector<GLfloat> verticesPista;
	std::vector<unsigned int> indicesPista;
	std::vector<GLfloat> verticesMuros;
	std::vector<unsigned int> indicesMuros;
	for (int i = 0; i <= totalPuntos; i++) // hasta totalPuntos para cerrar la vuelta
	{
		glm::vec3 punto = puntosPista[i % totalPuntos];
		glm::vec3 direccion = puntosPista[(i + 1) % totalPuntos] - puntosPista[(i - 1 + totalPuntos) % totalPuntos];
		glm::vec3 derecha = glm::normalize(glm::vec3(-direccion.z, 0.0f, direccion.x)); // lado derecho de la pista
		glm::vec3 bordeIzq = punto - derecha * (anchoPista / 2.0f);
		glm::vec3 bordeDer = punto + derecha * (anchoPista / 2.0f);
		float v = i * pasoPista / largoTextura;
		float u = i * pasoPista / 4.0f;

		//asfalto: un vertice en cada borde (la normal va hacia abajo, igual que tu piso)
		GLfloat asfalto[] = {
			bordeIzq.x, bordeIzq.y, bordeIzq.z,   0.0f, v,   0.0f, -1.0f, 0.0f,
			bordeDer.x, bordeDer.y, bordeDer.z,   1.0f, v,   0.0f, -1.0f, 0.0f };
		verticesPista.insert(verticesPista.end(), asfalto, asfalto + 16);

		//muros: izquierdo (arriba y abajo) y derecho (arriba y abajo)
		GLfloat muros[] = {
			bordeIzq.x, bordeIzq.y, bordeIzq.z,   u, (bordeIzq.y - sueloY) / 4.0f,   derecha.x, 0.0f, derecha.z,
			bordeIzq.x, sueloY,     bordeIzq.z,   u, 0.0f,                           derecha.x, 0.0f, derecha.z,
			bordeDer.x, bordeDer.y, bordeDer.z,   u, (bordeDer.y - sueloY) / 4.0f,   -derecha.x, 0.0f, -derecha.z,
			bordeDer.x, sueloY,     bordeDer.z,   u, 0.0f,                           -derecha.x, 0.0f, -derecha.z };
		verticesMuros.insert(verticesMuros.end(), muros, muros + 32);
	}
	for (int i = 0; i < totalPuntos; i++)
	{
		//cada par de puntos con el siguiente forma 2 triangulos
		unsigned int a = 2 * i, b = 2 * i + 1, c = 2 * i + 2, d = 2 * i + 3;
		unsigned int triAsfalto[] = { a, b, c, b, d, c };
		indicesPista.insert(indicesPista.end(), triAsfalto, triAsfalto + 6);

		unsigned int w = 4 * i;
		unsigned int triMuros[] = { w, w + 1, w + 4, w + 1, w + 5, w + 4,       // muro izquierdo
									w + 2, w + 6, w + 3, w + 3, w + 6, w + 7 }; // muro derecho
		indicesMuros.insert(indicesMuros.end(), triMuros, triMuros + 12);
	}
	MeshModel* pistaMesh = new MeshModel();
	pistaMesh->CreateMeshModel(verticesPista.data(), indicesPista.data(), (unsigned int)verticesPista.size(), (unsigned int)indicesPista.size());
	MeshModel* murosMesh = new MeshModel();
	murosMesh->CreateMeshModel(verticesMuros.data(), indicesMuros.data(), (unsigned int)verticesMuros.size(), (unsigned int)indicesMuros.size());

	//TRES NAVES en la pista: cada una empieza en un punto distinto, va a su ritmo y en su carril
	float distanciaNaves[3] = { 0.0f, largoPista / 3.0f, 2.0f * largoPista / 3.0f };
	float velNaves[3] = { 1.0f, 0.85f, 1.15f };								//multiplicador de la velocidad base
	float carrilNaves[3] = { 0.0f, -2.5f, 2.5f };								//carril: centro, izquierda, derecha
	glm::vec3 tinteNaves[3] = { glm::vec3(1.0f), glm::vec3(1.0f, 0.55f, 0.55f), glm::vec3(0.55f, 0.8f, 1.0f) };

	//variables que se usan en cada ciclo para mover las naves
	int actual = 0;                          // punto de la pista donde esta la nave
	int siguiente = 0;                       // el punto que sigue
	float fraccion = 0.0f;                   // que tan cerca esta del siguiente punto (0 a 1)
	glm::vec3 posNave(0.0f, 0.0f, 0.0f);     // posicion de la nave
	glm::vec3 adelante(1.0f, 0.0f, 0.0f);    // hacia donde avanza
	float giroNave = 0.0f;                   // giro para mirar hacia donde avanza
	float inclinacionNave = 0.0f;            // sube o baja la nariz en las rampas
	//TRAYECTORIA DEL SATELITE (un circulo a una altura fija)
	glm::vec3 centroOrbita(0.0f, 6.0f, -20.0f);  // centro del circulo (la altura en Y es la del vuelo)
	float radioOrbita = 8.0f;                     // radio del circulo (entre mas chico, mas corta la trayectoria)
	float anguloSatelite = 0.0f;                  // en que parte de la orbita va (solo aumenta)
	float velocidadSatelite = 0.01f;              // velocidad en la orbita
	float giroPropioSatelite = 0.0f;              // giro del satelite sobre si mismo
	glm::vec3 posSatelite(0.0f, 0.0f, 0.0f);      // posicion del satelite




	//---------------------------------
	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	//planeta
	Sphere planeta = Sphere(10.0f, 40, 40);   // global: radio 10, 40 cortes
	Texture planetaTexture;                   // global, junto a las otras texturas

	// en el main, antes del while 
	planetaTexture = Texture("Textures/planeta3.jpg");
	planetaTexture.LoadTextureA();
	planeta.init();
	planeta.load();


	// antes del while
	glm::vec3 centroPlaneta(-40.0f, 50.0f, -30.0f);     // planeta Snoopy visto desde el espacio
	glm::vec3 centroHilex(40.0f, 50.0f, -30.0f);        // planeta Hilex visto desde el espacio
	float giroPlaneta = 0.0f;
	//luces
	glm::vec3 posLuna(30.0f, 50.0f, 0.0f);
	glm::vec3 posEstrella(90.0f, 50.0f, 0.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		float dtSeg = (float)(now - lastTime);		//segundos reales (para el avatar)
		if (dtSeg > 0.1f) dtSeg = 0.1f;
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f * deltaTime;


		//Recibir eventos del usuario
		glfwPollEvents();
		float tiempo = (float)glfwGetTime();		//segundos (para las animaciones de los planetas)
		GLfloat xCambio = mainWindow.getXChange();
		GLfloat yCambio = mainWindow.getYChange();

		//tecla P: cambia de vista (universo -> Peanuts -> Hilex -> universo) y el avatar viaja con el cambio
		if (TeclaNueva(GLFW_KEY_P))
		{
			vista = (vista + 1) % 3;
			glm::vec3 arriba(0.0f, 1.0f, 0.0f);
			if (vista == VISTA_UNIVERSO) { camera = Camera(glm::vec3(0.0f, 25.0f, 70.0f), arriba, -90.0f, -20.0f, 0.5f, 0.5f); printf("Vista: UNIVERSO\n"); }
			if (vista == VISTA_PEANUTS) { ColocarAvatar(CENTRO_PEANUTS + INICIO_AVATAR_PEANUTS, 180.0f); printf("Avatar en: planeta PEANUTS\n"); }
			if (vista == VISTA_HILEX) { ColocarAvatar(CENTRO_HILEX + INICIO_AVATAR_HILEX, 180.0f); printf("Avatar en: planeta HILEX\n"); }
		}
		if (TeclaNueva(GLFW_KEY_N)) AlternarLamparasPeanuts();		//lamparas de Peanuts
		if (TeclaNueva(GLFW_KEY_H))									//dia / noche
		{
			deNoche = !deNoche;
			if (deNoche) mainLight = DirectionalLight(0.45f, 0.5f, 0.8f, 0.12f, 0.12f, 0.0f, 0.0f, -1.0f);
			else mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 0.3f, 0.3f, 0.0f, 0.0f, -1.0f);
		}

		//tecla C: el cohete despega de la base (solo se ve en la vista del universo)
		if (coheteVolando) coheteT += dtSeg;
		if (TeclaNueva(GLFW_KEY_C) && !coheteVolando) { coheteVolando = true; coheteT = 0.0f; }
		if (coheteT > 12.5f) coheteVolando = false;				//termino el vuelo: el cohete vuelve a la base

		//universo: camara libre (como siempre) | planetas: se mueve el avatar y la camara lo sigue
		glm::mat4 vistaMatriz;
		glm::vec3 posOjo, dirOjo;
		if (vista == VISTA_UNIVERSO)
		{
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(xCambio, yCambio);
			vistaMatriz = camera.calculateViewMatrix();
			posOjo = camera.getCameraPosition();
			dirOjo = camera.getCameraDirection();
		}
		else
		{
			if (vista == VISTA_PEANUTS)
			{
				ActualizarAvatar(mainWindow.getsKeys(), xCambio, dtSeg, CENTRO_PEANUTS, RADIO_CAMINABLE_PEANUTS);
				avatar.posicion = ResolverColisionPeanuts(avatar.posicion, 1.2f);		//no atraviesa casas, arboles, lamparas...
			}
			else
			{
				ActualizarAvatar(mainWindow.getsKeys(), xCambio, dtSeg, CENTRO_HILEX, RADIO_CAMINABLE_HILEX);
				avatar.posicion = ResolverColisionHilex(avatar.posicion, 1.2f);
			}
			glm::vec3 objetivo;
			CamaraAvatar(dtSeg, posOjo, objetivo);
			vistaMatriz = glm::lookAt(posOjo, objetivo, glm::vec3(0.0f, 1.0f, 0.0f));
			dirOjo = glm::normalize(objetivo - posOjo);
		}

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float distanciaAlPlaneta = glm::distance(posOjo, centroPlaneta);
		float radioPlaneta = 10.0f; // El radio que definiste en tu esfera

		//if (distanciaAlPlaneta < radioPlaneta) {
		//// Si la cámara está dentro del planeta, dibuja el skybox interior
		//	skyboxPlaneta.DrawSkybox(camera.calculateViewMatrix(), projection);
		//}
		//else {
		//// Si está afuera, dibuja el skybox normal del espacio
		//skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		//}


		if (vista == VISTA_PEANUTS && tieneCieloPeanuts) skyboxPeanuts.DrawSkybox(vistaMatriz, projection);		//cielo de Peanuts
		else skybox.DrawSkybox(vistaMatriz, projection);									//cielo del espacio
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(vistaMatriz));
		glUniform3f(uniformEyePosition, posOjo.x, posOjo.y, posOjo.z);

		// luz ligada a la cámara de tipo flash
		lowerLight = posOjo;
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, dirOjo);

		////////
		glm::vec3 camPos = posOjo;
		glm::vec3 luzLuna = posLuna + glm::normalize(camPos - posLuna) * 6.0f;
		glm::vec3 luzEstrella = posEstrella + glm::normalize(camPos - posEstrella) * 6.0f;

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		//luces: las del universo, o las del planeta en el que estas
		PointLight* lucesActivas = pointLights;
		unsigned int numLucesActivas = pointLightCount;
		if (vista == VISTA_PEANUTS) { numLucesActivas = 0; LucesPeanuts(tiempo, deNoche, pointLightsPlaneta, numLucesActivas); lucesActivas = pointLightsPlaneta; }
		if (vista == VISTA_HILEX) { numLucesActivas = 0; LucesHilex(tiempo, deNoche, pointLightsPlaneta, numLucesActivas); lucesActivas = pointLightsPlaneta; }
		shaderList[0].SetPointLights(lucesActivas, numLucesActivas);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		//lo que necesita cada planeta para dibujar
		ContextoEscena ctx = { uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess, uniformTextureOffset,
							   &Material_opaco, &Material_brillante, &Material_emisivo };
		//

		//Reinicializando variables cada ciclo de reloj
		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);

		if (vista == VISTA_UNIVERSO)
		{
			//PISO RECTANGULAR DE LA ESTACION: el plano base mide 20 x 20, por eso se divide entre 20
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
			model = glm::scale(model, glm::vec3(PISO_ANCHO / 20.0f, 1.0f, PISO_LARGO / 20.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			pisoTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			meshList[2]->RenderMeshModel();


			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);


			//============pista y naves
			//PISTA DE CARRERAS
			model = glm::mat4(1.0);
			model = glm::translate(model, origenPista);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			pistaTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			pistaMesh->RenderMeshModel(); // el asfalto
			muroTexture.UseTexture();
			murosMesh->RenderMeshModel(); // los muros que sostienen la pista

			//NAVES: las tres avanzan por la pista, cada una en su carril
			for (int n = 0; n < 3; n++)
			{
				distanciaNaves[n] += velocidadNave * velNaves[n] * deltaTime; // solo suma, nunca va de reversa
				if (distanciaNaves[n] >= largoPista)
					distanciaNaves[n] -= largoPista; // termino la vuelta, empieza otra
				actual = (int)(distanciaNaves[n] / pasoPista) % totalPuntos;
				siguiente = (actual + 1) % totalPuntos;
				fraccion = distanciaNaves[n] / pasoPista - (int)(distanciaNaves[n] / pasoPista);
				posNave = glm::mix(puntosPista[actual], puntosPista[siguiente], fraccion); // entre dos puntos
				//hacia donde mira: del punto de atras al punto de adelante
				adelante = puntosPista[(actual + 2) % totalPuntos] - puntosPista[(actual - 2 + totalPuntos) % totalPuntos];
				glm::vec3 derechaNave = glm::normalize(glm::vec3(-adelante.z, 0.0f, adelante.x));
				posNave += derechaNave * carrilNaves[n];										// se corre al carril de esta nave
				giroNave = atan2(-adelante.z, adelante.x);                                       // gira hacia donde avanza
				inclinacionNave = atan2(adelante.y, sqrt(adelante.x * adelante.x + adelante.z * adelante.z)); // sube o baja la nariz

				model = glm::mat4(1.0);
				model = glm::translate(model, origenPista + posNave + glm::vec3(0.0f, alturaNave, 0.0f)); // donde esta en la pista
				model = glm::rotate(model, giroNave, glm::vec3(0.0f, 1.0f, 0.0f));                       // gira hacia donde avanza
				model = glm::rotate(model, inclinacionNave, glm::vec3(0.0f, 0.0f, 1.0f));                // sube/baja la nariz
				model = glm::rotate(model, giroModelo * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));         // corrige la orientacion del modelo
				model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				glUniform3fv(uniformColor, 1, glm::value_ptr(tinteNaves[n]));						//el color multiplica a la textura
				nave_carrera.RenderModel();
			}
			color = glm::vec3(1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));


			//satelite: vuela siguiendo su orbita
			anguloSatelite += velocidadSatelite * deltaTime; // solo suma, siempre avanza en la orbita
			if (anguloSatelite > 360.0f * toRadians)
				anguloSatelite -= 360.0f * toRadians; // completo una vuelta, empieza otra
			giroPropioSatelite += 1.0f * deltaTime;       // el satelite gira sobre si mismo (grados)
			posSatelite.x = centroOrbita.x + radioOrbita * cos(anguloSatelite);
			posSatelite.z = centroOrbita.z + radioOrbita * sin(anguloSatelite);
			posSatelite.y = centroOrbita.y; // siempre a la misma altura

			model = glm::mat4(1.0);
			model = glm::translate(model, posSatelite);                                          // donde esta en la orbita
			model = glm::rotate(model, giroPropioSatelite * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // gira sobre si mismo
			model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			satelite.RenderModel();


			//============base de lanzamiento: dos placas planas sobre el piso
			color = glm::vec3(0.3f, 0.3f, 0.35f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::translate(glm::mat4(1.0f), BASE_COHETE + glm::vec3(0.0f, 0.05f, 0.0f));
			model = glm::scale(model, glm::vec3(0.7f, 1.0f, 0.7f));		//14 x 14 unidades
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			plainTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[2]->RenderMeshModel();

			color = glm::vec3(0.9f, 0.6f, 0.15f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::translate(glm::mat4(1.0f), BASE_COHETE + glm::vec3(0.0f, 0.12f, 0.0f));
			model = glm::scale(model, glm::vec3(0.35f, 1.0f, 0.35f));	//7 x 7 unidades
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			brickTexture.UseTexture();
			meshList[2]->RenderMeshModel();

			//============cohete: 0 a 2 s tiembla (cuenta regresiva), despues sube acelerando y se inclina hacia +X
			float altura = 0.0f, deriva = 0.0f, inclina = 0.0f, temblor = 0.0f;
			if (coheteVolando)
			{
				if (coheteT < 2.0f) temblor = 0.05f * sinf(coheteT * 60.0f);
				else
				{
					float s = coheteT - 2.0f;
					altura = 0.9f * s * s;					//sube cada vez mas rapido
					deriva = 0.15f * s * s;					//se va hacia la derecha: trayectoria curva
					inclina = fminf(s * 6.0f, 35.0f);		//grados, la punta gira hacia +X
				}
			}
			glm::mat4 mCohete = glm::translate(glm::mat4(1.0f), BASE_COHETE + glm::vec3(deriva + temblor, ALTURA_BASE_COHETE + altura, 0.0f));
			mCohete = glm::rotate(mCohete, -inclina * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

			color = glm::vec3(1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::scale(mCohete, glm::vec3(ESCALA_COHETE));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			Cohete_M.RenderModel();

			//fuego: esfera naranja brillante que parpadea
			if (coheteVolando && coheteT >= 2.0f)
			{
				float f = 0.05f + 0.012f * sinf(tiempo * 40.0f);
				model = glm::translate(mCohete, glm::vec3(0.0f, -LARGO_COLA, 0.0f));
				model = glm::scale(model, glm::vec3(f, f * 3.0f, f));
				color = glm::vec3(2.5f, 1.2f, 0.2f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				planetaTexture.UseTexture();
				Material_emisivo.UseMaterial(uniformSpecularIntensity, uniformShininess);
				planeta.render();
			}
			color = glm::vec3(1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);


			//luna
			model = glm::mat4(1.0);                      // la matriz del planeta
			model = glm::translate(model, posLuna);
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
			color = glm::vec3(1.5f, 1.6f, 2.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Material_emisivo.UseMaterial(uniformSpecularIntensity, uniformShininess);
			lunaSnoopy.RenderModel();

			//estrella 
			model = glm::mat4(1.0);                      // la matriz del planeta
			model = glm::translate(model, posEstrella);
			model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
			color = glm::vec3(2.50f, 2.0f, 0.60f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Material_emisivo.UseMaterial(uniformSpecularIntensity, uniformShininess);
			estrellaWoodstock.RenderModel();



			giroPlaneta += 0.2f * deltaTime;
			model = glm::mat4(1.0);
			model = glm::translate(model, centroPlaneta);
			model = glm::rotate(model, giroPlaneta * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			modelaux = model;                       // padre: el planeta

			color = glm::vec3(1.0f, 1.0f, 1.0f);   // la estrella dejo el color en amarillo
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			model = modelaux;       // la matriz del planeta
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			planetaTexture.UseTexture();
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			planeta.render();

			//planeta HILEX visto desde el espacio (cambiale la textura/color cuando tengas la suya)
			model = glm::mat4(1.0);
			model = glm::translate(model, centroHilex);
			model = glm::rotate(model, giroPlaneta * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 0.6f, 0.4f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			planeta.render();

			glDisable(GL_BLEND);
		}
		else if (vista == VISTA_PEANUTS)
		{
			DibujarPeanuts(ctx, tiempo, deNoche);
			FijarContexto(ctx);
			DibujarAvatar(tiempo);
		}
		else
		{
			DibujarHilex(ctx, tiempo, deNoche);
			FijarContexto(ctx);
			DibujarAvatar(tiempo);
		}



		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(0);

		for (int k = 0; k < 1024; k++) teclaPrevia[k] = mainWindow.getsKeys()[k];		//para detectar la siguiente presion

		mainWindow.swapBuffers();
	}

	return 0;
}