#version 410
 
uniform mat4 MVmat;
uniform mat4 model2World;

in vec3 vertex, normal, color;
out vec3 normal_, position_, color_;
 
void main( void )
{
	position_ = (model2World * vec4(vertex,1)).xyz;
    gl_Position = vec4(vertex, 1);
	normal_ = (model2World*vec4(normal,1)).xyz;
	color_ = color;
}