// Planeta HILEX: aqui van sus modelos. Las posiciones son relativas a CENTRO_HILEX
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Formas.h"
#include "Hilex.h"

static Texture* texSuelo;

void CargarHilex()
{
	texSuelo = TexturaBlanca();
	//aqui cargas tus modelos de Hilex, por ejemplo:
	//modeloAlgo = Model(); modeloAlgo.LoadModel("Models/algo.obj");
}

void LucesHilex(float tiempo, bool deNoche, PointLight* puntuales, unsigned int& np)
{
	//luz arriba del planeta para que se vea
	puntuales[np++] = PointLight(1.0f, 1.0f, 1.0f, 0.4f, 1.0f,
		CENTRO_HILEX.x, CENTRO_HILEX.y + 20.0f, CENTRO_HILEX.z, 1.0f, 0.01f, 0.001f);
}

void DibujarHilex(const ContextoEscena& c, float t, bool deNoche)
{
	FijarContexto(c);
	glm::vec2 sinMov(0.0f, 0.0f);
	glUniform2fv(c.uTexOffset, 1, glm::value_ptr(sinMov));
	UsarMaterial(matOpaco);

	//piso temporal para que el avatar tenga donde caminar (cambialo por el de Hilex)
	texSuelo->UseTexture();
	Dibujar(mPlano, glm::scale(glm::translate(glm::mat4(1.0f), CENTRO_HILEX), glm::vec3(400.0f, 1.0f, 400.0f)), glm::vec3(0.9f, 0.55f, 0.35f));
}

glm::vec3 ResolverColisionHilex(glm::vec3 p, float radio)
{
	return p;		//cuando pongas objetos en Hilex, aqui los haces solidos (mira ResolverColisionPeanuts)
}