#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#define NOEXCEPT noexcept
extern "C" {
#elif 
#define NOEXCEPT
#endif

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef float f32;

#define glTF 0x46546C67
#define JSON 0x4E4F534A
#define BIN 0x004E4942

// Ironic
typedef struct smol_str {
        u8 const *data;
        size_t length;
} smol_str;

inline bool smol_str_is_equal(smol_str stra, char const *const strb) NOEXCEPT {
        for(size_t i = 0;i < stra.length;++i) if(stra.data[i] != strb[i]) return false;
        if(strb[stra.length] != '\0') return false;
        return true;
}

inline bool smol_str_in(smol_str str, char const * const * strs, size_t count) NOEXCEPT{
        for(size_t i = 0; i < count; ++i) if(smol_str_is_equal(str, strs[i])) return true;
        return false;
}


inline s64 smol_str_to_s64(smol_str str) NOEXCEPT{
        if(str.length == 0 || str.data == NULL) return 0;
        if(str.length > 20) return 0;
        s64 value = 0;
        s64 sign = 1;
        if(str.data[0] == '-') sign = -1;
        for(u64 position = 0 + (sign < 0); position < str.length; ++position){
                u64 digit = str.data[position] - '0';
                bool is_valid = digit >= 0 || digit <= 9;
                value = value * 10 + (digit * is_valid);
        }

        return value * sign;
}

inline f32 smol_str_to_f32(smol_str str) NOEXCEPT{
        if(str.length == 0 || str.data == NULL) return 0;
        f32 sign = 1;
        f32 value = 0;
        if(str.data[0] == '-') sign = -1;
        s64 point_index = -1;
        for(size_t i = 0 + (sign < 0); i < str.length; ++i){
                if(str.data[i] == '.'){
                        point_index = i;
                        continue;
                }
                u64 digit = str.data[i] - '0';
                if(digit >= 0 || digit <= 9){
                        value = value * 10.0f + (f32)digit;
                        if(point_index >= 0){
                                f32 decimal = 10;
                                for(size_t b = 1; b < i-point_index; ++b){
                                        decimal *= 10.0f;
                                }
                                value += (f32)digit/decimal;
                        }
                }
        }
        return value * sign;
}

typedef struct smol_asset {
        // in data
        smol_str generator;
        // in data
        // Maybe just a number or enum.
        smol_str version;
} smol_asset;

typedef struct smol_scene {
        smol_str name;
        u32 *nodes;
        u8 node_count;
} smol_scene;

typedef struct smol_node {
        smol_str name;
        u32 mesh;
        u32 skin;
        // f32 translation[3];
        // f32 rotation[4];
        // f32 scale[3];
        f32 matrix[4 * 4];
        u32 *children;
        u8 child_count;
} smol_node;

typedef enum smol_mesh_primitive_attribute_name : u8{
        smol_POSITION,
        smol_NORMAL,
        smol_TANGENT,

        //Require set index.
        smol_TEXCOORD,
        smol_COLOR,
        smol_JOINTS,
        smol_WEIGHTS,
}smol_mesh_primitive_attribute_name;

typedef struct smol_mesh_primitive_attribute{
        u32 accessor;
        smol_mesh_primitive_attribute_name name;
        s8 set_index;
}smol_mesh_primitive_attribute;

inline s8 smol_check_value_is_mesh_primitive_attribute_name_get_set_index(smol_str value, char const * test_str){
        for(size_t i = 0; i < value.length; ++i){
                if(test_str[i] != value.data[i]){
                        if(value.data[i] == '_'){
                                smol_str set_index_str = {.data = value.data + i + 1, .length = value.length - (i+1)};
                                return smol_str_to_s64(set_index_str);
                        }
                        else return -1;
                } 
        }
        return -1;
}

typedef struct smol_mesh_primitive{
        u8 attribute_count;
        smol_mesh_primitive_attribute * attributes;
        u32 indices;
        u32 material;
        u32 mode;
}smol_mesh_primitive;

typedef struct smol_mesh {
        smol_str name;
        smol_mesh_primitive *primitives;
        u8 primitive_count;
} smol_mesh;

typedef enum smol_GLTF_component_type : s8 {
        smol_GLTF_component_type_none = -1,
        smol_GLTF_component_type_s8,
        smol_GLTF_component_type_u8,
        smol_GLTF_component_type_s16,
        smol_GLTF_component_type_u16,
        smol_GLTF_component_type_u32,
        smol_GLTF_component_type_f32,
} smol_GLTF_component_type;

char const * const smol_GLTF_component_type_strings[6] = {"5120","5121","5122","5123","5125","5126"};

//returns -1 if the string is not a valid type code.
inline smol_GLTF_component_type lookup_component_type(smol_str value) NOEXCEPT{
        for(size_t i = 0; i < 6; ++i){
                if(smol_str_is_equal(value, smol_GLTF_component_type_strings[i])) return (smol_GLTF_component_type)i;
        }
        return smol_GLTF_component_type_none;
}

u8 const smol_GLTF_component_type_byte_count[6] = {1,1,2,2,4,4};

typedef enum smol_GLTF_type: s8{
        smol_GLTF_none = -1,
        smol_GLTF_SCALAR,
        smol_GLTF_VEC2,
        smol_GLTF_VEC3,
        smol_GLTF_VEC4,
        smol_GLTF_MAT2,
        smol_GLTF_MAT3,
        smol_GLTF_MAT4,
} smol_GLTF_type;

char const * const smol_GLTF_type_strings[8] = { "SCALAR", "VEC2", "VEC3", "VEC4", "MAT2", "MAT3", "MAT4" };

//returns -1 if the string is not a valid component.
inline s8 lookup_smol_GLTF_component(smol_str str) NOEXCEPT{
        for(size_t i = 0; i < 8; ++i){
                if(smol_str_is_equal(str, smol_GLTF_type_strings[i])) return i;
        }
        return -1;
}

u8 const smol_GLTF_type_component_count[8] = {1,2,3,4,4,9,16};

typedef struct smol_accessor {
        smol_GLTF_component_type component_type;
        smol_GLTF_type type;
        u32 buffer_view;
        u32 byte_offset;
        //must be cast based off the components array;
        void *min_values;
        void *max_values;
        u32 count;
} smol_accessor;

typedef struct smol_buffer_view {
        u32 buffer;
        u32 byte_length;
        u32 byte_offset;
        u32 byte_stride;
        u32 target;
} smol_buffer_view;

typedef struct smol_buffer {
        smol_str uri;
        u64 byte_length;
} smol_buffer;

typedef struct smol_allocator {
        void *(*allocate)(size_t size);
        void (*free)(void *ptr);
} smol_allocator;

typedef struct smol_GLTF {
        smol_allocator allocator;
        smol_asset asset;
        u32 scene;
        smol_scene *scenes;
        smol_node *nodes;
        smol_mesh *meshes;
        smol_accessor * accessors;
        smol_buffer_view *buffer_views;
        smol_buffer *buffers;
        size_t data_length;
        u8 const *data;
        u32 accessor_count;
        u32 scene_count;
        u32 node_count;
        u32 mesh_count;
        u32 buffer_view_count;
        u32 buffer_count;

} smol_GLTF;

typedef struct smol_header {
        u32 magic;
        u32 version;
        u32 length;
} smol_header;

typedef struct smol_chunk {
        u32 length;
        u32 type;
        u8 const *data;
} smol_chunk;

typedef enum smol_symbol : u8 {
        smol_symbol_none = 0,
        smol_symbol_open_square,
        smol_symbol_close_square,
        smol_symbol_open_squigily,
        smol_symbol_close_squigily,
        smol_symbol_begin_string,
        smol_symbol_end_string,
        smol_symbol_colon,
        smol_symbol_comma,
} smol_symbol;

typedef enum smol_token : u8 {
        smol_token_key,
        smol_token_value,
        smol_token_begin_object,
        smol_token_end_object,
        smol_token_begin_array,
        smol_token_end_array,
} smol_tokens;

typedef enum smol_root_object : u8 {
        smol_root_accessors,
        smol_root_asset,
        smol_root_bufferViews,
        smol_root_buffers,
        smol_root_meshes,
        smol_root_nodes,
        smol_root_scene,
        smol_root_scenes,
//Meta feild.
        smol_root_object_count,
} smol_root_object;

 const char * const smol_root_object_names[smol_root_object_count] = {
    "accessors",
    "asset",
    "bufferViews",
    "buffers",
    "meshes",
    "nodes",
    "scene",
    "scenes",
};

inline u64 smol_count_json_symbols(u64 json_size, u8 const *json)  NOEXCEPT{
        u64 token_count = 0;
        for (u8 const *byte = json; byte != json + json_size; ++byte) {
                switch (*byte) {
                case '"':
                case '{':
                case '}':
                case '[':
                case ']':
                case ':':
                case ',':
                        ++token_count;
                default:
                        continue;
                }
        }
        return token_count;
}

inline void smol_parse_json_symbols(u64 json_size, u8 const * const json, u64 symbol_count, smol_symbol *symbols, u8 const **symbol_chars)  NOEXCEPT{
        u8 const **current_symbol_char = symbol_chars;
        smol_symbol *current_symbol = symbols;
        bool in_string = false;

        for (u8 const *byte = json; byte != json + json_size; ++byte) {
                if (*byte == '"') {
                        if (in_string) {
                                in_string = false;
                                *current_symbol = smol_symbol_end_string;
                                goto eat_symbol;
                        } else {
                                in_string = true;
                                *current_symbol = smol_symbol_begin_string;
                                goto eat_symbol;
                        }
                } else if (*byte == '{') {
                        *current_symbol = smol_symbol_open_squigily;
                        goto eat_symbol;
                } else if (*byte == '}') {
                        *current_symbol = smol_symbol_close_squigily;
                        goto eat_symbol;
                } else if (*byte == '[') {
                        *current_symbol = smol_symbol_open_square;
                        goto eat_symbol;
                } else if (*byte == ']') {
                        *current_symbol = smol_symbol_close_square;
                        goto eat_symbol;
                } else if (*byte == ':') {
                        *current_symbol = smol_symbol_colon;
                        goto eat_symbol;
                } else if (*byte == ',') {
                        *current_symbol = smol_symbol_comma;
                        goto eat_symbol;
                } else {
                        continue;
                }
        eat_symbol:
                *current_symbol_char = byte;
                ++current_symbol;
                ++current_symbol_char;
        }
}

typedef enum smol_json_parse_stack_type: u8{
        smol_none,
        smol_object,
        smol_array,
        smol_value_array,
        smol_single_or_empty_array,
}smol_json_parse_stack_type;

inline size_t smol_count_json_tokens(size_t symbol_count, smol_symbol const *const symbols)  NOEXCEPT{
        size_t token_count = 0;
        bool in_value_array = false;
        for (size_t i = 0; i < symbol_count; ++i) {
                smol_symbol symbol = symbols[i];
                if (symbol == smol_symbol_open_squigily)
                        ++token_count;
                else if (symbol == smol_symbol_close_squigily)
                        ++token_count;
                else if (symbol == smol_symbol_open_square){
                        ++token_count;
                        if(symbols[i+1] == smol_symbol_comma || symbols[i+1] == smol_symbol_close_square){
                                ++token_count;
                                in_value_array = true;
                        }
                }
                else if (symbol == smol_symbol_close_square){
                        ++token_count;
                        if(in_value_array) in_value_array = false;
                }
                else if (symbol == smol_symbol_begin_string)
                        ++token_count;
                else if (symbol == smol_symbol_colon) {
                        smol_symbol next_symbol = symbols[i + 1];
                        if (next_symbol == smol_symbol_comma) {
                                ++token_count;
                                ++i;
                        } else if (next_symbol == smol_symbol_close_squigily){
                                ++token_count;
                        } else if (next_symbol == smol_symbol_begin_string)
                                continue;
                }else if(symbol == smol_symbol_comma && in_value_array){
                        ++token_count;
                }
        }
        return token_count;
}


inline void smol_parse_json_tokens(size_t symbol_count, smol_symbol const *const symbols, u8 const *const *const symbol_locations, smol_token *tokens, smol_str *token_values) NOEXCEPT{
        size_t token_index = 0;
        smol_json_parse_stack_type type_stack[UINT8_MAX] = {smol_none}; 
        u8 stack_spot = 0;
        for (size_t i = 0; i < symbol_count; ++i) {
                if (symbols[i] == smol_symbol_open_squigily){
                        tokens[token_index] = smol_token_begin_object;
                        ++stack_spot;
                        type_stack[stack_spot] = smol_object;
                } else if (symbols[i] == smol_symbol_close_squigily){
                        tokens[token_index] = smol_token_end_object;
                        --stack_spot;
                } else if (symbols[i] == smol_symbol_open_square){
                        tokens[token_index] = smol_token_begin_array;
                        ++stack_spot;
                        if(symbols[i+1] == smol_symbol_comma || symbols[i+1] == smol_symbol_close_square){
                                ++token_index;
                                tokens[token_index] = smol_token_value;
                                token_values[token_index].data = symbol_locations[i]+1;
                                token_values[token_index].length = symbol_locations[i+1] - token_values[token_index].data;
                                type_stack[stack_spot] = smol_value_array;
                        } else type_stack[stack_spot] = smol_array;
                } else if (symbols[i] == smol_symbol_close_square){
                        tokens[token_index] = smol_token_end_array;
                        --stack_spot;
                } else if (symbols[i] == smol_symbol_begin_string) {
                        if(type_stack[stack_spot] == smol_object) tokens[token_index] = smol_token_key;
                        else if(type_stack[stack_spot] == smol_array) tokens[token_index] = smol_token_value;
                        token_values[token_index].data = symbol_locations[i] + 1;
                        token_values[token_index].length = symbol_locations[i + 1] - token_values[token_index].data;
                } else if (symbols[i] == smol_symbol_colon) {
                        if (symbols[i+1] == smol_symbol_comma) {
                                tokens[token_index] = smol_token_value;
                                token_values[token_index].data = symbol_locations[i] + 1;
                                token_values[token_index].length = symbol_locations[i + 1] - token_values[token_index].data;
                                ++i;
                        }else if(symbols[i+1] == smol_symbol_close_squigily){
                                tokens[token_index] = smol_token_value;
                                token_values[token_index].data = symbol_locations[i] + 1;
                                token_values[token_index].length = symbol_locations[i + 1] - token_values[token_index].data;
                        } else if (symbols[i+1] == smol_symbol_begin_string) {
                                tokens[token_index] = smol_token_value;
                                token_values[token_index].data = symbol_locations[i + 1] + 1;
                                token_values[token_index].length = symbol_locations[i + 2] - token_values[token_index].data;
                                i += 2;
                        } else continue;
                } else if(symbols[i] == smol_symbol_comma && type_stack[stack_spot] == smol_value_array){
                        token_values[token_index].data = symbol_locations[i]+1;
                        token_values[token_index].length = symbol_locations[i+1] - token_values[token_index].data;
                        tokens[token_index] = smol_token_value;
                } else continue;
                ++token_index;
        }
}

inline size_t smol_count_objects_til_end_of_array(size_t token_count, smol_token const * tokens, size_t token_index)NOEXCEPT{
        size_t object_count = 0;
        s64 nested_object_count = -1;
        do{
                if(tokens[token_index] == smol_token_begin_object){
                        ++nested_object_count;
                }else if(tokens[token_index] == smol_token_end_object){
                        --nested_object_count;
                        if(nested_object_count < 0) ++object_count;
                }else if(tokens[token_index] == smol_token_end_array  && nested_object_count < 0){
                        break;
                }
                ++token_index;
        }while(token_index < token_count);

        return object_count;
}

inline size_t smol_count_tokens_til_end_of_array(size_t token_count, smol_token const * tokens, size_t current_token_index)NOEXCEPT{
        size_t starting_token_index = current_token_index;
        do ++current_token_index; while(tokens[current_token_index] != smol_token_end_array);
        return current_token_index - starting_token_index;
}

//TODO: decrease count and remove object from array by swapping it to the end.
inline s8 smol_parse_next_root_object_key(u8 * root_objects_to_parse_count, smol_root_object * root_objects_to_parse, smol_str token_value)NOEXCEPT{
        if(token_value.length == 0 || token_value.data == NULL) return -1;
        for (u8 root_object_to_parse_index = 0; root_object_to_parse_index < *root_objects_to_parse_count; ++root_object_to_parse_index) {
                if (smol_str_is_equal(token_value, smol_root_object_names[root_objects_to_parse[root_object_to_parse_index]])){
                        smol_root_object root_object = root_objects_to_parse[root_object_to_parse_index];
                        root_objects_to_parse[root_object_to_parse_index] = root_objects_to_parse[(*root_objects_to_parse_count) - 1];
                        --*root_objects_to_parse_count;
                        return root_object;
                } 
        }
        return -1;
}

inline void smol_check_key_is_string_and_assign_value(smol_str const * token_values, size_t * token_index, char const * test_str,smol_str * assign_location)NOEXCEPT{
        if(smol_str_is_equal(token_values[*token_index], test_str)){
                ++*token_index;
                *assign_location = token_values[*token_index];
                ++*token_index;
        }
}

inline void check_key_is_string_and_assign_s64_value(smol_str const * token_values, size_t * token_index, char const * test_str,s64 * assign_location)NOEXCEPT{
        if(smol_str_is_equal(token_values[*token_index], test_str)){
                ++*token_index;
                *assign_location = smol_str_to_s64(token_values[*token_index]);
                ++*token_index;
        }
}

inline void smol_check_key_is_string_and_assign_u64_value(smol_str const * token_values, size_t * token_index, char const * test_str,u64 * assign_location)NOEXCEPT{
        if(smol_str_is_equal(token_values[*token_index], test_str)){
                ++*token_index;
                //TODO: write an unsigned version.
                *assign_location = (u32)smol_str_to_s64(token_values[*token_index]);
                ++*token_index;
        }
}

inline void smol_check_key_is_string_and_assign_u32_value(smol_str const * token_values, size_t * token_index, char const * test_str,u32 * assign_location)NOEXCEPT{
        if(smol_str_is_equal(token_values[*token_index], test_str)){
                ++*token_index;
                //TODO: write an unsigned version.
                *assign_location = (u32)smol_str_to_s64(token_values[*token_index]);
                ++*token_index;
        }
}

inline void smol_check_attribute_name_and_assign_accessor_index_and_set_index(smol_str const * token_values, size_t * token_index, char const * test_str, smol_mesh_primitive_attribute * attribute_location){
        s8 set_index = -1;
        for(size_t i = 0;i < token_values[*token_index].length;++i){
                if(token_values[*token_index].data[i] != test_str[i]){
                        if(token_values[*token_index].data[i] == '_'){
                                set_index = smol_str_to_s64({.data = token_values[*token_index].data + i + 1, .length = token_values[*token_index].length - (i + 1)});
                        }
                }
        }
        if(set_index < 0) return;
        ++*token_index;
        u32 accessor = (u32)smol_str_to_s64(token_values[*token_index]);
        ++*token_index;
        attribute_location->set_index = set_index;
        
}

inline bool smol_parse_GLTF(u32 raw_gltf_size, u8 const *raw_gltf_data, smol_GLTF *gltf, smol_allocator allocator) NOEXCEPT{
        if (allocator.allocate && allocator.free) gltf->allocator = allocator; 
        // Allocator is required right now.
        else return false;

        if (raw_gltf_size < 20) return false;

        smol_header header;
        smol_chunk json_chunk;
        smol_chunk binary_chunk;

        memcpy(&header.magic, raw_gltf_data, 4);
        if (header.magic != glTF) return false;
        memcpy(&header.version, raw_gltf_data + 4, 4);
        memcpy(&header.length, raw_gltf_data + 8, 4);
        memcpy(&json_chunk.length, raw_gltf_data + 12, 4);
        memcpy(&json_chunk.type, raw_gltf_data + 16, 4);
        if (json_chunk.type != JSON) return false;
        json_chunk.data = raw_gltf_data + 20;
        memcpy(&binary_chunk.length, raw_gltf_data + 20 + json_chunk.length, 4);
        memcpy(&binary_chunk.type, raw_gltf_data + 20 + json_chunk.length + 4, 4);
        if (binary_chunk.type != BIN) return false;
        binary_chunk.data = raw_gltf_data + 20 + json_chunk.length + 8;
        gltf->data = binary_chunk.data;
        gltf->data_length = binary_chunk.length;

        size_t json_sybmol_count = smol_count_json_symbols(json_chunk.length, json_chunk.data);
        smol_symbol *json_symbols = (smol_symbol *)gltf->allocator.allocate(json_sybmol_count * sizeof(smol_symbol));
        u8 const **symbol_char_locations = (u8 const **)gltf->allocator.allocate(json_sybmol_count * sizeof(u8 const *));
        smol_parse_json_symbols(json_chunk.length, json_chunk.data, json_sybmol_count, json_symbols, symbol_char_locations);

        size_t token_count = smol_count_json_tokens(json_sybmol_count, json_symbols);
        smol_token *tokens = (smol_token *)gltf->allocator.allocate(token_count * sizeof(smol_token));
        smol_str *token_values = (smol_str *)gltf->allocator.allocate(token_count * sizeof(smol_str));
        memset(token_values, 0, token_count * sizeof(smol_str));
        smol_parse_json_tokens(json_sybmol_count, json_symbols, symbol_char_locations, tokens, token_values);

        u8 root_objects_to_parse_count = smol_root_object_count;
        smol_root_object root_objects_to_parse[smol_root_object_count] = {
            smol_root_accessors,
            smol_root_asset,
            smol_root_bufferViews,
            smol_root_buffers,
            smol_root_meshes,
            smol_root_nodes,
            smol_root_scene,
            smol_root_scenes,
        };

        //TODO: maybe this would be improved if instead each if statment called some function on a gltf_parse_state struct and kept track of its object depth with a stack.
        size_t token_index = 0;
        while (token_index < token_count) {
                //TODO: this code should never evaluate to true.
                if (tokens[token_index] != smol_token_key){
                        ++token_index;
                        continue;
                }
                s8 maybe_root_object = smol_parse_next_root_object_key(&root_objects_to_parse_count, root_objects_to_parse, token_values[token_index]);
                if(maybe_root_object < 0){
                        ++token_index; continue;
                }
                smol_root_object root_object = (smol_root_object)maybe_root_object;
                ++token_index;
                
                if(root_object == smol_root_asset){
                        ++token_index;
                        while(tokens[token_index] != smol_token_end_object){
                                if(tokens[token_index] == smol_token_key){
                                        smol_check_key_is_string_and_assign_value(token_values, &token_index, "generator", &gltf->asset.generator);
                                        smol_check_key_is_string_and_assign_value(token_values, &token_index, "version", &gltf->asset.version);
                                }
                        }
                        ++token_index;
                } else if(root_object == smol_root_bufferViews){
                        ++token_index;
                        gltf->buffer_view_count = smol_count_objects_til_end_of_array(token_count, tokens, token_index);
                        gltf->buffer_views = (smol_buffer_view * )gltf->allocator.allocate(gltf->buffer_view_count * sizeof(smol_buffer_view));
                        memset(gltf->buffer_views, 0, gltf->buffer_view_count * sizeof(smol_buffer_view));
                        size_t buffer_view_index = 0;
                        while(tokens[token_index] != smol_token_end_array){
                                if(tokens[token_index] == smol_token_begin_object) ++token_index;
                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "buffer", &gltf->buffer_views[buffer_view_index].buffer);
                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "byteLength", &gltf->buffer_views[buffer_view_index].byte_length);
                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "byteOffset", &gltf->buffer_views[buffer_view_index].byte_offset);
                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "byteStride", &gltf->buffer_views[buffer_view_index].byte_stride);
                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "target", &gltf->buffer_views[buffer_view_index].target);
                                if(tokens[token_index] == smol_token_end_object)++buffer_view_index; 
                                ++token_index;
                        }
                        ++token_index;
                } else if(root_object == smol_root_buffers){
                        ++token_index;
                        gltf->buffer_count = smol_count_objects_til_end_of_array(token_count, tokens, token_index);
                        gltf->buffers = (smol_buffer *)gltf->allocator.allocate(gltf->buffer_count * sizeof(smol_buffer));
                        size_t buffer_index = 0;
                        while(tokens[token_index] != smol_token_end_array){
                                if(tokens[token_index] == smol_token_begin_object) ++token_index;
                                smol_check_key_is_string_and_assign_u64_value(token_values, &token_index, "byteLength", &gltf->buffers[buffer_index].byte_length);
                                if(smol_str_is_equal(token_values[token_index], "uri")){
                                        ++token_index;
                                        gltf->buffers[buffer_index].uri = token_values[token_index];
                                        ++token_index;
                                }else gltf->buffers[buffer_index].uri = {NULL,0};
                                if(tokens[token_index] == smol_token_end_object) ++buffer_index;
                                ++token_index;
                        }
                } else if(root_object == smol_root_meshes){
                        ++token_index;
                        gltf->mesh_count = smol_count_objects_til_end_of_array(token_count, tokens, token_index);
                        gltf->meshes = (smol_mesh *)gltf->allocator.allocate(sizeof(smol_mesh) * gltf->mesh_count);
                        memset(gltf->meshes, 0, sizeof(smol_mesh) * gltf->mesh_count);
                        u32 mesh_index = 0;
                        while(tokens[token_index] != smol_token_end_array){
                                if(tokens[token_index] == smol_token_begin_object) ++token_index;
                                smol_check_key_is_string_and_assign_value(token_values, &token_index, "name", &gltf->meshes[mesh_index].name);
                                if(smol_str_is_equal(token_values[token_index], "primitives")){
                                        ++token_index;
                                        gltf->meshes[mesh_index].primitive_count = smol_count_objects_til_end_of_array(token_count, tokens, token_index);
                                        gltf->meshes[mesh_index].primitives = (smol_mesh_primitive *)gltf->allocator.allocate(sizeof(smol_mesh_primitive) * gltf->meshes[mesh_index].primitive_count);
                                        memset(gltf->meshes[mesh_index].primitives, 0, sizeof(smol_mesh_primitive) * gltf->meshes[mesh_index].primitive_count);
                                        ++token_index;
                                        u8 primitive_index = 0;
                                        while(tokens[token_index] != smol_token_end_array){
                                                if(tokens[token_index] == smol_token_begin_object)++token_index;
                                                if(smol_str_is_equal(token_values[token_index], "attributes")){
                                                        token_index +=2;
                                                        u32 attribute_count = 0;
                                                        size_t counting_index = token_index;
                                                        while(tokens[counting_index] != smol_token_end_object){
                                                                if(tokens[counting_index] == smol_token_key){
                                                                        ++attribute_count;
                                                                        ++counting_index;
                                                                }
                                                                ++counting_index;
                                                        }

                                                        gltf->meshes[mesh_index].primitives[primitive_index].attribute_count = attribute_count;
                                                        gltf->meshes[mesh_index].primitives[primitive_index].attributes = (smol_mesh_primitive_attribute *)gltf->allocator.allocate(sizeof(smol_mesh_primitive_attribute) * attribute_count);
                                                        smol_mesh_primitive_attribute * attributes = gltf->meshes[mesh_index].primitives[primitive_index].attributes;
                                                        memset(attributes, 0, sizeof(smol_mesh_primitive_attribute) * attribute_count);

                                                        u8 attribute_index = 0;
                                                        while(tokens[token_index] != smol_token_end_object){
                                                                if(smol_str_is_equal(token_values[token_index], "POSITION")){
                                                                        ++token_index;
                                                                        attributes[attribute_index].name = smol_POSITION;
                                                                        attributes[attribute_index].set_index = -1;
                                                                        attributes[attribute_index].accessor = smol_str_to_s64(token_values[token_index]);
                                                                        ++attribute_index;
                                                                       ++token_index; 
                                                                } 
                                                                if(smol_str_is_equal(token_values[token_index], "NORMAL")){
                                                                        ++token_index;
                                                                        attributes[attribute_index].name = smol_NORMAL;
                                                                        attributes[attribute_index].set_index = -1;
                                                                        attributes[attribute_index].accessor = smol_str_to_s64(token_values[token_index]);
                                                                        ++attribute_index;
                                                                        ++token_index;
                                                                } 
                                                                if(smol_str_is_equal(token_values[token_index], "TANGENT")){
                                                                        ++token_index;
                                                                        attributes[attribute_index].name = smol_TANGENT;
                                                                        attributes[attribute_index].set_index = -1;
                                                                        attributes[attribute_index].accessor = smol_str_to_s64(token_values[token_index]);
                                                                        ++attribute_index;
                                                                        ++token_index;
                                                                } 
                                                                s8 texcoord_set_index = smol_check_value_is_mesh_primitive_attribute_name_get_set_index(token_values[token_index], "TEXCOORD");
                                                                if(texcoord_set_index >=0 ){
                                                                        ++token_index;
                                                                        attributes[attribute_index].name = smol_TEXCOORD;
                                                                        attributes[attribute_index].set_index = texcoord_set_index;
                                                                        attributes[attribute_index].accessor = smol_str_to_s64(token_values[token_index]);
                                                                        ++attribute_index;
                                                                        ++token_index;
                                                                } 
                                                                s8 color_set_index = smol_check_value_is_mesh_primitive_attribute_name_get_set_index(token_values[token_index], "COLOR");
                                                                if(color_set_index >=0 ){
                                                                        ++token_index;
                                                                        attributes[attribute_index].name = smol_COLOR;
                                                                        attributes[attribute_index].set_index = color_set_index;
                                                                        attributes[attribute_index].accessor = smol_str_to_s64(token_values[token_index]);
                                                                        ++attribute_index;
                                                                        ++token_index;
                                                                } 
                                                                s8 joints_set_index = smol_check_value_is_mesh_primitive_attribute_name_get_set_index(token_values[token_index], "JOINTS");
                                                                if(joints_set_index >=0 ){
                                                                        ++token_index;
                                                                        attributes[attribute_index].name = smol_JOINTS;
                                                                        attributes[attribute_index].set_index = joints_set_index;
                                                                        attributes[attribute_index].accessor = smol_str_to_s64(token_values[token_index]);
                                                                        ++attribute_index;
                                                                        ++token_index;
                                                                } 
                                                                s8 weights_set_index = smol_check_value_is_mesh_primitive_attribute_name_get_set_index(token_values[token_index], "WEIGHTS");
                                                                if(weights_set_index >=0 ){
                                                                        ++token_index;
                                                                        attributes[attribute_index].name = smol_WEIGHTS;
                                                                        attributes[attribute_index].set_index = weights_set_index;
                                                                        attributes[attribute_index].accessor = smol_str_to_s64(token_values[token_index]);
                                                                        ++attribute_index;
                                                                        ++token_index;
                                                                } 
                                                                if(tokens[token_index] == smol_token_end_object){
                                                                        break;
                                                                }
                                                                ++token_index;
                                                        }
                                                        ++token_index;
                                                }
                                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "indices", &gltf->meshes[mesh_index].primitives[primitive_index].indices);
                                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "material", &gltf->meshes[mesh_index].primitives[primitive_index].material);
                                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "mode", &gltf->meshes[mesh_index].primitives[primitive_index].mode);
                                                if(tokens[token_index] == smol_token_end_object){
                                                        ++primitive_index;
                                                }
                                                ++token_index;
                                        }
                                        ++token_index;
                                }
                                if(smol_str_is_equal(token_values[token_index], "weights")){

                                }
                                if(tokens[token_index] == smol_token_end_object) ++mesh_index;
                                ++token_index;
                        }
                        ++token_index;
                } else if(root_object == smol_root_nodes){
                        ++token_index;
                        gltf->node_count = smol_count_objects_til_end_of_array(token_count, tokens, token_index);
                        gltf->nodes = (smol_node *)gltf->allocator.allocate(sizeof(smol_node) * gltf->node_count); 
                        memset(gltf->nodes, 0, sizeof(smol_node) * gltf->node_count);
                        u32 node_index =0;
                        while(tokens[token_index] != smol_token_end_array){
                                if(tokens[token_index] == smol_token_begin_object)++token_index;
                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "mesh", &gltf->nodes[node_index].mesh);
                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "skin", &gltf->nodes[node_index].mesh);
                                smol_check_key_is_string_and_assign_value(token_values, &token_index, "name", &gltf->nodes[node_index].name);
                                if(tokens[token_index] == smol_token_end_object)++node_index;
                                ++token_index;
                        }
                        ++token_index;
                } else if(root_object == smol_root_scene){
                        gltf->scene = smol_str_to_s64(token_values[token_index]);
                        ++token_index;
                } else if(root_object == smol_root_scenes){
                        ++token_index;
                        gltf->scene_count = smol_count_objects_til_end_of_array(token_count, tokens, token_index);
                        gltf->scenes = (smol_scene *)gltf->allocator.allocate(sizeof(smol_scene) * gltf->scene_count);
                        size_t scene_index = 0;
                        for(;tokens[token_index] != smol_token_end_array; ++token_index){
                                if(tokens[token_index] == smol_token_begin_object) ++token_index;
                                if(tokens[token_index] == smol_token_key){
                                        smol_check_key_is_string_and_assign_value(token_values, &token_index, "name", &gltf->scenes[scene_index].name );
                                        if(smol_str_is_equal(token_values[token_index], "nodes")){
                                                token_index+=2;
                                                size_t node_count = smol_count_tokens_til_end_of_array(token_count, tokens, token_index);
                                                gltf->scenes[scene_index].node_count = node_count;
                                                gltf->scenes[scene_index].nodes = (u32 *)gltf->allocator.allocate(sizeof(u32) * node_count);
                                                memset(gltf->scenes[scene_index].nodes, 0, sizeof(u32) * node_count);
                                                for(size_t node_index = 0; node_index < node_count; ++node_index){
                                                        gltf->scenes[scene_index].nodes[node_index] = smol_str_to_s64(token_values[token_index]);
                                                        ++token_index;
                                                }
                                                ++token_index;
                                        }
                                }
                        }
                } else if (root_object == smol_root_accessors) {
                        ++token_index;
                        gltf->accessor_count = smol_count_objects_til_end_of_array(token_count, tokens, token_index);
                        gltf->accessors = (smol_accessor *) gltf->allocator.allocate(sizeof(smol_accessor) * gltf->accessor_count);
                        memset(gltf->accessors, 0, sizeof(smol_accessor) * gltf->accessor_count);
                        u32 accessor_index = 0;
                        s64 min_array_begin_index =-1; 
                        s64 max_array_begin_index =-1; 
                        bool has_component_type = false;
                        bool has_type = false;
                        while(tokens[token_index] != smol_token_end_array){
                                if(tokens[token_index] == smol_token_begin_object) ++token_index;
                                if(tokens[token_index] == smol_token_key){
                                        smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "bufferView", &gltf->accessors[accessor_index].buffer_view);
                                        if(smol_str_is_equal(token_values[token_index], "componentType")){
                                                ++token_index;
                                                s8 maybe_component_type = lookup_component_type(token_values[token_index]);
                                                if(maybe_component_type >= 0){
                                                        has_component_type = true;
                                                        gltf->accessors[accessor_index].component_type = (smol_GLTF_component_type)maybe_component_type;
                                                }else{
                                                        //TODO: log this is bad somewhere.
                                                }
                                                ++token_index;
                                        }
                                        smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "count", &gltf->accessors[accessor_index].count);
                                        if(smol_str_is_equal(token_values[token_index], "max")){
                                                ++token_index;
                                                max_array_begin_index = token_index;
                                                while(tokens[token_index] != smol_token_end_array) ++token_index;
                                                ++token_index;
                                        }  
                                        if(smol_str_is_equal(token_values[token_index], "min")){
                                                ++token_index;
                                                min_array_begin_index = token_index;
                                                while(tokens[token_index] != smol_token_end_array) ++token_index;
                                                ++token_index;
                                        } 
                                        if(smol_str_is_equal(token_values[token_index], "type")){
                                                ++token_index;
                                                s8 maybe_type = lookup_smol_GLTF_component(token_values[token_index]);
                                                if(maybe_type >= 0){
                                                        gltf->accessors[accessor_index].type = (smol_GLTF_type)maybe_type;
                                                        has_type = true;
                                                }else{
                                                        //TODO: log this error.
                                                }
                                                ++token_index;
                                        }
                                }  
                                if(tokens[token_index] == smol_token_end_object){
                                        if(has_component_type && has_type && min_array_begin_index >= 0 && max_array_begin_index >= 0){
                                                //TODO: figure out wtf the padding bullshit was that they were talking about in the spec.
                                                size_t component_byte_count = smol_GLTF_component_type_byte_count[gltf->accessors[accessor_index].component_type];
                                                size_t type_component_count = smol_GLTF_type_component_count[gltf->accessors[accessor_index].type];
                                                size_t total_byte_count = type_component_count * component_byte_count;
                                                gltf->accessors[accessor_index].min_values = gltf->allocator.allocate(total_byte_count);
                                                gltf->accessors[accessor_index].max_values = gltf->allocator.allocate(total_byte_count);
                                                auto min_token_index = min_array_begin_index+1;
                                                auto max_token_index = max_array_begin_index+1;
                                                switch(gltf->accessors[accessor_index].component_type){
                                                        case smol_GLTF_component_type_none:{
                                                        //TODO: error
                                                        break;
                                                        } 
                                                        case smol_GLTF_component_type_s8:
                                                        case smol_GLTF_component_type_u8:{
                                                                for(size_t i = 0; i < type_component_count; ++i){
                                                                        ((s8 *)gltf->accessors[accessor_index].min_values)[i] = (s8)smol_str_to_s64(token_values[min_token_index]);
                                                                        ((s8 *)gltf->accessors[accessor_index].max_values)[i] = (s8)smol_str_to_s64(token_values[max_token_index]);
                                                                        ++min_token_index;
                                                                        ++max_token_index;
                                                                }
                                                                break;
                                                        }
                                                        case smol_GLTF_component_type_s16:
                                                        case smol_GLTF_component_type_u16:{
                                                                for(size_t i = 0; i < type_component_count; ++i){
                                                                        ((s16 *)gltf->accessors[accessor_index].min_values)[i] = (s16)smol_str_to_s64(token_values[min_token_index]);
                                                                        ((s16 *)gltf->accessors[accessor_index].max_values)[i] = (s16)smol_str_to_s64(token_values[max_token_index]);
                                                                        ++min_token_index;
                                                                        ++max_token_index;
                                                                }
                                                                break;
                                                        }
                                                        case smol_GLTF_component_type_u32:{
                                                                for(size_t i = 0; i < type_component_count; ++i){
                                                                        ((s32 *)gltf->accessors[accessor_index].min_values)[i] = (u32)smol_str_to_s64(token_values[min_token_index]);
                                                                        ((s32 *)gltf->accessors[accessor_index].max_values)[i] = (u32)smol_str_to_s64(token_values[max_token_index]);
                                                                        ++min_token_index;
                                                                        ++max_token_index;
                                                                }
                                                                break;
                                                        }
                                                        case smol_GLTF_component_type_f32:{
                                                                for(size_t i = 0; i < type_component_count; ++i){
                                                                        ((s32 *)gltf->accessors[accessor_index].min_values)[i] = smol_str_to_f32(token_values[min_token_index]);
                                                                        ((s32 *)gltf->accessors[accessor_index].max_values)[i] = smol_str_to_f32(token_values[max_token_index]);
                                                                        ++min_token_index;
                                                                        ++max_token_index;
                                                                }
                                                                break;
                                                        }
                                                }
                                        }else{
                                                //TODO: error incomplete data to parse max and min values.
                                        }
                                        min_array_begin_index =-1; 
                                        max_array_begin_index =-1; 
                                        has_component_type = false;
                                        has_type = false;
                                        ++accessor_index;
                                } 
                                ++token_index;
                        }
                        ++token_index;
                }
        }

        gltf->allocator.free(token_values);
        gltf->allocator.free(tokens);
        gltf->allocator.free(symbol_char_locations);
        gltf->allocator.free(json_symbols);
        return true;
}

//TODO:
inline void smol_free_GLTF(smol_GLTF *gltf) NOEXCEPT {}

#ifdef __cplusplus
}
#endif
