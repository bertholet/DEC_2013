#version 410
 

uniform mat4 normalMat;
uniform mat4 model2World;

in vec3 vertex, normal;
//flat out vec3 normal_; 
out vec3 position;
 
void main( void )
{
	position = (model2World * vec4(vertex,1)).xyz;
    gl_Position = vec4(vertex, 1);
	//normal_ = (model2World*vec4(normal,1)).xyz;
}