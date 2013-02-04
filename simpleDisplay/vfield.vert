#version 410
 

//uniform mat4 normalMat;
//uniform mat4 model2World;

in vec3 position, direction; //normal;
out vec3 direction_;//, normal_;
 
void main( void )
{
	direction_ = direction;//(model2World * vec4(vertex,1)).xyz;
	//normal_ = normal;
    gl_Position = vec4(position, 1);
}