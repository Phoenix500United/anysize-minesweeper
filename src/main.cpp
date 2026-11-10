#include "minefield.hpp"
#include "matrix.hpp"
#include <ctime>
#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include <vector>
#include <memory>
#include "raylib.h"
#include "rlgl.h"
#include "external/glad.h"
#include "shaders.hpp"
#include "textures.hpp"

enum ChunkRendererUniformLocations{
    MODE = 0,
    CELL_SIZE = 1,
    CELL_SIZE_CUTOFF = 2,
};


class Timer {
private:
    std::chrono::steady_clock::time_point start;

public:
    void start_timer() {
        start = std::chrono::steady_clock::now();
    }

    double get_time() {
        auto end = std::chrono::steady_clock::now();

        return std::chrono::duration<double, std::milli>(
            end - start
        ).count();
    }
};

struct alignas(16) StaticData {
    Mat4f projection;
    Vec2<GLuint> chunk_size;
};

int main()
{
    
    Timer timer;
    ThreadInfoArray ti;
    
    std::atomic<GenProgress> generation_flag = GenProgress::INITILIZING_MINEFIELD;
    
    int height, width;
    float density;
    // std::cout << "Input Width (even number only): ";
    // std::cin >> width;
    // std::cout << "Input Height (even number only): ";
    // std::cin >> height;
    // std::cout << "Input a Density percentage from 0->100: ";
    // std::cin >> density;
    // std::cout << "Generating...\n";
    height = 3256;
    width = 3256;
    density = 0.75;


    MineField mf(width, height, density);
    timer.start_timer();
    std::thread start_gen_thread(start_generating_minefield, std::ref(mf), std::ref(ti), std::ref(generation_flag));
    start_gen_thread.join();


    for (size_t i = 0; i < ti.nthreads; ++i) {
        if (ti[i].thread.joinable()) {
            ti[i].thread.join();
        }
    }

    
    
    std::cout << "TIME: " << timer.get_time() << "ms" << std::endl;
    timer.start_timer();
    InitWindow(1000, 1000, "anysize");
    std::cout << "WINDOW INIT: " << timer.get_time() << std::endl;
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    SetTargetFPS(144);

    timer.start_timer();
    std::cout << "COMPILING RENDERER" << std::endl;
    Shader chunk_renderer = LoadShaderFromMemory(chunk_renderer_vert, chunk_renderer_frag);

    std::cout << "COMPILING TESTING SHADER" << std::endl;
    Shader testing_shader = LoadShaderFromMemory(testing_vert, testing_frag);

    std::cout << "COMPILING PROCCESSOR" << std::endl;
    GLuint temp = rlLoadShader(minefield_proccessor_optomized_comp, RL_COMPUTE_SHADER);
    GLuint minefield_proccessor = rlLoadShaderProgramCompute(temp); 

    std::cout << "SHADER COMPILATION: " << timer.get_time() << std::endl;

    Image cpu_minesweeper_textures = LoadImageFromMemory(".png", minesweeper_textures_png, minesweeper_textures_png_size);
    Image cpu_solid_minesweeper_textures = LoadImageFromMemory(".png", minesweeper_solid_colour_textures_png, minesweeper_solid_colour_textures_png_size);

    ImageFormat(&cpu_minesweeper_textures, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    ImageFormat(&cpu_solid_minesweeper_textures, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    GLuint minesweeper_textures;
    GLuint solid_minesweeper_textures;
    
    glGenTextures(1, &minesweeper_textures);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, minesweeper_textures);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8, 
        cpu_minesweeper_textures.width, 
        cpu_minesweeper_textures.height, 
        0, GL_RGBA, GL_UNSIGNED_BYTE, 
        cpu_minesweeper_textures.data
    );
    UnloadImage(cpu_minesweeper_textures);

    glGenTextures(1, &solid_minesweeper_textures);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, solid_minesweeper_textures);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8,
        cpu_solid_minesweeper_textures.width,
        cpu_solid_minesweeper_textures.height,
        0, GL_RGBA, GL_UNSIGNED_BYTE,
        cpu_solid_minesweeper_textures.data
    );
    UnloadImage(cpu_solid_minesweeper_textures);

    RenderBuffer rb;
    mf.initilize_chunk_interpretation(128, 128);


    GLint alignment;
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &alignment);
    rb.construct_render_buffer(mf, alignment, GetScreenWidth(), GetScreenHeight());


    GLuint unpacked_chunks_array; 
    GLuint packed_chunks_array; 
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &packed_chunks_array);
    glBindTexture(GL_TEXTURE_2D_ARRAY, packed_chunks_array);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
    glTexStorage3D(
        GL_TEXTURE_2D_ARRAY, 
        1, GL_R8UI, 
        mf.chunk_width_bytes + 2, 
        mf.chunk_height_bytes + 2, 
        rb.screen_chunk_count
    );
    GLenum err = glGetError();
    std::cout << "Storage allocation: " << err << '\n';

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &unpacked_chunks_array);
   
    glBindTexture(GL_TEXTURE_2D_ARRAY, unpacked_chunks_array);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexStorage3D(
        GL_TEXTURE_2D_ARRAY, 
        1, GL_R8UI, 
        mf.chunk_width_cells, 
        mf.chunk_height_cells, 
        rb.screen_chunk_count
    );

    
    glBindImageTexture(
        0, 
        unpacked_chunks_array, 
        0, 
        GL_TRUE, 
        0, 
        GL_WRITE_ONLY, 
        GL_R8UI
    );

    GLuint dynamic_data;
    glGenBuffers(1, &dynamic_data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dynamic_data);
    glBufferData(GL_SHADER_STORAGE_BUFFER, rb.render_meta_data_size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, dynamic_data, 0, rb.output_layer_buffer_size);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, dynamic_data, rb.camera_offset, rb.location_buffer_size);

    StaticData sd;
    sd.projection = transform_matrix(0, GetScreenWidth(), 0, GetScreenHeight(), -1, 1);
    sd.chunk_size = Vec2<GLuint>({static_cast<GLuint>(mf.chunk_width_cells), static_cast<GLuint>(mf.chunk_height_cells)});
   
    GLuint static_data; 
    glGenBuffers(1, &static_data);
    glBindBuffer(GL_UNIFORM_BUFFER, static_data);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(StaticData), &sd, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, static_data);

    glUseProgram(chunk_renderer.id);
    glUniform1i(MODE, 2);
    glUniform1ui(CELL_SIZE, 5);
    glUniform1ui(CELL_SIZE_CUTOFF, 5);

    for(size_t y = 0; y < rb.screen_chunk_height; ++y){
        for(size_t x = 0; x < rb.screen_chunk_width; ++x){
            //std::cout << x << ", " << y << std::endl;
            rb.add_chunk(mf, x, y, y * (rb.screen_chunk_width) + x, Vec2<uint32_t>({static_cast<uint32_t>(x), static_cast<uint32_t>(y)}));
        }
    }

    rb.camera() = Vec2f({0.0f, -128.0f});
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dynamic_data);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, rb.render_meta_data_size,  rb.data());
    
    glActiveTexture(GL_TEXTURE0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
    glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY, 
        0, 
        0, 0, 0, 
        mf.chunk_width_bytes + 2, 
        mf.chunk_height_bytes + 2, 
        rb.layer_index, //doesn't refer to the index I am uploading but how high the current index for chunk buffer construction is
        GL_RED_INTEGER, 
        GL_UNSIGNED_BYTE, 
        rb.get_chunk_buffer()
    ); 
    //std::unique_ptr<uint8_t[]> image = std::make_unique<uint8_t[]>((mf.chunk_width_cells) * (mf.chunk_height_cells));



    GLuint dummy_vao;
    glGenVertexArrays(1, &dummy_vao);
    
    glUseProgram(minefield_proccessor);
    glDispatchCompute(mf.chunk_width_cells/16, mf.chunk_height_cells/16, rb.layer_index);
    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

    // glPixelStorei(GL_PACK_ALIGNMENT, 4);
    // glGetTextureSubImage(
    //     packed_chunks_array,
    //     0,
    //     0,0, 25,
    //     mf.chunk_width_cells, mf.chunk_height_cells, 1,
    //     GL_RED_INTEGER,
    //     GL_UNSIGNED_BYTE,
    //     (mf.chunk_width_bytes + 2) * (mf.chunk_height_bytes + 2),
    //     image.get()
    // );

    // for(size_t y = 0; y < mf.chunk_height_cells; ++y){
    //     for(size_t x = 0; x < mf.chunk_width_cells; ++x){
    //         std::cout << static_cast<int>(image[y * mf.chunk_width_cells + x]) << ", ";
    //     }
    //     std::cout << std::endl;
    // }


    while(!WindowShouldClose()){
        rb.chunk_buffer_clear();
        BeginDrawing();
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, minesweeper_textures);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D_ARRAY, unpacked_chunks_array);
            glBindVertexArray(dummy_vao);
            glUseProgram(chunk_renderer.id);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, rb.layer_index);
        EndDrawing();
    }
    rlUnloadShader(minefield_proccessor); 
    UnloadShader(chunk_renderer);
    UnloadShader(testing_shader);

    glDeleteBuffers(1, &static_data);
    glDeleteBuffers(1, &dynamic_data);
    glDeleteTextures(1, &minesweeper_textures);
    glDeleteTextures(1, &unpacked_chunks_array);
    glDeleteTextures(1, &packed_chunks_array);

    CloseWindow();
    return 0;
}
