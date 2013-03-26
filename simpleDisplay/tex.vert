#version 410
 
uniform mat4 MVmat;
uniform mat4 model2World;

in vec3 vertex, normal, color, tex;
out vec3 normal_, position_, color_;
 
void main( void )
{
	position_ = (model2World * vec4(tex,1)).xyz; //vertex
    gl_Position = vec4(tex, 1);
	normal_ = (model2World*vec4(normal,1)).xyz;
	color_ = color;
}