#include <cmath>
#include <glew.h>
#include <glfw3.h>
#include <gtc/matrix_transform.hpp>
#include "Avatar.h"
#include "Formas.h"

Avatar avatar = { glm::vec3(0.0f), 180.0f, 0.0f, 0.0f };

//---------- AJUSTES (cambia estos numeros a tu gusto)
static const float ESCALA_AVATAR = 1.0f;		// tamaño del avatar
static const float VELOCIDAD = 14.0f;			// unidades por segundo
static const float SENSIBILIDAD_RATON = 0.15f;	// grados por cada pixel que mueves el raton
static const float DISTANCIA_CAMARA = 9.0f;		// que tan atras va la camara
static const float ALTURA_CAMARA = 4.5f;		// que tan arriba del suelo va la camara

static glm::vec3 ojoSuave(0.0f);
static bool reiniciarCamara = true;

//hacia donde mira el avatar (en el piso)
static glm::vec3 Frente()
{
	float r = glm::radians(avatar.rumbo);
	return glm::vec3(sinf(r), 0.0f, cosf(r));
}

void ColocarAvatar(glm::vec3 posicion, float rumbo)
{
	avatar.posicion = posicion;
	avatar.rumbo = rumbo;
	avatar.faseCaminata = 0.0f;
	avatar.intensidadCaminata = 0.0f;
	reiniciarCamara = true;			// la camara salta con el avatar, no cruza el espacio
}

void ActualizarAvatar(bool* teclas, float xCambio, float dt, glm::vec3 centro, float radioCaminable)
{
	//el raton gira al avatar (mover a la derecha = girar a la derecha)
	avatar.rumbo -= xCambio * SENSIBILIDAD_RATON;

	glm::vec3 f = Frente();
	glm::vec3 derecha(-f.z, 0.0f, f.x);
	glm::vec3 mov(0.0f);
	if (teclas[GLFW_KEY_W]) mov += f;
	if (teclas[GLFW_KEY_S]) mov -= f;
	if (teclas[GLFW_KEY_D]) mov += derecha;
	if (teclas[GLFW_KEY_A]) mov -= derecha;

	bool camina = glm::length(mov) > 0.001f;
	if (camina)
	{
		mov = glm::normalize(mov);
		avatar.posicion += mov * VELOCIDAD * dt;
		avatar.faseCaminata += dt * 9.0f;
	}
	//la intensidad sube y baja suave para que empiece y termine de caminar sin brincos
	float meta = camina ? 1.0f : 0.0f;
	avatar.intensidadCaminata += (meta - avatar.intensidadCaminata) * (1.0f - expf(-dt * 10.0f));

	//no salir del circulo del planeta
	glm::vec2 d(avatar.posicion.x - centro.x, avatar.posicion.z - centro.z);
	float dist = glm::length(d);
	if (dist > radioCaminable)
	{
		d *= radioCaminable / dist;
		avatar.posicion.x = centro.x + d.x;
		avatar.posicion.z = centro.z + d.y;
	}
}

void CamaraAvatar(float dt, glm::vec3& ojo, glm::vec3& objetivo)
{
	glm::vec3 f = Frente();
	glm::vec3 deseado = avatar.posicion - f * DISTANCIA_CAMARA;
	if (reiniciarCamara) { ojoSuave = deseado; reiniciarCamara = false; }
	else ojoSuave = glm::mix(ojoSuave, deseado, 1.0f - expf(-dt * 6.0f));
	ojo = glm::vec3(ojoSuave.x, avatar.posicion.y + ALTURA_CAMARA, ojoSuave.z);
	objetivo = avatar.posicion + glm::vec3(0.0f, 2.0f, 0.0f) + f * 4.0f;
}

