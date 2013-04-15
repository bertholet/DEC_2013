#version 410
 

uniform mat4 MVmat;

in vec3 vertex;
 
void main( void )
{
	gl_Position = MVmat * vec4(vertex, 1);
}