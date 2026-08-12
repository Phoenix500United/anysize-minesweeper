#include <thread>
#include <atomic>
#include <memory>
#include <random>
#include "generation.hpp"



void generation_worker(GenParameters parameters, std::unique_ptr<std::atomic<uint32_t>> &place_count_counter){
    uint32_t place_count_remaining = place_count_counter->load(std::memory_order_relaxed);
    int counter = 0;
    while (true)
    {
        
        uint32_t value = parameters.rng();
        //Where in the byte is the data
        uint8_t shift = value & 0b11;
        //modulo for speed but does have bias
        //Which byte
        uint32_t index = (value >> 2) % parameters.section_size;
        uint8_t bomb_check = to_underlying(CellFlag::Bomb) << shift*2;
        uint8_t cell_cluster = parameters.minefield_section[index];
        if(!(cell_cluster & bomb_check)){
            cell_cluster |= bomb_check;
            //counts till there are no bombs left to place
            if(--place_count_remaining == 0){
                place_count_counter->store(place_count_remaining, std::memory_order_relaxed);
                return;
            //Batch counter for atomic update
            }else if (++counter < 64){
                place_count_counter->store(place_count_remaining, std::memory_order_relaxed);
                counter = 0;
            }
        }

    }
}

void start_generating_minefield(MineField &minefield, int density, std::vector<Thread_Info> &info){

    static constexpr uint32_t min_thread_size = 50000; //minimum thread size bassically not worth running threads for grid sizes smaller than this
    static constexpr uint32_t max_thread_size = UINT32_MAX; //max thread size individual threads are capped at the integer limit because I am using a 32 bit random number for indexing

    //gets the number of hardware threads
    uint32_t max_threads_normal = std::thread::hardware_concurrency();
    if(max_threads_normal == 0){
        max_threads_normal = 1; //if max threads fails set max threads to 1
    }

    // size of the grid * number of cells per byte
    size_t size = minefield.width * minefield.height * 4; 

    gt generation_type;
    size_t gen_amount;
    if (density > 50){
        generation_type = gt::REMOVE;
        gen_amount = (size)*static_cast<float>((100-density))/100.0f;
    }else{
        generation_type = gt::PLACE;
        gen_amount = (size)*static_cast<float>(density)/100.0f;
    }

    //adds another thread for every min_thread_size of cells until max_threads_normal is reached 
    //if max_threads_normal is reached then increases the size of each thead until max thread size is reached
    //then it will further increase nthreads

    uint32_t nthreads = 1; //number of threads
    
    if(size/min_thread_size >= max_threads_normal){
        nthreads = max_threads_normal;
        if (size/nthreads > max_thread_size){
            nthreads = size/max_thread_size + 1;
        }
    }else if(size > min_thread_size) {
        nthreads = size/min_thread_size;
    }
    
    info.reserve(nthreads);

    size_t thread_offset = 0;
    uint32_t thread_size = size/min_thread_size; 
    uint32_t thread_remainder = size % nthreads;

    uint32_t perthread_gen_amount = gen_amount/nthreads;
    uint32_t gen_remainder = perthread_gen_amount % nthreads;

    for(uint32_t i = 0; i < nthreads; ++i){
        Thread_Info t_info;
        GenParameters parameters;

        parameters.generation_type = generation_type;
        parameters.minefield_section = minefield.field.data() + thread_offset;

        //For the size remainder
        if(thread_remainder){
            parameters.section_size = thread_size + 1;
            thread_offset += thread_size + 1;
            thread_remainder--;
        }else{
            parameters.section_size = thread_size;
            thread_offset += thread_size;
        }
        
        //For the amount remainder
        if(gen_remainder){
            t_info.total_place_count = perthread_gen_amount + 1;
            t_info.remaining_place_count = std::make_unique<std::atomic<uint32_t>>(perthread_gen_amount + 1);
            gen_remainder--;
        }else{
            t_info.total_place_count = perthread_gen_amount + 1;
            t_info.remaining_place_count = std::make_unique<std::atomic<uint32_t>>(perthread_gen_amount + 1);;
        }
        t_info.thread = std::thread(generation_worker, parameters, std::ref(t_info.remaining_place_count));
        info.push_back(t_info);
        
    }
} 




