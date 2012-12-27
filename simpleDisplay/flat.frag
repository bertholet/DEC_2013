#version 410

//uniform int nrLights;
uniform vec3 eye;
uniform vec3 light_pos;
uniform vec3 light;
uniform vec3 color;


flat in vec3 normal_;
in vec3 position;
out vec4 fragColor;
 
void main( void )
{
	vec3 ambient = vec3(0.3,0.3,0.3);
	vec3 specular= vec3(1,1,1);
	vec3 l = normalize(light_pos - position);
	vec3 normal = normalize(normal_);
	vec3 r= normalize(reflect(l,normal));
	vec3 v = normalize(-eye - position); 
	vec3 color_ = color;

	float k_d = 0.8;
	float k_s = 2.3;
	float alpha = 12;
	
	fragColor = vec4(k_d * dot(l, normal) * color_ * light 
			+ max(k_s *pow(dot(r,v), alpha), 0 )*color_ 
			+ ambient *color_,1) ;


	//fragColor = vec4( normal_ ,1);
}