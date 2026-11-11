#include "Formas.h"
#include <cmath>
#include <gtc/type_ptr.hpp>

MeshModel* mEsfera = nullptr;
MeshModel* mEsferaBaja = nullptr;
MeshModel* mCilindro = nullptr;
MeshModel* mCono = nullptr;
MeshModel* mPlano = nullptr;

static ContextoEscena ctx;
static Texture* texBlancaPropia = nullptr;
const float PI_F = 3.14159265f;

//---------------------------------------------------------------- utilidades
void AgregarVertice(std::vector<GLfloat>& v, glm::vec3 pos, float u, float vv, glm::vec3 normal)
{
	//tu shader (igual que tu piso y los .obj) usa la normal al reves, por eso se voltea aqui
	GLfloat d[8] = { pos.x, pos.y, pos.z, u, vv, -normal.x, -normal.y, -normal.z };
	v.insert(v.end(), d, d + 8);
}

MeshModel* GuardarMalla(std::vector<GLfloat>& v, std::vector<unsigned int>& idx)
{
	MeshModel* m = new MeshModel();
	m->CreateMeshModel(v.data(), idx.data(), (unsigned int)v.size(), (unsigned int)idx.size());
	return m;
}

Texture* CargarTextura(const char* archivo)
{
	Texture* t = new Texture(archivo);
	t->LoadTextureA();
	return t;
}

Texture* TexturaBlanca()
{
	if (!texBlancaPropia) texBlancaPropia = CargarTextura("Textures/plain.png");
	return texBlancaPropia;
}

//---------------------------------------------------------------- formas
static MeshModel* CrearEsfera(int cortes, int anillos)
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> idx;
	for (int i = 0; i <= anillos; i++)
	{
		float fi = PI_F * i / anillos;						// 0 arriba, PI abajo
		for (int j = 0; j <= cortes; j++)
		{
			float th = 2.0f * PI_F * j / cortes;
			glm::vec3 p(sinf(fi) * cosf(th), cosf(fi), sinf(fi) * sinf(th));
			AgregarVertice(v, p, (float)j / cortes, (float)i / anillos, p);
		}
	}
	for (int i = 0; i < anillos; i++)
		for (int j = 0; j < cortes; j++)
		{
			unsigned int a = i * (cortes + 1) + j, b = a + cortes + 1;
			unsigned int t[] = { a, b, a + 1, a + 1, b, b + 1 };
			idx.insert(idx.end(), t, t + 6);
		}
	return GuardarMalla(v, idx);
}

static MeshModel* CrearCilindro(int cortes)
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> idx;
	//lado
	for (int j = 0; j <= cortes; j++)
	{
		float th = 2.0f * PI_F * j / cortes;
		glm::vec3 n(cosf(th), 0.0f, sinf(th));
		AgregarVertice(v, glm::vec3(n.x, 0.5f, n.z), (float)j / cortes, 1.0f, n);
		AgregarVertice(v, glm::vec3(n.x, -0.5f, n.z), (float)j / cortes, 0.0f, n);
	}
	for (int j = 0; j < cortes; j++)
	{
		unsigned int a = 2 * j;
		unsigned int t[] = { a, a + 1, a + 2, a + 2, a + 1, a + 3 };
		idx.insert(idx.end(), t, t + 6);
	}
	//tapas (arriba y abajo)
	for (int tapa = 0; tapa < 2; tapa++)
	{
		float y = tapa == 0 ? 0.5f : -0.5f;
		glm::vec3 n(0.0f, tapa == 0 ? 1.0f : -1.0f, 0.0f);
		unsigned int centro = (unsigned int)(v.size() / 8);
		AgregarVertice(v, glm::vec3(0.0f, y, 0.0f), 0.5f, 0.5f, n);
		for (int j = 0; j <= cortes; j++)
		{
			float th = 2.0f * PI_F * j / cortes;
			AgregarVertice(v, glm::vec3(cosf(th), y, sinf(th)), 0.5f + 0.5f * cosf(th), 0.5f + 0.5f * sinf(th), n);
		}
		for (int j = 0; j < cortes; j++)
		{
			unsigned int t[] = { centro, centro + 1 + j, centro + 2 + j };
			idx.insert(idx.end(), t, t + 3);
		}
	}
	return GuardarMalla(v, idx);
}

