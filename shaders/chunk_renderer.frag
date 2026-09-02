#version 430 core 

out vec4 Frag_color;

in vec2 frag_uv;

(binding = 1) uniform usampler2DArray unpacked_minefield_chunk
(binding = 2) uniform sampler2D minesweeper_texture_atlas;

layout (std140, binding = 0) uniform screen_data {
    mat4 projection; 
    uint chunk_size;
    uint cell_size;
    vec2 camera;
};

(binding = 3) uniform int mode;

const float num_textures = 14.0
const uint unrevealed_offset = 9;
const uint bomb_offset = 11;

const uint revealed_shift = 7;
const uint flag_shift = 5;
const uint bomb_shift = 6;
const uint value_and = 15;

const uint NORMAL = 0;
const uint GAME_OVER = 1;
const uint REVEAL = 2;

void main(){
    vec2 scaled_uv = frag_uv * vec2(chunk_size);

    ivec2 cell = ivec2(floor(scaled_uv));
    vec2 uv = fract(scaled_uv);
    uint cell_value = texelFetch(unpacked_minefield_chunk, vec3(cell, gl_InstanceID), 0).r; 

    float texture_offset = 0.0;

    if(mode == NORMAL){

        //Normal cares about revealed/value and flag

        uint is_revealed = (cell_value >> revealed_shift) & 1u;
        uint is_flag = (cell_value >> flag_shift) & 1u;
        uint value = (cell_value & value_and);
        float texture_offset = float(value * is_revealed + (unrevealed_offset + is_flag) * (1u - is_revealed));

    }else if(mode == GAME_OVER){

        //Game Over cares about unflagged bombs, flags and revealed/value 
        //false flags will be dealt with seperatly due to me not actually being able to encode them in the packed
        //minefield representation that I get the unpacked one from

        uint is_revealed = (cell_value >> revealed_shift) & 1u;
        uint is_flag = (cell_value >> flag_shift) & 1u;
        uint is_bomb = (cell_value >> bomb_shift) & iu;
        uint value = (cell_value & value_and);
        float texture_offset = float(value * is_revealed + (unrevealed_offset + is_flag) * (1u - is_revealed));
        float texture_offset = texture_offset * float(1u - is_bomb) + float(is_bomb * bomb_offset);
        
    }else {
        //Revealed only cares about bomb and value

        uint is_bomb = (cell_value >> bomb_shift_shift) & 1u;
        uint value = (cell_value & value_and);
        float texture_offset = float(value * (1u - is_bomb) + (bomb_offset + is_bomb) * (is_bomb));

    }

    vec2 texture_uv = vec2(uv.x, (uv.y + texture_offset)/num_textures)
    Frag_color = texture(minesweeper_texture_atlas, texture_uv);
}