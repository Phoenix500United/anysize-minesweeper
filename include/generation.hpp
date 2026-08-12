#pragma once
#include <vector>
#include <cstdint>



enum class CellFlag : uint8_t{
    Bomb = 1,
    Flag = 3, 
    Unrevealed = 0,
    Revealed = 2,
};

static inline constexpr uint8_t to_underlying(CellFlag cf){
    return static_cast<uint8_t>(cf);
}

struct Thread_Info
{
    std::thread thread;
    std::unique_ptr<std::atomic<uint32_t>> remaining_place_count;
    uint32_t total_place_count;
};

enum class gt : bool {
    PLACE,
    REMOVE,
};


struct GenParameters{
    uint8_t* minefield_section;
    uint32_t section_size;
    gt generation_type;
    std::mt19937 rng;
};

class MineField
{   
    
    public:
    size_t width;
    size_t height;
    std::vector<uint8_t> field;
    void insert_bomb(size_t x, size_t y){
        field[y*width + x];
    }

    void generate_full(size_t w, size_t h){
        constexpr uint8_t bombValue = 
        (to_underlying(CellFlag::Bomb) << 6) |
        (to_underlying(CellFlag::Bomb) << 4) | 
        (to_underlying(CellFlag::Bomb) << 2) |
        (to_underlying(CellFlag::Bomb));
        field = std::vector<uint8_t>(w*h, bombValue);
        width = w;
        height = h;
    }
    void generate_empty(size_t w, size_t h){
        field = std::vector<uint8_t>(w*h, 0);
        width = w;
        height = h;
    }
};

float generate_minefield(MineField &minefield);