static MeshModel* CrearCono(int cortes)
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> idx;
	//lado: la normal apunta un poco hacia arriba
	for (int j = 0; j <= cortes; j++)
	{
		float th = 2.0f * PI_F * j / cortes;
		glm::vec3 n = glm::normalize(glm::vec3(cosf(th), 0.5f, sinf(th)));
		AgregarVertice(v, glm::vec3(0.0f, 0.5f, 0.0f), (float)j / cortes, 1.0f, n);
		AgregarVertice(v, glm::vec3(cosf(th), -0.5f, sinf(th)), (float)j / cortes, 0.0f, n);
	}
	for (int j = 0; j < cortes; j++)
	{
		unsigned int a = 2 * j;
		unsigned int t[] = { a, a + 1, a + 3 };
		idx.insert(idx.end(), t, t + 3);
	}
	//base
	glm::vec3 n(0.0f, -1.0f, 0.0f);
	unsigned int centro = (unsigned int)(v.size() / 8);
	AgregarVertice(v, glm::vec3(0.0f, -0.5f, 0.0f), 0.5f, 0.5f, n);
	for (int j = 0; j <= cortes; j++)
	{
		float th = 2.0f * PI_F * j / cortes;
		AgregarVertice(v, glm::vec3(cosf(th), -0.5f, sinf(th)), 0.5f + 0.5f * cosf(th), 0.5f + 0.5f * sinf(th), n);
	}
	for (int j = 0; j < cortes; j++)
	{
		unsigned int t[] = { centro, centro + 1 + j, centro + 2 + j };
		idx.insert(idx.end(), t, t + 3);
	}
	return GuardarMalla(v, idx);
}

static MeshModel* CrearPlano()
{
	std::vector<GLfloat> v;
	std::vector<unsigned int> idx = { 0, 2, 1, 1, 2, 3 };
	glm::vec3 arriba(0.0f, 1.0f, 0.0f);
	AgregarVertice(v, glm::vec3(-0.5f, 0.0f, -0.5f), 0.0f, 0.0f, arriba);
	AgregarVertice(v, glm::vec3(0.5f, 0.0f, -0.5f), 1.0f, 0.0f, arriba);
	AgregarVertice(v, glm::vec3(-0.5f, 0.0f, 0.5f), 0.0f, 1.0f, arriba);
	AgregarVertice(v, glm::vec3(0.5f, 0.0f, 0.5f), 1.0f, 1.0f, arriba);
	return GuardarMalla(v, idx);
}

void CrearFormas()
{
	mEsfera = CrearEsfera(32, 20);
	mEsferaBaja = CrearEsfera(12, 8);
	mCilindro = CrearCilindro(24);
	mCono = CrearCono(24);
	mPlano = CrearPlano();
}

//---------------------------------------------------------------- dibujar
void FijarContexto(const ContextoEscena& c) { ctx = c; }
const ContextoEscena& ContextoActual() { return ctx; }

void UsarMaterial(int tipo)
{
	Material* m = (tipo == matBrillante) ? ctx.brillante : (tipo == matEmisivo) ? ctx.emisivo : ctx.opaco;
	m->UseMaterial(ctx.uSpecular, ctx.uShininess);
}

//tu shader no tiene "emision": para que algo brille se le sube el color por encima de 1 (como la luna)
void Dibujar(MeshModel* malla, const glm::mat4& model, glm::vec3 color, float emision)
{
	glUniformMatrix4fv(ctx.uModel, 1, GL_FALSE, glm::value_ptr(model));
	glm::vec3 c = color * (1.0f + emision);
	glUniform3fv(ctx.uColor, 1, glm::value_ptr(c));
	malla->RenderMeshModel();
}

void DibujarObj(Model& modelo, const glm::mat4& model, float emision)
{
	glUniformMatrix4fv(ctx.uModel, 1, GL_FALSE, glm::value_ptr(model));
	glm::vec3 c(1.0f + emision);
	glUniform3fv(ctx.uColor, 1, glm::value_ptr(c));
	modelo.RenderModel();
}