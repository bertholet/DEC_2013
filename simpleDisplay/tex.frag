#version 410

//uniform int nrLights;
uniform vec3 eye;
uniform vec3 light_pos;
uniform vec3 light;


in vec3 normal_g, color_g;//color and normal is anywhere....
in vec3 position_g;//pos ok.
out vec4 fragColor;
 
void main( void )
{
	vec3 ambient = vec3(0.3,0.3,0.3);
	vec3 specular= vec3(1,1,1);
	vec3 l = normalize(light_pos - position_g);
	vec3 normal = normalize(normal_g);
	vec3 r= normalize(reflect(l,normal));
	vec3 v = normalize(-eye - position_g); 
	//vec3 color_ = color_g;

	float k_d = 0.8;
	float k_s = 2.3;
	float alpha = 12;
	
	//fragColor = vec4(0,1,0,1);
	fragColor = vec4(k_d * dot(l, normal) * color_g * light 
			+ max(k_s *pow(dot(r,v), alpha), 0 )*color_g
			+ ambient *color_g,1) ;
  
  //fragColor = vec4( abs(normal_g) ,1);
}