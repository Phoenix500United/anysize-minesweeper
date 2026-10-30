#pragma once
#include "matrix.hpp"
#include <vector>
#include <cstdint>
#include <thread>
#include <atomic>
#include <memory>
#include <cmath>
#include <span>
#include <cstring>
#include <iostream>

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


const uint8_t reveal_value = 
    (to_underlying(CellFlag::Revealed) << 6) |
    (to_underlying(CellFlag::Revealed) << 4) | 
    (to_underlying(CellFlag::Revealed) << 2) |
    (to_underlying(CellFlag::Revealed));

const uint8_t bombValue = 
    (to_underlying(CellFlag::Bomb) << 6) |
    (to_underlying(CellFlag::Bomb) << 4) | 
    (to_underlying(CellFlag::Bomb) << 2) |
    (to_underlying(CellFlag::Bomb));

class MineField
{   
    std::unique_ptr<uint8_t[]> field;

    public:
    size_t size;

    size_t width_cells;
    size_t height_cells;

    size_t width_bytes;
    size_t height_bytes;

    size_t chunk_leftovers_y;
    size_t chunk_leftovers_x;

    size_t chunk_width; 
    size_t chunk_height;

    size_t chunk_width_bytes;
    size_t chunk_height_bytes;

    size_t width_chunks; 
    size_t height_chunks;
    
    float density;

    MineField() = default;

    MineField(size_t w, size_t h, float _density){
        uint8_t odd_x = w % 2;
        uint8_t odd_y = h % 2;
        width_bytes = w/2 + odd_x;
        height_bytes = h/2 + odd_y;
        density = _density;
        std::cout << "DENSITY: " << density << std::endl;
        size = w*h;

        uint8_t value;
        if(density < 0.5){
            generate_empty();
            value = 0;
        }else{
            generate_full();
            value = bombValue;
        }
        if(odd_x & odd_y){
            propagate_odd_values_both(value);
        }
        if(odd_y){
            propagate_odd_values_y(value);
        }
        if(odd_x){
            propagate_odd_values_x(value);
        }


    }
    size_t chunk_index(uint32_t chunk_x, uint32_t chunk_y){
        return chunk_x * (chunk_height - 1 + chunk_leftovers_y) + chunk_y;
    }
    
    uint8_t* recieve_corner(uint32_t chunk_x, uint32_t chunk_y, uint32_t corner){
        return field.get() + chunk_index(chunk_x, chunk_y) + corner;
    }

    uint8_t* recieve_edge(uint32_t chunk_x, uint32_t chunk_y, uint32_t row, uint32_t edge){
        return field.get() + chunk_index(chunk_x, chunk_y) + row * chunk_width + edge;
    }
    void propagate_odd_values_both(uint8_t value){

        //sets x
        value &= (to_underlying(CellFlag::Revealed) << 4) | to_underlying(CellFlag::Revealed);
        for(uint32_t i = 1; i < width_bytes - 1; ++i){
            field[width_bytes * i - 1] = value;
        }

        //sets y
        value &= (to_underlying(CellFlag::Revealed) << 2) | to_underlying(CellFlag::Revealed);
        std::memset(field.get(), value,  width_bytes - 1);

        //probably isn't needed but sets corner value
        field[width_bytes-1] = reveal_value;
    }
    void propagate_odd_values_x(uint8_t value){
        //Have to insert a value at the end of each row
        value &= (to_underlying(CellFlag::Revealed) << 4) | to_underlying(CellFlag::Revealed);
        for(uint32_t i = 1; i < width_bytes; ++i){
            field[width_bytes * i - 1] = value;
        }
    }
    void propagate_odd_values_y(uint8_t value){
        value &= (to_underlying(CellFlag::Revealed) << 2) | to_underlying(CellFlag::Revealed);
        std::memset(field.get(), value,  width_bytes);
    }
    void generate_full(){
        field = std::make_unique<uint8_t[]>(width_bytes*height_bytes);
        std::memset(field.get(), bombValue,  width_bytes*height_bytes);
    }

    void generate_empty(){
        field = std::make_unique<uint8_t[]>(width_bytes*height_bytes);
        std::memset(field.get(), 0,  width_bytes*height_bytes);
    }
    void initilize_chunk_interpretation(uint32_t _chunk_width, uint32_t _chunk_height){
        chunk_width = _chunk_width;
        chunk_height = _chunk_height;

        chunk_width_bytes = chunk_width / 2; 
        chunk_height_bytes = chunk_height / 2;

        width_chunks = width_bytes / chunk_width_bytes;
        height_chunks = height_bytes / chunk_height_bytes;

        chunk_leftovers_x = width_bytes % chunk_width_bytes;
        chunk_leftovers_y = height_bytes % chunk_height_bytes;
    }

