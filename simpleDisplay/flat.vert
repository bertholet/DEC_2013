#version 410
 

uniform mat4 normalMat;
uniform mat4 model2World;

in vec3 vertex, color;//, normal;
out vec3 position_, color_;
 
void main( void )
{
	position_ = (model2World * vec4(vertex,1)).xyz;
    gl_Position = vec4(vertex, 1);
	color_ = color;
}