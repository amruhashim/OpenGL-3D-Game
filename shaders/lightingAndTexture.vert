#version 330 core

// input data
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

// uniform input data
uniform mat4 uMVPMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;

//add
uniform sampler2D uTextureSampler2;
uniform float uFactor;

// output data
out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
	// set vertex position
    gl_Position = uMVPMatrix * vec4(aPosition, 1.0f);

	// set vertex shader output
	// will be interpolated for each fragment
	vPosition = (uModelMatrix * vec4(aPosition, 1.0f)).xyz;
	vNormal = uNormalMatrix * aNormal;
	vTexCoord = aTexCoord;
}
