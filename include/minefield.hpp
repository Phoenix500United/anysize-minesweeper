#pragma once
#include <vector>
#include <cstdint>
#include <thread>
#include <atomic>
#include <memory>
#include <cmath>
#include <span>
#include <cstring>





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


enum class Edge : uint8_t {
    TOP,
    BOTTOM,
    LEFT, 
    RIGHT,
    NONE
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


struct UVec2 
{
    uint32_t x; 
    uint32_t y;
};

class RenderBuffer {
    public:

    std::vector<uint8_t> render_buffer;
    std::vector<uint32_t> chunk_indicies; 

    RenderBuffer() = default;

    RenderBuffer(size_t render_buffer_size, uint32_t chunk_size, uint32_t gpu_padding){
        render_buffer.resize(render_buffer_size);
        chunk_indicies.reserve(2 * render_buffer_size / ((chunk_size + gpu_padding) * chunk_size));
    }

    uint8_t* data(){
        return render_buffer.data();
    }

    void insert(uint32_t chunk_x, uint32_t chunk_y){
        chunk_indicies.push_back(chunk_x);
        chunk_indicies.push_back(chunk_y);
    }

    void clear(){
        chunk_indicies.clear();
    } 

    uint8_t& operator[](int index){
        return render_buffer[index];
    }
};

class MineField
{   
    std::vector<uint8_t> field;
    RenderBuffer render_buffer;
    std::vector<uint8_t> blank_chunk;
    uint8_t chunk_size = 128/2;
    uint8_t gpu_padding = 2;
    uint16_t render_chunk_bytes;
    uint16_t chunk_leftovers_y;
    uint16_t chunk_leftovers_x;
    uint32_t chunk_width; 
    uint32_t chunk_height;


    const uint8_t reveal_value = 
        (to_underlying(CellFlag::Revealed) << 6) |
        (to_underlying(CellFlag::Revealed) << 4) | 
        (to_underlying(CellFlag::Revealed) << 2) |
        (to_underlying(CellFlag::Revealed));

    // void genereate_blank_chunk(){
    //     constexpr 

    //     blank_chunk = std::vector<uint8_t>((chunk_size+2) * (chunk_size+2), reveal_value);
    // }


    const uint32_t TOP_LEFT_CORNER = 0;
    uint32_t TOP_RIGHT_CORNER = chunk_size-1;
    uint32_t BOTTOM_LEFT_CORNER = (chunk_size-1) * chunk_size;
    uint32_t BOTTOM_RIGHT_CORNER = chunk_size * chunk_size -1;
    const uint32_t LEFT_EDGE = 0;

    uint32_t RIGHT_EDGE = chunk_size-1;

    size_t chunk_index(uint32_t chunk_x, uint32_t chunk_y){
        return chunk_x * (chunk_height - 1 + chunk_leftovers_y) + chunk_y;
    }
    
    uint8_t* recieve_corner(uint32_t chunk_x, uint32_t chunk_y, uint32_t corner){
        return field.data() + chunk_index(chunk_x, chunk_y) + corner;
    }

    uint8_t* recieve_edge(uint32_t chunk_x, uint32_t chunk_y, uint32_t row, uint32_t edge){
        return field.data() + chunk_index(chunk_x, chunk_y) + row * chunk_width + edge;
    }


    void load_render_chunk_regular(uint32_t chunk_x, uint32_t chunk_y, uint32_t &offset){
        //bottom row and corner of chunks above
        render_buffer[offset++] = *recieve_corner(chunk_x-1, chunk_y+1, BOTTOM_RIGHT_CORNER);
        std::memcpy(render_buffer.data() + offset, recieve_corner(chunk_x, chunk_y+1, BOTTOM_LEFT_CORNER), chunk_width);
        offset += chunk_width + gpu_padding;
        render_buffer[offset++] = *recieve_corner(chunk_x+1, chunk_y+1, BOTTOM_LEFT_CORNER);
        offset += gpu_padding;

        for(uint32_t row = 0; row < chunk_height; ++row){
            render_buffer[offset++] = *recieve_edge(chunk_x-1, chunk_y, row, RIGHT_EDGE);
            std::memcpy(render_buffer.data() + offset, recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), chunk_width);
            offset += chunk_width;
            render_buffer[offset++] = *recieve_edge(chunk_x-1, chunk_y, row, LEFT_EDGE);
            offset += gpu_padding;
        }

