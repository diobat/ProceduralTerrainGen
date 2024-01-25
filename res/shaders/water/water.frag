#version 430

// Inputs from the vertex shader
in VertexOutput{
	vec3 objectColor;
} FragmentIn;


// Outputs
layout (location = 0) out vec4 fragColor;

void main()
{
	fragColor = vec4( FragmentIn.objectColor, 1.0);
}
