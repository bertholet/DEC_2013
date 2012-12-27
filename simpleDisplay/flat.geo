#version 410
#extension GL_EXT_geometry_shader4 : enable

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

uniform mat4 MVmat;
uniform mat4 model2World;

flat out vec3 normal_;

void main( void )
{
	normal_ = normalize(cross(gl_PositionIn[1].xyz - gl_PositionIn[0].xyz, gl_PositionIn[2].xyz - gl_PositionIn[0].xyz));
	
	normal_ = (model2World * vec4(normal_.xyz,0)).xyz;

	for(int i=0; i<3; i++)
	{
		gl_Position = MVmat * gl_in[i].gl_Position;
		gl_PrimitiveID = gl_PrimitiveIDIn;
		EmitVertex();
	}
	EndPrimitive();
}