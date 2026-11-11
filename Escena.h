#pragma once
// Datos que el main le pasa a cada planeta para que pueda dibujar (uniforms y materiales)
#include <glew.h>
#include "Material.h"

struct ContextoEscena
{
	GLuint uModel;			// uniformModel
	GLuint uColor;			// uniformColor
	GLuint uSpecular;		// uniformSpecularIntensity
	GLuint uShininess;		// uniformShininess
	GLuint uTexOffset;		// uniformTextureOffset
	Material* opaco;
	Material* brillante;
	Material* emisivo;
};
