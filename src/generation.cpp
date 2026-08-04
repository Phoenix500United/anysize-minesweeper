#include <thread>
#include <atomic>
#include <random>
#include "generation.hpp"

enum class gt : bool {
    PLACE,
    REMOVE,
};
struct GenParameters{
    uint8_t* minefield_section;
    uint32_t section_size;
    gt generation_type;

    std::mt19937 rng;
    std::atomic<uint32_t> place_count_remaining;  
};

void generation_worker(GenParameters &parameters){
    while (true)
    {
        uint32_t value = parameters.rng();
        uint8_t shift = value & 0b11;
        //modulo for speed but does have bias
        uint32_t index = (value >> 2) % parameters.section_size;
        uint8_t bomb_check = to_underlying(CellFlag::Bomb) << shift*2;
        uint8_t cell_cluster = parameters.minefield_section[index];
        if(!(cell_cluster & bomb_check)){
            cell_cluster |= bomb_check;
            if(!(--parameters.place_count_remaining)){
                return;
            }
        }
    }
}

float generate_minefield(MineField &minefield){
    static uint32_t max_threads = std::thread::hardware_concurrency();
    static bool generating = false;
    std::vector<GenParameters> threadParameters;

    

    if (generating){

    }else{
        uint32_t min_thread_size = 50000; //minimum thread size bassically not worth running threads for grid sizes smaller than this
        uint32_t max_thread_size = UINT32_MAX; //max thread size individual threads are capped at the integer limit because I am using a 32 bit random number for indexing


        //gets the number of hardware threads
        static uint32_t max_threads_normal = std::thread::hardware_concurrency();
        if(max_threads_normal == 0){
            max_threads_normal = 1; //if max threads fails set max threads to 1
        }

        // size of the grid * number of cells per byte
        size_t size = minefield.width * minefield.height * 4; 

        

        //adds another thread for every min_thread_size of cells until max_threads_normal is reached 
        //if max_threads_normal is reached then increases the size of each thead until max thread size is reached
        //then it will further increase nthreads

        uint32_t nthreads;
        uint32_t base_size = min_thread_size;
        uint32_t remainder;

        
        if(size/min_thread_size < max_threads_normal){
            nthreads = max_threads_normal;
            if (size/nthreads > max_thread_size){
                nthreads = size/max_thread_size + 1;
                base_size = max_thread_size;
            }else{ 
                base_size = size/nthreads;
            }
        }else {
            nthreads = size/min_thread_size;
            base_size = size/nthreads;
        }
        
        threadParameters.reserve(nthreads);
        size_t thread_offset = 0;
        uint32_t remainder = size % nthreads;
        for(uint32_t i = 0; i < threadParameters.size(); i++){
            GenParameters parameter;
            parameter.minefield_section = minefield.field.data() + thread_offset;
            if(remainder){
                parameter.section_size = base_size + 1;
                thread_offset += base_size + 1;
                remainder--;
            }else{
                parameter.section_size = base_size;
                thread_offset += base_size;
            }   
            threadParameters.push_back(parameter);
        }

        
    }
}