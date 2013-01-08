#version 410
 
uniform mat4 MVmat;
uniform mat4 normalMat;
uniform mat4 model2World;

in vec3 vertex, normal, color;
out vec3 normal_, position, color_;
 
void main( void )
{
	position = (model2World * vec4(vertex,1)).xyz;
    gl_Position = MVmat * vec4(vertex, 1);
	normal_ = (model2World*vec4(normal,1)).xyz;
	color_ = color;
}