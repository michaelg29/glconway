#version 330 core

// take in current cell as an integer (only occupies LSB)
layout (location = 0) in int val;

uniform int width;
uniform float cellWidth;
uniform float cellHeight;

// output structure
out VS_OUT {
	int valid;
} vs_out;

void main() {
	if (val != 0) {
		// tell geometry shader to render
		vs_out.valid = 1;

		// calculate grid coordinates using index
		float row = float(gl_VertexID / width);
		float col = float(gl_VertexID % width);

		// output bottom-left coordinate of each box
		// grid cell --> [0, 1] x [0, 1] --> [-1, 1] x [-1, 1]
		gl_Position = vec4(
			col * cellWidth - 1.0,
			row * cellHeight - 1.0,
			0.0,
			1.0
		);
	}
	else {
		// tell geometry shader not to render
		vs_out.valid = 0;
	}
}