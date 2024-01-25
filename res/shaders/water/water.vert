#version 430

// Input Layout Locations
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aObjectColor;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in mat4 instanceMatrix;
//layout(location = 6) in mat4 instanceMatrix;		// Taken due to mat4x4
//layout(location = 7) in mat4 instanceMatrix;		// Taken due to mat4x4
//layout(location = 8) in mat4 instanceMatrix;		// Taken due to mat4x4

layout(std140) uniform mvp_camera 
{
	mat4 view;
	mat4 projection;
};

// Output
out VertexOutput{
	vec3 objectColor;
} VertexOut;


void main()
{
	VertexOut.objectColor = aObjectColor;
	vec4 pos = projection * view * instanceMatrix * vec4(aPosition, 1.0f);
	gl_Position = pos.xyzw;
}