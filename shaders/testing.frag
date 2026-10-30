#version 430 core 
out vec4 FragColor;
in vec2 frag_uv;
layout (binding = 2) uniform sampler2D minesweeper_texture_atlas;
void main(){
    FragColor = texture(minesweeper_texture_atlas, frag_uv);
}