#pragma once
#include <vector>
#include <cstdint>



enum class cellflag : uint8_t{
    Bomb = 1,
    Flag = 3, 
    Unrevealed = 0,
    Revealed = 2,
};

constexpr uint8_t to_underlying(cellflag cf){
    return static_cast<uint8_t>(cf);
}

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
        (to_underlying(cellflag::Bomb) << 6) |
        (to_underlying(cellflag::Bomb) << 4) | 
        (to_underlying(cellflag::Bomb) << 2) |
        (to_underlying(cellflag::Bomb));
        field = std::vector<uint8_t>(bombValue, width*height);
        width = w;
        height = h;
    }
    void generate_empty(size_t w, size_t h){
        field = std::vector<uint8_t>(0, w*h);
        width = w;
        height = h;
    }
};

void generate_minefield(MineField &minefield);