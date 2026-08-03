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
    static bool generating = false;
    std::vector<GenParameters> threadParameters;
    if (generating){

    }else{

    }
}