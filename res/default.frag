#version 450 core

layout(location = 0) out vec4 o_Color;

in vec4 v_Color;
in vec2 v_TexCoord;

// could have multiple and pick one according to the index, but there is allways just one texture
uniform sampler2D u_Texture;

void main() {
	o_Color = texture(u_Texture, v_TexCoord) * v_Color;
}
