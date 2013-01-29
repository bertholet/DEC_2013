#version 410

//uniform int nrLights;
uniform vec3 eye;
uniform vec3 light_pos;//
uniform vec3 light;//
//uniform vec3 color;


flat in vec3 normal_g;
in vec3 position_g; 
in vec3 color_g;
out vec4 fragColor;
 
void main( void )
{
	vec3 ambient = vec3(0.3,0.3,0.3);
	vec3 specular= vec3(1,1,1);
	vec3 l = normalize(light_pos - position_g);
	vec3 normal = normalize(normal_g);
	vec3 r= normalize(reflect(l,normal));
	vec3 v = normalize(-eye - position_g); 
	vec3 color = color_g;//vec3(0.9f,0.1f,0.1f);

	//color_ = normal_;

	float k_d = 0.8;
	float k_s = 2.3;
	float alpha = 12;
	
	fragColor = vec4(k_d * dot(l, normal) * color * light 
			+ max(k_s *pow(dot(r,v), alpha), 0 )*color 
			+ ambient *color,1) ;


	//fragColor = vec4( abs(normal_g) ,1);
}