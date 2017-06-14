#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 normal;
out vec3 position;
void main()
	//normal = mat3(transpose(inverse(model))) * aNormal; 
{	//normal=(model*vec4(aNormal,0.0)).xyz;
	normal = mat3(transpose(inverse(model))) * aNormal; 
	//normal=aNormal;
	position=vec3(model*vec4(aPos,1.0));
    TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}