//------------------------------------------------------------------ dibujo
// Jerarquia: cuerpo (raiz, la cadera) -> torso -> bufanda (y su cola)
//                                              -> cabeza -> ojos, nariz, gorro (y pompon)
//                                              -> brazos -> manoplas
//                                -> piernas -> botas
void DibujarAvatar(float t)
{
	UsarMaterial(matOpaco);
	TexturaBlanca()->UseTexture();		// el color lo pone Dibujar

	glm::mat4 base = glm::translate(glm::mat4(1.0f), avatar.posicion);
	base = glm::rotate(base, glm::radians(avatar.rumbo), glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::scale(base, glm::vec3(ESCALA_AVATAR));

	float inten = avatar.intensidadCaminata;
	float ciclo = avatar.faseCaminata;
	float respira = t * 2.0f;											// movimiento leve cuando esta quieto
	float ang = glm::radians(35.0f * inten * sinf(ciclo));				// cuanto gira la pierna 0 (la 1 al reves)

	//la cadera sube o baja para que el pie mas bajo SIEMPRE toque la nieve
	float bajo0 = 0.92f * cosf(ang) + 0.07f * sinf(ang);
	float bajo1 = 0.92f * cosf(ang) - 0.07f * sinf(ang);
	float altura = 0.12f + fmaxf(bajo0, bajo1);

	glm::vec3 chamarra(0.85f, 0.15f, 0.15f), piel(1.0f, 0.82f, 0.68f), oscuro(0.05f);
	glm::vec3 gorro(0.15f, 0.55f, 0.4f), bufanda(0.2f, 0.5f, 0.9f), pantalon(0.2f, 0.25f, 0.5f), bota(0.35f, 0.2f, 0.1f);

	// 1. CUERPO (raiz = cadera)
	glm::mat4 cuerpo = glm::translate(base, glm::vec3(0.0f, altura, 0.0f));
	cuerpo = glm::rotate(cuerpo, glm::radians(3.0f * inten * sinf(ciclo)), glm::vec3(0.0f, 1.0f, 0.0f));	// los hombros se balancean un poco

	// 2. TORSO (hijo del cuerpo): chamarra
	glm::mat4 torso = glm::translate(cuerpo, glm::vec3(0.0f, 0.65f, 0.0f));
	Dibujar(mEsfera, glm::scale(torso, glm::vec3(0.45f, 0.62f, 0.36f)), chamarra);

	// bufanda (hija del torso) y su cola, que se mece
	glm::mat4 cuello = glm::translate(torso, glm::vec3(0.0f, 0.6f, 0.0f));
	Dibujar(mCilindro, glm::scale(cuello, glm::vec3(0.34f, 0.16f, 0.3f)), bufanda);
	glm::mat4 cola = glm::translate(cuello, glm::vec3(0.18f, 0.0f, -0.28f));
	cola = glm::rotate(cola, glm::radians(12.0f + 14.0f * inten * sinf(ciclo + 1.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
	Dibujar(mCilindro, glm::scale(glm::translate(cola, glm::vec3(0.0f, -0.3f, 0.0f)), glm::vec3(0.09f, 0.6f, 0.04f)), bufanda);

	// 3. CABEZA (hija del torso): gira un poquito y asiente al caminar
	glm::mat4 cabeza = glm::translate(torso, glm::vec3(0.0f, 1.05f, 0.0f));
	cabeza = glm::rotate(cabeza, glm::radians(4.0f * inten * sinf(ciclo * 2.0f) + 1.5f * sinf(respira)), glm::vec3(1.0f, 0.0f, 0.0f));
	Dibujar(mEsfera, glm::scale(cabeza, glm::vec3(0.42f)), piel);
	//ojos y nariz (hijos de la cabeza, sobre la superficie del frente +Z)
	for (int k = 0; k < 2; k++)
	{
		float lado = (k == 0) ? 1.0f : -1.0f;
		Dibujar(mEsferaBaja, glm::scale(glm::translate(cabeza, glm::vec3(lado * 0.15f, 0.07f, 0.37f)), glm::vec3(0.055f)), oscuro);
		Dibujar(mEsferaBaja, glm::scale(glm::translate(cabeza, glm::vec3(lado * 0.24f, -0.1f, 0.32f)), glm::vec3(0.07f, 0.05f, 0.04f)), glm::vec3(1.0f, 0.6f, 0.6f));	// cachetes
	}
	Dibujar(mEsferaBaja, glm::scale(glm::translate(cabeza, glm::vec3(0.0f, -0.02f, 0.42f)), glm::vec3(0.06f)), glm::vec3(1.0f, 0.55f, 0.5f));	// nariz
	//gorro: una cupula, un borde y un pompon (hijos de la cabeza)
	Dibujar(mEsfera, glm::scale(glm::translate(cabeza, glm::vec3(0.0f, 0.17f, -0.02f)), glm::vec3(0.45f, 0.36f, 0.45f)), gorro);
	Dibujar(mCilindro, glm::scale(glm::translate(cabeza, glm::vec3(0.0f, 0.16f, 0.0f)), glm::vec3(0.45f, 0.1f, 0.45f)), glm::vec3(0.95f));
	Dibujar(mEsferaBaja, glm::scale(glm::translate(cabeza, glm::vec3(0.0f, 0.56f, -0.02f)), glm::vec3(0.12f)), glm::vec3(1.0f));

	// 4. BRAZOS (hijos del torso): cada uno se mueve al contrario que la pierna de su lado; la manopla cuelga del brazo
	for (int k = 0; k < 2; k++)
	{
		float lado = (k == 0) ? 1.0f : -1.0f;
		float anguloBrazo = ((k == 0) ? -ang : ang) * 1.1f + 0.04f * sinf(respira + k);
		glm::mat4 hombro = glm::translate(torso, glm::vec3(lado * 0.5f, 0.35f, 0.0f));
		hombro = glm::rotate(hombro, anguloBrazo, glm::vec3(1.0f, 0.0f, 0.0f));
		hombro = glm::rotate(hombro, glm::radians(-lado * 10.0f), glm::vec3(0.0f, 0.0f, 1.0f));		// los brazos abiertos un poquito
		Dibujar(mCilindro, glm::scale(glm::translate(hombro, glm::vec3(0.0f, -0.32f, 0.0f)), glm::vec3(0.11f, 0.64f, 0.11f)), chamarra);
		Dibujar(mEsferaBaja, glm::scale(glm::translate(hombro, glm::vec3(0.0f, -0.7f, 0.0f)), glm::vec3(0.14f)), glm::vec3(0.95f));	// manopla
	}

	// 5. PIERNAS (hijas del cuerpo) y BOTAS (hijas de la pierna)
	for (int k = 0; k < 2; k++)
	{
		float lado = (k == 0) ? 1.0f : -1.0f;
		float anguloPierna = (k == 0) ? ang : -ang;
		glm::mat4 cadera = glm::translate(cuerpo, glm::vec3(lado * 0.2f, 0.0f, 0.0f));
		cadera = glm::rotate(cadera, anguloPierna, glm::vec3(1.0f, 0.0f, 0.0f));						// gira sobre la cadera
		Dibujar(mCilindro, glm::scale(glm::translate(cadera, glm::vec3(0.0f, -0.46f, 0.0f)), glm::vec3(0.14f, 0.92f, 0.14f)), pantalon);
		Dibujar(mEsferaBaja, glm::scale(glm::translate(cadera, glm::vec3(0.0f, -0.92f, 0.07f)), glm::vec3(0.17f, 0.12f, 0.26f)), bota);
	}
}