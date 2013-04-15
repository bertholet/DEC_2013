#version 410

//uniform int nrLights;
// uniform vec3 eye;
// uniform vec3 light_pos;//
// uniform vec3 light;//
// //uniform vec3 color;
// 
// 
// flat in vec3 normal_g;
// in vec3 position_g; 
// in vec3 color_g;
out vec4 fragColor;
 
void main( void )
{
	vec3 color = vec3(0.9f,0.1f,0.1f);
	fragColor = vec4(color,1);
}