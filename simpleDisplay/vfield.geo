#version 410
#extension GL_EXT_geometry_shader4 : enable

layout(points) in;
layout(triangle_strip, max_vertices=7) out;

uniform mat4 MVmat;
uniform mat4 normalMat;
uniform vec3 color;
uniform float len;
uniform vec3 eye;

in vec3 direction_[];//, normal_[];
//flat out vec3 normal_g;
out vec3 color_g;// position_g;

void main( void )
{

	float tipSz = 0.2;
	float base = 0.05;
	float baselen = 1-1.5*tipSz;

	//vec3 side = normalize(cross(direction_[0], normal_[0]+vec3(0.000001503,0.00000005,0.0000024153)))* length(direction_[0]) * len; //perturb for improbable alignment
	vec3 side = normalize(cross(direction_[0], (normalMat*vec4(eye,1)).xyz+vec3(0.000001503,0.00000005,0.0000024153)))* length(direction_[0]) * len; //perturb for improbable alignment
	vec3 dir_ =  direction_[0]*len;

	gl_PrimitiveID = gl_PrimitiveIDIn;
	gl_Position = MVmat * vec4(gl_in[0].gl_Position.xyz -side*base,1);
	color_g = color*0.1;
	EmitVertex();

	gl_Position = MVmat * vec4(gl_in[0].gl_Position.xyz +side*base,1);
	color_g = color*0.1;
	EmitVertex();

	gl_Position =MVmat * vec4(gl_in[0].gl_Position.xyz + dir_*baselen -side*base,1);
	color_g = color*baselen;
	EmitVertex();

	gl_Position =MVmat * vec4(gl_in[0].gl_Position.xyz + dir_*baselen +side*base,1);
	color_g = color*baselen;
	EmitVertex();

	gl_Position =MVmat * vec4(gl_in[0].gl_Position.xyz + dir_,1);
	color_g = color;
	EmitVertex();

	gl_Position =MVmat * vec4(gl_in[0].gl_Position.xyz + dir_*baselen + side*tipSz ,1);
	color_g = color*baselen;
	EmitVertex();


	gl_Position =MVmat * vec4(gl_in[0].gl_Position.xyz + dir_*baselen - side*tipSz ,1);
	color_g = color*baselen;
	EmitVertex();

		

	EndPrimitive();
}