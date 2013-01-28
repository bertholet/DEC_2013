#version 410
#extension GL_EXT_geometry_shader4 : enable

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

uniform mat4 MVmat;
uniform mat4 model2World;

in vec3 color_[];
in vec3 position_[];
flat out vec3 normal_g;
out vec3 color_g, position_g;

void main( void )
{
	normal_g = normalize(cross(gl_PositionIn[1].xyz - gl_PositionIn[0].xyz, gl_PositionIn[2].xyz - gl_PositionIn[0].xyz));
	
	normal_g = (model2World * vec4(normal_g.xyz,0)).xyz;

	for(int i=0; i<3; i++)
	{
		gl_Position = MVmat * gl_in[i].gl_Position;
		gl_PrimitiveID = gl_PrimitiveIDIn;
		color_g = color_[i];
		position_g= position_[i];
		EmitVertex();
	}
	EndPrimitive();
}