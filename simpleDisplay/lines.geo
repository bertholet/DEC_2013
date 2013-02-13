#version 410
#extension GL_EXT_geometry_shader4 : enable

layout(triangles) in;
layout(line_strip, max_vertices=4) out;

void main( void )
{
	//normal_g = normalize(cross(gl_PositionIn[1].xyz - gl_PositionIn[0].xyz, gl_PositionIn[2].xyz - gl_PositionIn[0].xyz));
	
	//normal_g = (model2World * vec4(normal_g.xyz,0)).xyz;

	for(int i=0; i<4; i++)
	{
		gl_Position = gl_in[i%3].gl_Position;
		gl_PrimitiveID = gl_PrimitiveIDIn;
		//color_g = color_[i%3];
		EmitVertex();
	}
	EndPrimitive();
}