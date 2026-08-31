#pragma once
#include <vector>
#include <cstdint>
#include <thread>
#include <atomic>
#include <memory>




enum class CellFlag : uint8_t{
    Unrevealed = 0,
    Revealed = 1,
    Bomb = 2,
    Flag = 3, 
};

static inline constexpr uint8_t to_underlying(CellFlag cf){
    return static_cast<uint8_t>(cf);
}

struct ThreadInfo{

    std::thread thread;
    std::atomic<uint32_t> remaining_place_count;
    uint32_t total_place_count;
}; 

struct ThreadInfoArray {
    std::unique_ptr<ThreadInfo[]> data;
    size_t nthreads;

    ThreadInfo& operator[](size_t i) {
        return data[i];
    }

    const ThreadInfo& operator[](size_t i) const {
        return data[i];
    }
};


enum class gt : bool {
    PLACE,
    REMOVE,
};

enum class GenProgress : uint8_t {
    INITILIZING_MINEFIELD,
    LOADING_THREADS,
    DONE
};

struct GenParameters{
    uint8_t* minefield_section;
    uint32_t section_size;
    gt generation_type;
    uint32_t thread_seed;
};

class MineField
{   
    
    public:
    size_t width;
    size_t height;
    std::vector<uint8_t> field;

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

void start_generating_minefield(MineField &minefield, int density, size_t width, size_t height, ThreadInfoArray &info, std::atomic<GenProgress> &generating_indicator);