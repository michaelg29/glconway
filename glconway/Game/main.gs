#version 330 core

// take in points from vertex shader
layout (points) in;
// output square (4 vertices)
layout (triangle_strip, max_vertices=4) out;

uniform float cellWidth;
uniform float cellHeight;

in VS_OUT {
	int valid;
} gs_in[];

void buildCell(vec4 pos) {
	gl_Position = pos;
	EmitVertex();

	gl_Position = pos + vec4(cellWidth, 0.0, 0.0, 0.0);
	EmitVertex();

	gl_Position = pos + vec4(0.0, cellHeight, 0.0, 0.0);
	EmitVertex();

	gl_Position = pos + vec4(cellWidth, cellHeight, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();
}

void main() {
	// only render cell if alive
	if (gs_in[0].valid != 0) {
		buildCell(gl_in[0].gl_Position);
	}
}