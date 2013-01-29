#version 410
#extension GL_EXT_geometry_shader4 : enable

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

uniform mat4 MVmat;
uniform mat4 model2World;

in vec3 color_[];
in vec3 position_[];
in vec3 normal_[];
out vec3 normal_g;
out vec3 color_g, position_g;

void main( void )
{
	for(int i=0; i<3; i++)
	{
		gl_Position = MVmat * gl_in[i].gl_Position;
		gl_PrimitiveID = gl_PrimitiveIDIn;
		color_g = color_[i];
		position_g= position_[i];
		normal_g = normal_[i];
		EmitVertex();
	}
	EndPrimitive();
}