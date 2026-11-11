#pragma once
#pragma once
// Formas basicas PROPIAS (esfera, cilindro, cono, plano) y funciones para dibujar con tu shader.
// Con esto Peanuts.cpp y el avatar ya no dependen de nada del equipo.
#include <vector>
#include <glew.h>
#include <glm.hpp>
#include "Mesh_tn.h"
#include "Texture.h"
#include "Model.h"
#include "Escena.h"

//mallas basicas (se crean con CrearFormas(), ya con la ventana abierta)
//esfera: radio 1 | cilindro: radio 1, alto 1, centrado | cono: radio 1, alto 1, centrado, punta hacia +Y
extern MeshModel* mEsfera;
extern MeshModel* mEsferaBaja;		//esfera con pocos triangulos (para cosas chiquitas)
extern MeshModel* mCilindro;
extern MeshModel* mCono;
extern MeshModel* mPlano;			//cuadrado de 1x1 en XZ (altura 0), centrado

enum { matOpaco, matBrillante, matEmisivo };

void CrearFormas();

//crear mallas propias: vertices con x y z u v nx ny nz  (las normales se mandan "hacia afuera"; AgregarVertice ya las acomoda para tu shader)
void AgregarVertice(std::vector<GLfloat>& v, glm::vec3 pos, float u, float vv, glm::vec3 normal);
MeshModel* GuardarMalla(std::vector<GLfloat>& v, std::vector<unsigned int>& idx);
Texture* CargarTextura(const char* archivo);

//dibujar
void FijarContexto(const ContextoEscena& c);			//cada planeta lo llama al empezar
const ContextoEscena& ContextoActual();
void UsarMaterial(int tipo);							// matOpaco, matBrillante o matEmisivo
void Dibujar(MeshModel* malla, const glm::mat4& model, glm::vec3 color, float emision = 0.0f);
void DibujarObj(Model& modelo, const glm::mat4& model, float emision);	// un .obj con la matriz dada
Texture* TexturaBlanca();								// para pintar solo con color