    uint8_t* data(size_t offset){
        return field.get() + offset;
    }
    uint8_t* recieve_chunk(uint32_t chunk_x, uint32_t chunk_y){
        size_t offset = chunk_x * (chunk_height - 1 + chunk_leftovers_y) + chunk_y;
        return data(offset);
    }
    uint8_t recieve_data(uint32_t chunk_x, uint32_t chunk_y, uint32_t internal_x, uint32_t internal_y){
        return *(recieve_chunk(chunk_x, chunk_y) + internal_y  * chunk_width + internal_x);
    }

    
};


class RenderBuffer {
    //Parent buffer
    std::unique_ptr<uint8_t[]> render_buffer;

    //Points to where the compute shader
    //draws the unpacked chunk to 
    int32_t* layer_buffer;

    //Contains the current upload of this frame    
    uint8_t* chunk_buffer;
    size_t chunk_buffer_size; 

    //Tells where to render the chunks on the screen
    Vec2<uint32_t>* chunk_locations; 
    //Small camera shift internal to a chunk
    Vec2f* _camera; 

    size_t gpu_padding;

    //Variables used for indexing 
    const uint32_t TOP_LEFT_CORNER = 0;    
    uint32_t TOP_RIGHT_CORNER;
    uint32_t BOTTOM_LEFT_CORNER;
    uint32_t BOTTOM_RIGHT_CORNER;

    const uint32_t LEFT_EDGE = 0;
    uint32_t RIGHT_EDGE;
    
    public:
    size_t layer_index;

    size_t screen_chunk_width;
    size_t screen_chunk_height;
    size_t screen_chunk_count;

    size_t render_buffer_size;
    size_t render_meta_data_size;
    size_t output_layer_buffer_size;
    size_t camera_offset;
    size_t location_buffer_size;

    void construct_render_buffer(MineField &mf, size_t ssbo_alignment, uint32_t window_width, uint32_t window_height){
        screen_chunk_width = (window_width / mf.chunk_width) + 2;
        screen_chunk_height = (window_height / mf.chunk_height) + 2;
        screen_chunk_count = screen_chunk_width * screen_chunk_height;
        
        output_layer_buffer_size = sizeof(int) * screen_chunk_count; 
        camera_offset = (output_layer_buffer_size + ssbo_alignment - 1) / ssbo_alignment * ssbo_alignment;
        location_buffer_size = sizeof(Vec2f) + screen_chunk_count * sizeof(Vec2<uint32_t>);
        size_t chunk_locations_offset = camera_offset + sizeof(Vec2f);
        size_t chunk_buffer_offset = chunk_locations_offset + screen_chunk_count * sizeof(Vec2<uint32_t>);

        

        TOP_RIGHT_CORNER = mf.chunk_width_bytes-1;
        BOTTOM_LEFT_CORNER = (mf.chunk_width_bytes-1) * mf.chunk_height_bytes;
        BOTTOM_RIGHT_CORNER = mf.chunk_width_bytes * mf.chunk_height_bytes -1;

        RIGHT_EDGE = mf.chunk_width - 1;

        //For aligning the chunk texture buffer to 2 bytes 
        gpu_padding = (mf.chunk_width_bytes + 2) % 2;

        render_meta_data_size = chunk_buffer_offset;
        render_buffer_size = ((mf.chunk_width_bytes + 2 + gpu_padding) * (mf.chunk_height_bytes + 2)) * screen_chunk_count + chunk_buffer_offset;
        render_buffer = std::make_unique<uint8_t[]>(render_buffer_size);

        
        layer_buffer = reinterpret_cast<int*>(render_buffer.get());
        _camera = reinterpret_cast<Vec2f*>(render_buffer.get() + camera_offset);
        chunk_locations = reinterpret_cast<Vec2<uint32_t>*>(render_buffer.get() + chunk_locations_offset);
        chunk_buffer = render_buffer.get() + chunk_buffer_offset;

        layer_index = 0;
        chunk_buffer_size = 0;
    }
    uint8_t* get_chunk_buffer(){
        return chunk_buffer;
    }
    int32_t* get_layer_buffer(){
        return layer_buffer;
    }
    Vec2<uint32_t>* get_chunk_locations(){
        return chunk_locations;
    }
    Vec2f& camera(){
        return *_camera;
    }
    void chunk_buffer_clear(){
        chunk_buffer_size = 0;
    }