        //top row and corners of chunks below
        render_buffer[offset++] = *recieve_corner(chunk_x-1, chunk_y-1, TOP_RIGHT_CORNER);
        std::memcpy(render_buffer.data() + offset, recieve_corner(chunk_x, chunk_y-1, TOP_LEFT_CORNER), chunk_width);
        offset += chunk_width;
        render_buffer[offset++] = *recieve_corner(chunk_x+1, chunk_y-1, TOP_LEFT_CORNER);
        offset += gpu_padding;
    }

    void load_render_chunk_top(uint32_t chunk_x, uint32_t chunk_y, uint32_t &offset){
        std::memset(render_buffer.data() + offset, reveal_value, chunk_width + 2);
        offset += chunk_width + 2 + gpu_padding;

        for(uint32_t row = 0; row < chunk_height; ++row){
            render_buffer[offset++] = *recieve_edge(chunk_x-1, chunk_y, row, RIGHT_EDGE);
            std::memcpy(render_buffer.data() + offset, recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), chunk_width);
            offset += chunk_width;
            render_buffer[offset++] = *recieve_edge(chunk_x-1, chunk_y, row, LEFT_EDGE);
            offset += gpu_padding;
        }

        //top row and corners of chunks below
        render_buffer[offset++] = *recieve_corner(chunk_x-1, chunk_y-1, TOP_RIGHT_CORNER);
        std::memcpy(render_buffer.data() + offset, recieve_corner(chunk_x, chunk_y-1, TOP_LEFT_CORNER), chunk_width);
        offset += chunk_width;
        render_buffer[offset++] = *recieve_corner(chunk_x+1, chunk_y-1, TOP_LEFT_CORNER);
        offset += gpu_padding;
    }

    void load_render_chunk_left(uint32_t chunk_x, uint32_t chunk_y, uint32_t &offset,  Edge edge){
        if(edge == Edge::TOP){
            std::memset(render_buffer.data() + offset, reveal_value, chunk_width + 2);
            offset += chunk_width + 2 + gpu_padding;
        }else{
            render_buffer[offset++] = reveal_value;
            std::memcpy(render_buffer.data() + offset, recieve_corner(chunk_x, chunk_y+1, BOTTOM_LEFT_CORNER), chunk_width);
            offset += chunk_width + gpu_padding;
            render_buffer[offset++] = *recieve_corner(chunk_x+1, chunk_y+1, BOTTOM_LEFT_CORNER);
            offset += gpu_padding;
        }
        
        for(uint32_t row = 0; row < chunk_height; ++row){
            render_buffer[offset++] = reveal_value;
            std::memcpy(render_buffer.data() + offset, recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), chunk_width);
            offset += chunk_width;
            render_buffer[offset++] = *recieve_edge(chunk_x-1, chunk_y, row, LEFT_EDGE);
            offset += gpu_padding;
        }

        render_buffer[offset++] = reveal_value;
        std::memcpy(render_buffer.data() + offset, recieve_corner(chunk_x, chunk_y+1, TOP_RIGHT_CORNER), chunk_width);
        offset += chunk_width;
        render_buffer[offset++] = *recieve_corner(chunk_x+1, chunk_y-1, TOP_RIGHT_CORNER);
        offset += gpu_padding;

    }

    void load_render_chunk_right(uint32_t chunk_x, uint32_t chunk_y, uint32_t &offset, Edge edge){

        if (edge == Edge::TOP){
            std::memset(render_buffer.data() + offset, reveal_value, chunk_width + 2);
            offset += chunk_width + 2 + gpu_padding;
        }else{
            render_buffer[offset++] = *recieve_corner(chunk_x-1, chunk_y+1, BOTTOM_RIGHT_CORNER);
            std::memcpy(render_buffer.data() + offset, recieve_corner(chunk_x, chunk_y+1, BOTTOM_LEFT_CORNER), chunk_leftovers_x);
            offset += chunk_leftovers_x;
            std::memset(render_buffer.data() + offset, reveal_value, chunk_width - chunk_leftovers_x + 1);
            offset += gpu_padding + chunk_size-chunk_leftovers_x + 1;
        }

        for(uint32_t row = 0; row < chunk_height; ++row){
            render_buffer[offset++] = *recieve_edge(chunk_x-1, chunk_y, row, RIGHT_EDGE);
            std::memcpy(render_buffer.data() + offset, recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), chunk_leftovers_x);
            offset += chunk_leftovers_x;
            std::memset(render_buffer.data() + offset, reveal_value, chunk_width - chunk_leftovers_x + 1);
            offset += gpu_padding + chunk_size - chunk_leftovers_x + 1;
        }

        render_buffer[offset++] = *recieve_corner(chunk_x-1, chunk_y-1, TOP_RIGHT_CORNER);
        std::memcpy(render_buffer.data() + offset, recieve_corner(chunk_x, chunk_y-1, TOP_LEFT_CORNER), chunk_leftovers_x);
        offset += chunk_leftovers_x;
        std::memset(render_buffer.data() + offset, reveal_value, chunk_width - chunk_leftovers_x + 1);
        offset += gpu_padding + chunk_size-chunk_leftovers_x + 1;

    }

    void load_render_chunk_bottom(uint32_t chunk_x, uint32_t chunk_y, uint32_t &offset){
        render_buffer[offset++] = *recieve_corner(chunk_x-1, chunk_y+1, BOTTOM_RIGHT_CORNER);
        std::memcpy(render_buffer.data() + offset, recieve_corner(chunk_x, chunk_y+1, BOTTOM_LEFT_CORNER), chunk_width);
        offset += chunk_width + gpu_padding;
        render_buffer[offset++] = *recieve_corner(chunk_x+1, chunk_y+1, BOTTOM_LEFT_CORNER);
        offset += gpu_padding;

        for(uint32_t row = 0; row < chunk_leftovers_y; ++row){
            render_buffer[offset++] = *recieve_edge(chunk_x-1, chunk_y, row, RIGHT_EDGE);
            std::memcpy(render_buffer.data() + offset, recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), chunk_width);
            offset += chunk_width;
            render_buffer[offset++] = *recieve_edge(chunk_x-1, chunk_y, row, LEFT_EDGE);
            offset += gpu_padding;
        }

        std::memset(render_buffer.data() + offset, reveal_value , (chunk_height-chunk_leftovers_y) * (chunk_width + 2 + gpu_padding));
        offset += (chunk_height-chunk_leftovers_y) * (chunk_width + 2 + gpu_padding);
    }

    void load_render_chunk_bottom_left(uint32_t chunk_x, uint32_t chunk_y, uint32_t &offset){
        render_buffer[offset++] = reveal_value;
        std::memcpy(render_buffer.data() + offset, recieve_corner(chunk_x, chunk_y+1, BOTTOM_LEFT_CORNER), chunk_width);
        offset += chunk_width + gpu_padding;
        render_buffer[offset++] = *recieve_corner(chunk_x+1, chunk_y+1, BOTTOM_LEFT_CORNER);
        offset += gpu_padding;

        for(uint32_t row = 0; row < chunk_leftovers_y; ++row){
            render_buffer[offset++] = reveal_value;
            std::memcpy(render_buffer.data() + offset, recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), chunk_width);
            offset += chunk_width;
            render_buffer[offset++] = *recieve_edge(chunk_x-1, chunk_y, row, LEFT_EDGE);
            offset += gpu_padding;
        }

        std::memset(render_buffer.data() + offset, reveal_value , (chunk_height-chunk_leftovers_y) * (chunk_width + 2 + gpu_padding));
        offset += (chunk_height-chunk_leftovers_y) * (chunk_width + 2 + gpu_padding);
        
    }

    void load_render_chunk_bottom_right(uint32_t chunk_x, uint32_t chunk_y, uint32_t &offset){
        render_buffer[offset++] = *recieve_corner(chunk_x-1, chunk_y+1, BOTTOM_RIGHT_CORNER);
        std::memcpy(render_buffer.data() + offset, recieve_corner(chunk_x, chunk_y+1, BOTTOM_LEFT_CORNER), chunk_leftovers_x);
        offset += chunk_leftovers_x;
        std::memset(render_buffer.data() + offset, reveal_value, chunk_width - chunk_leftovers_x + 1);
        offset += gpu_padding + chunk_size-chunk_leftovers_x + 1;

        for(uint32_t row = 0; row < chunk_leftovers_y; ++row){
            render_buffer[offset++] = *recieve_edge(chunk_x-1, chunk_y, row, RIGHT_EDGE);
            std::memcpy(render_buffer.data() + offset, recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), chunk_leftovers_x);
            offset += chunk_leftovers_x;
        }

        std::memset(render_buffer.data() + offset, reveal_value , (chunk_height-chunk_leftovers_y) * (chunk_width + 2 + gpu_padding) + (chunk_width - chunk_leftovers_x + 1));
        offset += (chunk_height-chunk_leftovers_y) * (chunk_width + 2 + gpu_padding);
        offset += gpu_padding + chunk_size - chunk_leftovers_x + 1;
    }

    public:
    size_t width;
    size_t height;

    uint32_t width_chunks; 
    uint32_t height_chunks;
    
    float density;

    MineField() = default;

    MineField(size_t w, size_t h, float density, uint32_t chunk_size_cells, uint32_t window_width, uint32_t window_height){
        if(density < 0.5){
            generate_empty(w, h);
        }else{
            generate_full(w, h);
        }
        chunk_size = chunk_size_cells/2;
        gpu_padding = (chunk_size + 2) % 4;
        uint32_t screen_chunk_count = (window_width/chunk_size_cells + 2 ) * (window_height/chunk_size_cells + 2);
        render_chunk_bytes = (chunk_size + 2 + gpu_padding) * (chunk_size + 2);
        size_t render_buffer_size = screen_chunk_count * render_chunk_bytes;
        render_buffer= RenderBuffer(render_buffer_size, chunk_size, gpu_padding);

        
        TOP_RIGHT_CORNER = chunk_size-1;
        BOTTOM_LEFT_CORNER = (chunk_size-1) * chunk_size;
        BOTTOM_RIGHT_CORNER = chunk_size * chunk_size -1;
        RIGHT_EDGE = chunk_size-1;
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
        height_chunks = height/chunk_size + 1;
        chunk_leftovers_y = height % chunk_size;
        
        width_chunks = width/chunk_size + 1;
        chunk_leftovers_x = width % chunk_size;

        

        //genereate_blank_chunk();

    }
    void generate_empty(size_t w, size_t h){
        field = std::vector<uint8_t>(w*h, 0);
        width = w;
        height = h;

        height_chunks = height/chunk_size + 1;
        chunk_leftovers_y = height % chunk_size;
        
        width_chunks = width/chunk_size + 1;
        chunk_leftovers_x = width % chunk_size;

        //genereate_blank_chunk();
    }
    uint8_t* data(size_t offset){
        return field.data() + offset;
    }
    uint8_t* recieve_chunk(UVec2 chunk_index){
        size_t offset = chunk_index.x * (chunk_height - 1 + chunk_leftovers_y) + chunk_index.y;
        return data(offset);
    }
    uint8_t recieve_data(UVec2 chunk_index, UVec2 internal_index){
        return *(recieve_chunk(chunk_index) + internal_index.y * chunk_size + internal_index.x);
    }

    
    void construct_render_buffer(){
        uint32_t offset = 0;
        for (uint32_t i = 0; i < render_buffer.chunk_indicies.size(); ++i){
            uint32_t chunk_x = render_buffer.chunk_indicies[i];
            uint32_t chunk_y = render_buffer.chunk_indicies[i+1];

            Edge edge_x = chunk_x == 0 ? Edge::LEFT : chunk_x == width_chunks ? Edge::RIGHT : Edge::NONE;
            Edge edge_y = chunk_y == 0 ? Edge::BOTTOM : chunk_y == height_chunks ? Edge::TOP : Edge::NONE;

            if(edge_x == Edge::NONE && edge_y == Edge::NONE){
                load_render_chunk_regular(chunk_x, chunk_y, offset);
            }else if(edge_y == Edge::BOTTOM){
                if(edge_x == Edge::NONE){
                    load_render_chunk_bottom(chunk_x, chunk_y, offset);
                }else if(edge_x == Edge::LEFT){
                    load_render_chunk_bottom_left(chunk_x, chunk_y, offset);
                }else{
                    load_render_chunk_bottom_right(chunk_x, chunk_y, offset);   
                }
            }else if(edge_x == Edge::LEFT){
                load_render_chunk_left(chunk_x, chunk_y, offset, edge_y);
            }else if(edge_x == Edge::RIGHT){
                load_render_chunk_right(chunk_x, chunk_y, offset, edge_y);
            }else{
                load_render_chunk_top(chunk_x, chunk_y, offset);
            }

        }

    }
    
};

void start_generating_minefield(MineField &minefield, int density, size_t width, size_t height, ThreadInfoArray &info, std::atomic<GenProgress> &generating_indicator);