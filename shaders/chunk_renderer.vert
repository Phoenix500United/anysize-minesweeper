#version 430 core 

layout (std430, binding = 1) readonly buffer render_location{
    uvec2 chunk_pos[];
}

layout (std140, binding = 0) uniform screen_data {
    mat4 projection; 
    uint chunk_size;
    uint cell_size;
    vec2 camera;
};

const vec2 vertices[6] = vec2[](
    vec2(0.0, 0.0), 
    vec2(1.0, 0.0),
    vec2(1.0, 1.0),
    
    vec2(1.0, 1.0), 
    vec2(0.0, 1.0),
    vec2(0.0, 0.0)
);

out vec2 frag_uv;

void main(){
    frag_uv = vertices[gl_VertexID] 
    vec2 chunk_scale = vec2(cell_size * chunk_size);
    vec2 vertex = frag_uv *  chunk_scale;
    vertex = vertex + vec2(chunk_pos[gl_InstanceID]) * chunk_scale;
    vertex += camera;
    
    gl_Position = projection * vec4(vertex, 0, 1);
}