    void load_render_chunk_regular(MineField &mf, uint32_t chunk_x, uint32_t chunk_y){
        
        //Copies a chunk and 1 byte of padding of the neighbouring chunks into the render buffer

        //Copies bottom right corner of the chunk to the upper left
        chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x-1, chunk_y+1, BOTTOM_RIGHT_CORNER);
        
        //Copies the bottom row of the chunk above
        std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_corner(chunk_x, chunk_y+1, BOTTOM_LEFT_CORNER), mf.chunk_width_bytes);
        chunk_buffer_size += mf.chunk_width_bytes;
    
        //Copies teh bottom left byte of the chunk to the upper right 
        chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x+1, chunk_y+1, BOTTOM_LEFT_CORNER);
        chunk_buffer_size += gpu_padding;

        for(uint32_t row = 0; row < mf.chunk_height_bytes; ++row){
            //copies the right most byte of the current row in the chunk to the left 
            chunk_buffer[chunk_buffer_size++] = *mf.recieve_edge(chunk_x-1, chunk_y, row, RIGHT_EDGE);
            
            //copies the row of the current chunk 
            std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), mf.chunk_width_bytes);
            chunk_buffer_size += mf.chunk_width_bytes;
            
            //copies the left most byte of the current row in the chunk to the right
            chunk_buffer[chunk_buffer_size++] = *mf.recieve_edge(chunk_x+1, chunk_y, row, LEFT_EDGE);
            chunk_buffer_size += gpu_padding;
        }

        //copies the top right corner of the chunk to the bottom left
        chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x-1, chunk_y-1, TOP_RIGHT_CORNER);
        
        //copies the top row of the chunk below
        std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_corner(chunk_x, chunk_y-1, TOP_LEFT_CORNER), mf.chunk_width_bytes);
        chunk_buffer_size += mf.chunk_width_bytes;
        
        //copies the top left corner of the chunk ot the bottom right 
        chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x+1, chunk_y-1, TOP_LEFT_CORNER);
        chunk_buffer_size += gpu_padding;
    }

    void load_render_chunk_top(MineField &mf, uint32_t chunk_x, uint32_t chunk_y){
        std::memset(chunk_buffer + chunk_buffer_size, reveal_value, mf.chunk_width_bytes + 2);
        chunk_buffer_size += mf.chunk_width_bytes + 2 + gpu_padding;

        for(uint32_t row = 0; row < mf.chunk_height_bytes; ++row){
            chunk_buffer[chunk_buffer_size++] = *mf.recieve_edge(chunk_x-1, chunk_y, row, RIGHT_EDGE);
            std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), mf.chunk_width_bytes);
            chunk_buffer_size += mf.chunk_width_bytes;
            chunk_buffer[chunk_buffer_size++] = *mf.recieve_edge(chunk_x+1, chunk_y, row, LEFT_EDGE);
            chunk_buffer_size += gpu_padding;
        }

        //top row and corners of chunks below
        chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x-1, chunk_y-1, TOP_RIGHT_CORNER);
        std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_corner(chunk_x, chunk_y-1, TOP_LEFT_CORNER), mf.chunk_width_bytes);
        chunk_buffer_size += mf.chunk_width_bytes;
        chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x+1, chunk_y-1, TOP_LEFT_CORNER);
        chunk_buffer_size += gpu_padding;
    }

    void load_render_chunk_left(MineField &mf, uint32_t chunk_x, uint32_t chunk_y,  Edge edge){
        if(edge == Edge::TOP){
            std::memset(chunk_buffer + chunk_buffer_size, reveal_value, mf.chunk_width_bytes + 2);
            chunk_buffer_size += mf.chunk_width_bytes + 2 + gpu_padding;
        }else{
            chunk_buffer[chunk_buffer_size++] = reveal_value;
            std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_corner(chunk_x, chunk_y+1, BOTTOM_LEFT_CORNER), mf.chunk_width_bytes);
            chunk_buffer_size += mf.chunk_width_bytes;
            chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x+1, chunk_y+1, BOTTOM_LEFT_CORNER);
            chunk_buffer_size += gpu_padding;
        }
        
        for(uint32_t row = 0; row < mf.chunk_height_bytes; ++row){
            chunk_buffer[chunk_buffer_size++] = reveal_value;
            std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), mf.chunk_width_bytes);
            chunk_buffer_size += mf.chunk_width_bytes;
            chunk_buffer[chunk_buffer_size++] = *mf.recieve_edge(chunk_x+1, chunk_y, row, LEFT_EDGE);
            chunk_buffer_size += gpu_padding;
        }

        chunk_buffer[chunk_buffer_size++] = reveal_value;
        std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_corner(chunk_x, chunk_y-1, TOP_LEFT_CORNER), mf.chunk_width_bytes);
        chunk_buffer_size += mf.chunk_width_bytes;
        chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x+1, chunk_y-1, TOP_LEFT_CORNER);
        chunk_buffer_size += gpu_padding;

    }

    void load_render_chunk_right(MineField &mf, uint32_t chunk_x, uint32_t chunk_y, Edge edge){

        if (edge == Edge::TOP){
            std::memset(chunk_buffer + chunk_buffer_size, reveal_value, mf.chunk_width_bytes + 2);
            chunk_buffer_size += mf.chunk_width_bytes + 2 + gpu_padding;
        }else{
            chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x-1, chunk_y+1, BOTTOM_RIGHT_CORNER);
            std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_corner(chunk_x, chunk_y+1, BOTTOM_LEFT_CORNER), mf.chunk_leftovers_x);
            chunk_buffer_size += mf.chunk_leftovers_x;
            std::memset(chunk_buffer + chunk_buffer_size, reveal_value, mf.chunk_width_bytes - mf.chunk_leftovers_x + 1);
            chunk_buffer_size += gpu_padding + mf.chunk_width_bytes- mf.chunk_leftovers_x + 1;
        }

        for(uint32_t row = 0; row < mf.chunk_height_bytes; ++row){
            chunk_buffer[chunk_buffer_size++] = *mf.recieve_edge(chunk_x-1, chunk_y, row, RIGHT_EDGE);
            std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), mf.chunk_leftovers_x);
            chunk_buffer_size += mf.chunk_leftovers_x;
            std::memset(chunk_buffer + chunk_buffer_size, reveal_value, mf.chunk_width_bytes - mf.chunk_leftovers_x + 1);
            chunk_buffer_size += gpu_padding + mf.chunk_width_bytes - mf.chunk_leftovers_x + 1;
        }

        chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x-1, chunk_y-1, TOP_RIGHT_CORNER);
        std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_corner(chunk_x, chunk_y-1, TOP_LEFT_CORNER), mf.chunk_leftovers_x);
        chunk_buffer_size += mf.chunk_leftovers_x;
        std::memset(chunk_buffer + chunk_buffer_size, reveal_value, mf.chunk_width_bytes - mf.chunk_leftovers_x + 1);
        chunk_buffer_size += gpu_padding + mf.chunk_width_bytes - mf.chunk_leftovers_x + 1;

    }

    void load_render_chunk_bottom(MineField &mf, uint32_t chunk_x, uint32_t chunk_y){
        chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x-1, chunk_y+1, BOTTOM_RIGHT_CORNER);
        std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_corner(chunk_x, chunk_y+1, BOTTOM_LEFT_CORNER), mf.chunk_width_bytes);
        chunk_buffer_size += mf.chunk_width_bytes;
        chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x+1, chunk_y+1, BOTTOM_LEFT_CORNER);
        chunk_buffer_size += gpu_padding;

        for(uint32_t row = 0; row < mf.chunk_leftovers_y; ++row){
            chunk_buffer[chunk_buffer_size++] = *mf.recieve_edge(chunk_x-1, chunk_y, row, RIGHT_EDGE);
            std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), mf.chunk_width_bytes);
            chunk_buffer_size += mf.chunk_width_bytes;
            chunk_buffer[chunk_buffer_size++] = *mf.recieve_edge(chunk_x+1, chunk_y, row, LEFT_EDGE);
            chunk_buffer_size += gpu_padding;
        }

        std::memset(chunk_buffer + chunk_buffer_size, reveal_value , (mf.chunk_height_bytes - mf.chunk_leftovers_y + 1) * (mf.chunk_width_bytes + 2 + gpu_padding));
        chunk_buffer_size += (mf.chunk_height_bytes - mf.chunk_leftovers_y + 1) * (mf.chunk_width_bytes + 2 + gpu_padding);
    }

    void load_render_chunk_bottom_left(MineField &mf, uint32_t chunk_x, uint32_t chunk_y){
        chunk_buffer[chunk_buffer_size++] = reveal_value;
        std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_corner(chunk_x, chunk_y+1, BOTTOM_LEFT_CORNER), mf.chunk_width_bytes);
        chunk_buffer_size += mf.chunk_width_bytes;
        chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x+1, chunk_y+1, BOTTOM_LEFT_CORNER);
        chunk_buffer_size += gpu_padding;

        for(uint32_t row = 0; row < mf.chunk_leftovers_y; ++row){
            chunk_buffer[chunk_buffer_size++] = reveal_value;
            std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), mf.chunk_width_bytes);
            chunk_buffer_size += mf.chunk_width_bytes;
            chunk_buffer[chunk_buffer_size++] = *mf.recieve_edge(chunk_x+1, chunk_y, row, LEFT_EDGE);
            chunk_buffer_size += gpu_padding;
        }

        std::memset(chunk_buffer + chunk_buffer_size, reveal_value , (mf.chunk_height_bytes-mf.chunk_leftovers_y + 1) * (mf.chunk_width_bytes + 2 + gpu_padding));
        chunk_buffer_size += (mf.chunk_height_bytes - mf.chunk_leftovers_y + 1) * (mf.chunk_width_bytes + 2 + gpu_padding);
        
    }

    void load_render_chunk_bottom_right(MineField &mf, uint32_t chunk_x, uint32_t chunk_y){
        chunk_buffer[chunk_buffer_size++] = *mf.recieve_corner(chunk_x-1, chunk_y+1, BOTTOM_RIGHT_CORNER);
        std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_corner(chunk_x, chunk_y+1, BOTTOM_LEFT_CORNER), mf.chunk_leftovers_x);
        chunk_buffer_size += mf.chunk_leftovers_x;
        std::memset(chunk_buffer + chunk_buffer_size, reveal_value, mf.chunk_width_bytes - mf.chunk_leftovers_x + 1);
        chunk_buffer_size += gpu_padding + mf.chunk_width_bytes - mf.chunk_leftovers_x + 1;

        for(uint32_t row = 0; row < mf.chunk_leftovers_y; ++row){
            chunk_buffer[chunk_buffer_size++] = *mf.recieve_edge(chunk_x-1, chunk_y, row, RIGHT_EDGE);
            std::memcpy(chunk_buffer + chunk_buffer_size, mf.recieve_edge(chunk_x, chunk_y, row, LEFT_EDGE), mf.chunk_leftovers_x);
            chunk_buffer_size += mf.chunk_leftovers_x;
            std::memset(chunk_buffer + chunk_buffer_size, reveal_value, mf.chunk_width_bytes - mf.chunk_leftovers_x + 1);
            chunk_buffer_size += gpu_padding + mf.chunk_width_bytes - mf.chunk_leftovers_x + 1;
        }

        std::memset(chunk_buffer + chunk_buffer_size, reveal_value , (mf.chunk_height_bytes - mf.chunk_leftovers_y + 1) * (mf.chunk_width_bytes + 2 + gpu_padding));
        chunk_buffer_size += (mf.chunk_height_bytes - mf.chunk_leftovers_y + 1) * (mf.chunk_width_bytes + 2 + gpu_padding);
    }

    void add_chunk(MineField &mf, uint32_t chunk_x, uint32_t chunk_y, int location_index, Vec2<uint32_t> location){

        Edge edge_x = chunk_x == 0 ? Edge::LEFT : chunk_x == mf.width_chunks-1 ? Edge::RIGHT : Edge::NONE;
        Edge edge_y = chunk_y == 0 ? Edge::BOTTOM : chunk_y == mf.height_chunks-1 ? Edge::TOP : Edge::NONE;

        if(edge_x == Edge::NONE && edge_y == Edge::NONE){
            //std::cout << "REGULAR_CHUNK\n";
            load_render_chunk_regular(mf, chunk_x, chunk_y);
        }else if(edge_y == Edge::BOTTOM){
            //std::cout << "BOTTOM_CHUNK\n";
            if(edge_x == Edge::NONE){
                load_render_chunk_bottom(mf, chunk_x, chunk_y);
            }else if(edge_x == Edge::LEFT){
                load_render_chunk_bottom_left(mf, chunk_x, chunk_y);
            }else{
                load_render_chunk_bottom_right(mf, chunk_x, chunk_y);   
            }
        }else if(edge_x == Edge::LEFT){
            //std::cout << "LEFT_CHUNK\n";
            load_render_chunk_left(mf, chunk_x, chunk_y, edge_y);
        }else if(edge_x == Edge::RIGHT){
            //std::cout << "RIGHT_CHUNK\n";
            load_render_chunk_right(mf, chunk_x, chunk_y, edge_y);
        }else{
            //std::cout << "TOP_CHUNK\n";
            load_render_chunk_top(mf, chunk_x, chunk_y);
        }
        layer_buffer[layer_index++] = location_index;
        chunk_locations[location_index] = location;
    }
    void* data(){
        return render_buffer.get();
    }
};

void start_generating_minefield(MineField &minefield, ThreadInfoArray &info, std::atomic<GenProgress> &generating_indicator);