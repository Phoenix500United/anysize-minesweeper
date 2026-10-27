#include "minefield.hpp"
#include <ctime>
#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include <vector>
#include "raylib.h"
#include "rlgl.h"
#include "external/glad.h"
#include "shaders.hpp"
#include "textures.hpp"


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

int main()
{
    
    Timer timer;
    ThreadInfoArray ti;
    
    std::atomic<GenProgress> generation_flag = GenProgress::INITILIZING_MINEFIELD;
    
    int height, width, density ;
    // std::cout << "Input Width (even number only): ";
    // std::cin >> width;
    // std::cout << "Input Height (even number only): ";
    // std::cin >> height;
    // std::cout << "Input a Density percentage from 0->100: ";
    // std::cin >> density;
    // std::cout << "Generating...\n";
    height = 100;
    width = 100;
    density = 20;

    MineField mf(width, height, density);
    timer.start_timer();
    std::thread start_gen_thread(start_generating_minefield, std::ref(mf), density, width, height, std::ref(ti), std::ref(generation_flag));
    start_gen_thread.join();
    
    
    InitWindow(1024, 1024, "anysize");
    std::cout << "COMPILING RENDERER" << std::endl;
    Shader chunk_renderer = LoadShaderFromMemory(chunk_renderer_vert, chunk_renderer_frag);

    std::cout << "COMPILING PROCCESSOR" << std::endl;
    GLuint temp = rlLoadShader(minefield_proccessor_optomized_comp, RL_COMPUTE_SHADER);
    GLuint minefield_proccessor = rlLoadShaderProgramCompute(temp); 
    
    //mf.initilize_chunk_interpretation(128, 128, )

    for (size_t i = 0; i < ti.nthreads; ++i) {
        if (ti[i].thread.joinable()) {
            ti[i].thread.join();
        }
    }
    std::cout << "TIME: " << timer.get_time() << "ms" << std::endl;



    return 0;
}
