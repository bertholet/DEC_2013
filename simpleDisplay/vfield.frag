#version 410

//uniform int nrLights;
//uniform vec3 eye;
//uniform vec3 light_pos;
//uniform vec3 light;



//flat in vec3 normal_g;
//in vec3 position_g; 
in vec3 color_g;
out vec4 fragColor;
 
void main( void )
{
	fragColor = vec4(color_g,1) ;


	//fragColor = vec4( normal_ ,1);
}