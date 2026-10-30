#version 430 core 

layout (std430, binding = 1) readonly buffer render_location{
    vec2 camera; 
    uvec2 chunk_pos[];
};

layout (std140, binding = 0) uniform screen_data {
    mat4 projection; 
    uvec2 chunk_size;
};

const vec2 vertices[6] = vec2[](
    vec2(0.0, 0.0), 
    vec2(1.0, 0.0),
    vec2(1.0, 1.0),
    
    vec2(1.0, 1.0), 
    vec2(0.0, 1.0),
    vec2(0.0, 0.0)
);

layout (location = 1) uniform uint cell_size;

out vec2 frag_uv;
flat out uint InstanceID;

void main(){
    frag_uv = vertices[gl_VertexID]; 

    //scales verticies
    vec2 chunk_scale = vec2(cell_size * chunk_size);
    vec2 vertex = frag_uv *  chunk_scale;

    //translates verticies
    InstanceID = gl_InstanceID;
    vertex = vertex + vec2(chunk_pos[InstanceID]) * chunk_scale;
    vertex += camera;
    
    //projects verticies
    gl_Position = projection * vec4(vertex, 0, 1);
}