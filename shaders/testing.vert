#version 430 core
const vec2 positions[6] = vec2[](
    vec2(-1.0,  1.0),  // top-left
    vec2( 1.0,  1.0),  // top-right
    vec2(-1.0, -1.0),  // bottom-left

    vec2(-1.0, -1.0),  // bottom-left
    vec2( 1.0,  1.0),  // top-right
    vec2( 1.0, -1.0)   // bottom-right
);

const vec2 uvs[6] = vec2[](
    vec2(0.0, 1.0),    // top-left
    vec2(1.0, 1.0),    // top-right
    vec2(0.0, 0.0),    // bottom-left

    vec2(0.0, 0.0),    // bottom-left
    vec2(1.0, 1.0),    // top-right
    vec2(1.0, 0.0)     // bottom-right
);

out vec2 frag_uv;

void main(){
    frag_uv = uvs[gl_VertexID];
    gl_Position = vec4(positions[gl_VertexID], 0 , 1);
}