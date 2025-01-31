#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#define NOEXCEPT noexcept
#define CONSTEXPR constexpr
#define PLA_NULL nullptr
#define NODISCARD [[nodiscard]]
extern "C" {
#else
#define NOEXCEPT
#define CONSTEXPR
#define PLA_NULL 0
#define NODISCARD
#endif

#define INTERNAL static inline CONSTEXPR

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef size_t usize;
typedef float f32;
typedef enum pla_bool{pla_false, pla_true}pla_bool;
typedef char const * c_str;

#define U8_MAX UINT8_MAX

#define glTF 0x46546C67
#define JSON 0x4E4F534A
#define BIN 0x004E4942

// Ironic
typedef struct pla_str {
        u8 const *data;
        usize length;
} pla_str;

NODISCARD INTERNAL pla_bool pla_str_is_equal(pla_str stra, char const *const strb) NOEXCEPT {
        for(usize i = 0;i < stra.length;++i) if(stra.data[i] != strb[i]) return pla_false;
        if(strb[stra.length] != '\0') return pla_false;
        return pla_true;
}

NODISCARD INTERNAL pla_bool pla_str_in(pla_str str, char const * const * strs, usize count) NOEXCEPT{
        for(usize i = 0; i < count; ++i) if(pla_str_is_equal(str, strs[i])) return pla_true;
        return pla_false;
}


NODISCARD INTERNAL s64 pla_str_to_s64(pla_str str) NOEXCEPT{
        if(str.length == 0 || str.data == PLA_NULL) return 0;
        if(str.length > 20) return 0;
        s64 value = 0;
        s64 sign = 1;
        if(str.data[0] == '-') sign = -1;
        for(u64 position = 0 + (sign < 0); position < str.length; ++position){
                u64 digit = str.data[position] - '0';
                pla_bool is_valid = (pla_bool)(digit >= 0 || digit <= 9);
                value = value * 10 + (digit * is_valid);
        }

        return value * sign;
}

NODISCARD INTERNAL f32 pla_str_to_f32(pla_str str) NOEXCEPT{
        if(str.length == 0 || str.data == PLA_NULL) return 0;
        f32 sign = 1;
        f32 value = 0;
        if(str.data[0] == '-') sign = -1;
        s64 point_index = -1;
        for(usize i = 0 + (sign < 0); i < str.length; ++i){
                if(str.data[i] == '.'){
                        point_index = i;
                        continue;
                }
                u64 digit = str.data[i] - '0';
                if(digit >= 0 || digit <= 9){
                        value = value * 10.0f + (f32)digit;
                        if(point_index >= 0){
                                f32 decimal = 10;
                                for(usize b = 1; b < i-point_index; ++b){
                                        decimal *= 10.0f;
                                }
                                value += (f32)digit/decimal;
                        }
                }
        }
        return value * sign;
}

typedef struct pla_asset {
        // in data
        pla_str generator;
        // in data
        // Maybe just a number or enum.
        pla_str version;
} pla_asset;

typedef struct pla_scene {
        pla_str name;
        u32 *nodes;
        u8 node_count;
} pla_scene;

typedef struct pla_node {
        pla_str name;
        u32 mesh;
        u32 skin;
        // f32 translation[3];
        // f32 rotation[4];
        // f32 scale[3];
        f32 matrix[4 * 4];
        u32 *children;
        u8 child_count;
} pla_node;

typedef enum pla_mesh_primitive_attribute_name : u8{
        pla_POSITION,
        pla_NORMAL,
        pla_TANGENT,

        //Require set index.
        pla_TEXCOORD,
        pla_COLOR,
        pla_JOINTS,
        pla_WEIGHTS,
}pla_mesh_primitive_attribute_name;

typedef struct pla_mesh_primitive_attribute{
        u32 accessor;
        pla_mesh_primitive_attribute_name name;
        s8 set_index;
}pla_mesh_primitive_attribute;

INTERNAL s8 pla_check_value_is_mesh_primitive_attribute_name_get_set_index(pla_str value, char const * test_str){
        for(usize i = 0; i < value.length; ++i){
                if(test_str[i] != value.data[i]){
                        if(value.data[i] == '_'){
                                pla_str set_index_str = {.data = value.data + i + 1, .length = value.length - (i+1)};
                                return pla_str_to_s64(set_index_str);
                        }
                        else return -1;
                } 
        }
        return -1;
}

typedef struct pla_mesh_primitive{
        u8 attribute_count;
        pla_mesh_primitive_attribute * attributes;
        u32 indices;
        u32 material;
        u32 mode;
}pla_mesh_primitive;

typedef struct pla_mesh {
        pla_str name;
        pla_mesh_primitive *primitives;
        u8 primitive_count;
} pla_mesh;

typedef enum pla_GLTF_component_type : s8 {
        pla_GLTF_component_type_s8,
        pla_GLTF_component_type_u8,
        pla_GLTF_component_type_s16,
        pla_GLTF_component_type_u16,
        pla_GLTF_component_type_u32,
        pla_GLTF_component_type_f32,
} pla_GLTF_component_type;

char const * const pla_GLTF_component_type_strings[6] = {"5120","5121","5122","5123","5125","5126"};

//returns false on failure to lookup component type.
INTERNAL pla_bool lookup_component_type(pla_str value, pla_GLTF_component_type * type) NOEXCEPT{
        for(usize i = 0; i < 6; ++i){
                if(pla_str_is_equal(value, pla_GLTF_component_type_strings[i])) {
                        *type =(pla_GLTF_component_type)i; 
                        return pla_true;
                }
        }
        return pla_false;
}

u8 const pla_GLTF_component_type_byte_count[6] = {1,1,2,2,4,4};

typedef enum pla_GLTF_type{
        pla_GLTF_none = -1,
        pla_GLTF_SCALAR,
        pla_GLTF_VEC2,
        pla_GLTF_VEC3,
        pla_GLTF_VEC4,
        pla_GLTF_MAT2,
        pla_GLTF_MAT3,
        pla_GLTF_MAT4,
        pla_GLTF_type_MAX_ENUM,
} pla_GLTF_type;

char const * const pla_GLTF_type_strings[8] = { "SCALAR", "VEC2", "VEC3", "VEC4", "MAT2", "MAT3", "MAT4" };

//returns -1 if the string is not a valid component.
INTERNAL pla_bool lookup_pla_GLTF_type(pla_str str, pla_GLTF_type * type) NOEXCEPT{
        for(usize i = 0; i < 8; ++i){
                if(pla_str_is_equal(str, pla_GLTF_type_strings[i])){
                        *type = (pla_GLTF_type)i;
                        return pla_true;
                };
        }
        return pla_false;
}

u8 const pla_GLTF_type_component_count[8] = {1,2,3,4,4,9,16};

typedef struct pla_accessor {
        pla_GLTF_component_type component_type;
        pla_GLTF_type type;
        u32 buffer_view;
        u32 byte_offset;
        //must be cast based off the components array;
        void *min_values;
        void *max_values;
        u32 count;
} pla_accessor;

typedef struct pla_buffer_view {
        u32 buffer;
        u32 byte_length;
        u32 byte_offset;
        u32 byte_stride;
        u32 target;
} pla_buffer_view;

typedef struct pla_buffer {
        pla_str uri;
        u64 byte_length;
} pla_buffer;

typedef struct pla_allocator {
        void * user_data;
        void *(*allocate)(void * user_data, usize size);
        void (*free)(void * user_data, void *ptr);
} pla_allocator;

typedef struct pla_GLTF {
        pla_allocator allocator;
        pla_asset asset;
        u32 scene;
        pla_scene *scenes;
        pla_node *nodes;
        pla_mesh *meshes;
        pla_accessor * accessors;
        pla_buffer_view *buffer_views;
        pla_buffer *buffers;
        usize data_length;
        //Non Owning this points into the raw_gltf_data passed in by the caller.
        u8 const *data;
        u32 accessor_count;
        u32 scene_count;
        u32 node_count;
        u32 mesh_count;
        u32 buffer_view_count;
        u32 buffer_count;

} pla_GLTF;

typedef struct pla_header {
        u32 magic;
        u32 version;
        u32 length;
} pla_header;

typedef struct pla_chunk {
        u32 length;
        u32 type;
        u8 const *data;
} pla_chunk;

typedef enum pla_symbol {
        pla_symbol_none = 0,
        pla_symbol_open_square,
        pla_symbol_close_square,
        pla_symbol_open_squigily,
        pla_symbol_close_squigily,
        pla_symbol_begin_string,
        pla_symbol_end_string,
        pla_symbol_colon,
        pla_symbol_comma,
} pla_symbol;

typedef enum pla_token {
        pla_token_key,
        pla_token_value,
        pla_token_begin_object,
        pla_token_end_object,
        pla_token_begin_array,
        pla_token_end_array,
} pla_token;

typedef enum pla_root_object{
        pla_root_accessors,
        pla_root_asset,
        pla_root_bufferViews,
        pla_root_buffers,
        pla_root_meshes,
        pla_root_nodes,
        pla_root_scene,
        pla_root_scenes,
        pla_root_MAX_ENUM,
        pla_root_none = U8_MAX,
} pla_root_object;

 const char * const pla_root_object_names[] = {
    "accessors",
    "asset",
    "bufferViews",
    "buffers",
    "meshes",
    "nodes",
    "scene",
    "scenes",
};

INTERNAL u64 pla_count_json_symbols(u64 json_size, u8 const *json)  NOEXCEPT{
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

INTERNAL void pla_parse_json_symbols(u64 json_size, u8 const * const json, u64 symbol_count, pla_symbol *symbols, u8 const **symbol_chars)  NOEXCEPT{
        u8 const **current_symbol_char = symbol_chars;
        pla_symbol *current_symbol = symbols;
        pla_bool in_string = pla_false;

        for (u8 const *byte = json; byte != json + json_size; ++byte) {
                if (*byte == '"') {
                        if (in_string) {
                                in_string = pla_false;
                                *current_symbol = pla_symbol_end_string;
                                goto eat_symbol;
                        } else {
                                in_string = pla_true;
                                *current_symbol = pla_symbol_begin_string;
                                goto eat_symbol;
                        }
                } else if (*byte == '{') {
                        *current_symbol = pla_symbol_open_squigily;
                        goto eat_symbol;
                } else if (*byte == '}') {
                        *current_symbol = pla_symbol_close_squigily;
                        goto eat_symbol;
                } else if (*byte == '[') {
                        *current_symbol = pla_symbol_open_square;
                        goto eat_symbol;
                } else if (*byte == ']') {
                        *current_symbol = pla_symbol_close_square;
                        goto eat_symbol;
                } else if (*byte == ':') {
                        *current_symbol = pla_symbol_colon;
                        goto eat_symbol;
                } else if (*byte == ',') {
                        *current_symbol = pla_symbol_comma;
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

typedef enum pla_json_parse_stack_type: u8{
        pla_none,
        pla_object,
        pla_array,
        pla_value_array,
        pla_single_or_empty_array,
}pla_json_parse_stack_type;

// INTERNAL usize pla_count_json_tokens(usize symbol_count, pla_symbol const *const symbols)  NOEXCEPT{
//         usize token_count = 0;
//         pla_bool in_value_array = pla_false;
//         for (usize i = 0; i < symbol_count; ++i) {
//                 pla_symbol symbol = symbols[i];
//                 if (symbol == pla_symbol_open_squigily)
//                         ++token_count;
//                 else if (symbol == pla_symbol_close_squigily)
//                         ++token_count;
//                 else if (symbol == pla_symbol_open_square){
//                         ++token_count;
//                         if(symbols[i+1] == pla_symbol_comma || symbols[i+1] == pla_symbol_close_square){
//                                 ++token_count;
//                                 in_value_array = pla_true;
//                         }
//                 }
//                 else if (symbol == pla_symbol_close_square){
//                         ++token_count;
//                         if(in_value_array) in_value_array = pla_false;
//                 }
//                 else if (symbol == pla_symbol_begin_string)
//                         ++token_count;
//                 else if (symbol == pla_symbol_colon) {
//                         pla_symbol next_symbol = symbols[i + 1];
//                         if (next_symbol == pla_symbol_comma) {
//                                 ++token_count;
//                                 ++i;
//                         } else if (next_symbol == pla_symbol_close_squigily){
//                                 ++token_count;
//                         } else if (next_symbol == pla_symbol_begin_string)
//                                 continue;
//                 }else if(symbol == pla_symbol_comma && in_value_array){
//                         ++token_count;
//                 }
//         }
//         return token_count;
// }
//
//
// INTERNAL void pla_parse_json_tokens(usize symbol_count, pla_symbol const *const symbols, u8 const *const *const symbol_locations, pla_token *tokens, pla_str *token_values) NOEXCEPT{
//         usize token_index = 0;
//         pla_json_parse_stack_type type_stack[UINT8_MAX] = {pla_none}; 
//         u8 stack_spot = 0;
//         for (usize i = 0; i < symbol_count; ++i) {
//                 if (symbols[i] == pla_symbol_open_squigily){
//                         tokens[token_index] = pla_token_begin_object;
//                         ++stack_spot;
//                         type_stack[stack_spot] = pla_object;
//                 } else if (symbols[i] == pla_symbol_close_squigily){
//                         tokens[token_index] = pla_token_end_object;
//                         --stack_spot;
//                 } else if (symbols[i] == pla_symbol_open_square){
//                         tokens[token_index] = pla_token_begin_array;
//                         ++stack_spot;
//                         if(symbols[i+1] == pla_symbol_comma || symbols[i+1] == pla_symbol_close_square){
//                                 ++token_index;
//                                 tokens[token_index] = pla_token_value;
//                                 token_values[token_index].data = symbol_locations[i]+1;
//                                 token_values[token_index].length = symbol_locations[i+1] - token_values[token_index].data;
//                                 type_stack[stack_spot] = pla_value_array;
//                         } else type_stack[stack_spot] = pla_array;
//                 } else if (symbols[i] == pla_symbol_close_square){
//                         tokens[token_index] = pla_token_end_array;
//                         --stack_spot;
//                 } else if (symbols[i] == pla_symbol_begin_string) {
//                         if(type_stack[stack_spot] == pla_object) tokens[token_index] = pla_token_key;
//                         else if(type_stack[stack_spot] == pla_array) tokens[token_index] = pla_token_value;
//                         token_values[token_index].data = symbol_locations[i] + 1;
//                         token_values[token_index].length = symbol_locations[i + 1] - token_values[token_index].data;
//                 } else if (symbols[i] == pla_symbol_colon) {
//                         if (symbols[i+1] == pla_symbol_comma) {
//                                 tokens[token_index] = pla_token_value;
//                                 token_values[token_index].data = symbol_locations[i] + 1;
//                                 token_values[token_index].length = symbol_locations[i + 1] - token_values[token_index].data;
//                                 ++i;
//                         }else if(symbols[i+1] == pla_symbol_close_squigily){
//                                 tokens[token_index] = pla_token_value;
//                                 token_values[token_index].data = symbol_locations[i] + 1;
//                                 token_values[token_index].length = symbol_locations[i + 1] - token_values[token_index].data;
//                         } else if (symbols[i+1] == pla_symbol_begin_string) {
//                                 tokens[token_index] = pla_token_value;
//                                 token_values[token_index].data = symbol_locations[i + 1] + 1;
//                                 token_values[token_index].length = symbol_locations[i + 2] - token_values[token_index].data;
//                                 i += 2;
//                         } else continue;
//                 } else if(symbols[i] == pla_symbol_comma && type_stack[stack_spot] == pla_value_array){
//                         token_values[token_index].data = symbol_locations[i]+1;
//                         token_values[token_index].length = symbol_locations[i+1] - token_values[token_index].data;
//                         tokens[token_index] = pla_token_value;
//                 } else continue;
//                 ++token_index;
//         }
// }
//
// INTERNAL usize pla_count_objects_til_end_of_array(usize token_count, pla_token const * tokens, usize token_index)NOEXCEPT{
//         usize object_count = 0;
//         s64 nested_object_count = -1;
//         do{
//                 if(tokens[token_index] == pla_token_begin_object){
//                         ++nested_object_count;
//                 }else if(tokens[token_index] == pla_token_end_object){
//                         --nested_object_count;
//                         if(nested_object_count < 0) ++object_count;
//                 }else if(tokens[token_index] == pla_token_end_array  && nested_object_count < 0){
//                         break;
//                 }
//                 ++token_index;
//         }while(token_index < token_count);
//
//         return object_count;
// }
//
// INTERNAL usize pla_count_tokens_til_end_of_array(usize token_count, pla_token const * tokens, usize current_token_index)NOEXCEPT{
//         usize starting_token_index = current_token_index;
//         do ++current_token_index; while(tokens[current_token_index] != pla_token_end_array);
//         return current_token_index - starting_token_index;
// }
//
// //TODO: decrease count and remove object from array by swapping it to the end.
// INTERNAL s8 pla_parse_next_root_object_key(u8 * root_objects_to_parse_count, pla_root_object * root_objects_to_parse, pla_str token_value)NOEXCEPT{
//         if(token_value.length == 0 || token_value.data == PLA_NULL) return -1;
//         for (u8 root_object_to_parse_index = 0; root_object_to_parse_index < *root_objects_to_parse_count; ++root_object_to_parse_index) {
//                 if (pla_str_is_equal(token_value, pla_root_object_names[root_objects_to_parse[root_object_to_parse_index]])){
//                         pla_root_object root_object = root_objects_to_parse[root_object_to_parse_index];
//                         root_objects_to_parse[root_object_to_parse_index] = root_objects_to_parse[(*root_objects_to_parse_count) - 1];
//                         --*root_objects_to_parse_count;
//                         return root_object;
//                 } 
//         }
//         return -1;
// }
//
// INTERNAL void pla_check_key_is_string_and_assign_value(pla_str const * token_values, usize * token_index, char const * test_str,pla_str * assign_location)NOEXCEPT{
//         if(pla_str_is_equal(token_values[*token_index], test_str)){
//                 ++*token_index;
//                 *assign_location = token_values[*token_index];
//                 ++*token_index;
//         }
// }
//
// INTERNAL void check_key_is_string_and_assign_s64_value(pla_str const * token_values, usize * token_index, char const * test_str,s64 * assign_location)NOEXCEPT{
//         if(pla_str_is_equal(token_values[*token_index], test_str)){
//                 ++*token_index;
//                 *assign_location = pla_str_to_s64(token_values[*token_index]);
//                 ++*token_index;
//         }
// }
//
// INTERNAL void pla_check_key_is_string_and_assign_u64_value(pla_str const * token_values, usize * token_index, char const * test_str,u64 * assign_location)NOEXCEPT{
//         if(pla_str_is_equal(token_values[*token_index], test_str)){
//                 ++*token_index;
//                 //TODO: write an unsigned version.
//                 *assign_location = (u32)pla_str_to_s64(token_values[*token_index]);
//                 ++*token_index;
//         }
// }
//
// INTERNAL void pla_check_key_is_string_and_assign_u32_value(pla_str const * token_values, usize * token_index, char const * test_str,u32 * assign_location)NOEXCEPT{
//         if(pla_str_is_equal(token_values[*token_index], test_str)){
//                 ++*token_index;
//                 //TODO: write an unsigned version.
//                 *assign_location = (u32)pla_str_to_s64(token_values[*token_index]);
//                 ++*token_index;
//         }
// }


//Have no idea wtf to call this.
#define do_loopy_thing(test1, test2) for(; c < jsize; ++c) if(test1) for(usize c2 = c+1; c2 < jsize; ++c2) if(test2)
#define check_string_loopy_thing do_loopy_thing(jdata[c] == '"', jdata[c2] == '"')
#define for_c for(; c < jsize; ++c)

//returns c + whatever bytes to the end of the value.
static usize parse_til_next_symbol(usize jsize, u8 const * jdata, usize c){
        ++c;
        for(; c < jsize; ++c){
                switch(jdata[c]){
                        case '"': return c;
                        case '{': return c;
                        case '}': return c;
                        case ',': return c;
                        case ':': return c;
                }
        }
        return c;
}

//checks the next symbol is correct this is only ment to be used in the main function
#define parse(symbol)\
c = parse_til_next_symbol(jsize, jdata, c);\
if(c != symbol) return pla_false;

//return new offset if correct else return SIZE_MAX
inline usize try_parse_string(usize jsize, u8 const * jdata, usize c, pla_str * out_str){
        c = parse_til_next_symbol(jsize, jdata, c);
        if(jdata[c] != '"') return SIZE_MAX;
        usize c2 = parse_til_next_symbol(jsize, jdata, c);
        if(jdata[c2] != '"') return SIZE_MAX;
        out_str->data = jdata+c+1;
        out_str->length = c2 -(c+1);
        return c2;
}

#define parse_string(string)\
c = try_parse_string(jsize, jdata, c, &string);\
if(c == SIZE_MAX) return pla_false;


static usize try_parse_value(usize jsize, u8 const * jdata, usize c, pla_str *value){
        c = parse_til_next_symbol(jsize, jdata, c);
        if(jdata[c] != ':') return SIZE_MAX;
        usize c2 = parse_til_next_symbol(jsize, jdata, c);
        //Not a number value, but a string
        if(jdata[c2] == '"'){
                c = c2;
                c2 = parse_til_next_symbol(jsize, jdata, c);
                if(jdata[c2] != '"') return SIZE_MAX;
                value->data = jdata+c+1;
                value->length = c2 - (c+1);
        }
        if(jdata[c2] != ',' && jdata[c2] != '}' ) return SIZE_MAX;
        value->data = jdata+c+1;
        value->length = c2 - (c+1);
        return c2;
}
#define parse_value \
pla_str value;\
c = try_parse_value(jsize, jdata, c, &value);\
if(c == SIZE_MAX) return pla_false;


//return new offset if correct else return usize max
inline usize try_count_items_in_array_or_object(usize jsize, u8 const * jdata, usize c, u32 * out_count){
        c = parse_til_next_symbol(jsize, jdata, c);
        if(jdata[c] != '[' && jdata[c] != '{') return SIZE_MAX;
        usize object_depth = 0;
        pla_bool in_string = pla_false;
        for (; c < jsize; ++c){
                c = parse_til_next_symbol(jsize, jdata, c);
                switch(jdata[c]){
                        case '"' : if(jdata[c-1] != '\\')in_string = (pla_bool)!in_string;
                        case '{': case '[': ++object_depth; continue;
                        case '}': case ']': 
                                if(object_depth == 0){
                                        return c;
                                }
                                --object_depth; 
                                continue;
                        case ',': if(object_depth == 0) ++*out_count; continue;
                }
        }
        return c;
}

#define parse_begining_of_root_array(property_name, count_name)\
        parse(':'); \
        u32 count_name = 0; \
        if(try_count_items_in_array_or_object(jsize, jdata, c, &count_name) == SIZE_MAX) return pla_false; \
        if(buffer) out_gltf->count_name = count_name; \
        if(buffer) out_gltf->property_name = (typeof(out_gltf->property_name))(buffer + *buffer_size); \
        *buffer_size += sizeof(out_gltf->property_name[0]) * count_name; \
        parse('[');

#define parse_array_object_key\
        u32 buffer_view_components = 0; \
        pla_str key = {0}; \
        c = try_parse_string(jsize, jdata, c, &key); \
        if(c == SIZE_MAX) return pla_false;

//If buffer is null it just counts the size needed for a buffer to put the object in.
pla_bool pla_parse_gltf2(u32 data_size, u8 const * data, usize * buffer_size, u8 * buffer, pla_GLTF * out_gltf) NOEXCEPT{
        pla_header header;
        pla_chunk json_chunk;
        pla_chunk binary_chunk;

        memcpy(&header.magic, data, 4);
        if (header.magic != glTF) return pla_false;
        memcpy(&header.version, data + 4, 4);
        memcpy(&header.length, data + 8, 4);
        memcpy(&json_chunk.length, data + 12, 4);
        memcpy(&json_chunk.type, data + 16, 4);
        if (json_chunk.type != JSON) return pla_false;
        json_chunk.data = data + 20;
        u8 const * jdata = json_chunk.data;
        usize jsize = json_chunk.length;
        memcpy(&binary_chunk.length, data + 20 + json_chunk.length, 4);
        memcpy(&binary_chunk.type, data + 20 + json_chunk.length + 4, 4);
        if (binary_chunk.type != BIN) return pla_false;
        binary_chunk.data = data + 20 + json_chunk.length + 8;

        pla_root_object current_object = pla_root_object::pla_root_none;

        if(buffer) out_gltf = (pla_GLTF *)(buffer + *buffer_size);
        *buffer_size += sizeof(pla_GLTF);

        usize c = 0;
        u32 root_object_count = 0;
        if(try_count_items_in_array_or_object(jsize, jdata, c, &root_object_count) == SIZE_MAX) return pla_false;

        parse('{')
        for(u32 root_object_index = 0; root_object_index < root_object_count; ++root_object_index){
                pla_str root_object = {};
                parse_string(root_object)

                if(pla_str_is_equal(root_object, "accessors")){
                        parse_begining_of_root_array(accessors, accessor_count);
                        for(usize a; a < accessor_count; ++a){
                                u32 accessor_components = 0;
                                if(try_count_items_in_array_or_object(jsize, jdata, c, &accessor_components) == SIZE_MAX) return pla_false;
                                parse('{');
                                for(u32 comp = 0; comp < accessor_components; ++comp){
                                        parse_array_object_key
                                        //TODO: check if this value is valid.
                                        if(pla_str_is_equal(key, "componentType") && buffer){
                                                parse_value
                                                if(!lookup_component_type(value, &out_gltf->accessors[a].component_type)) return pla_false;
                                        } else if(pla_str_is_equal(key, "type") && buffer) {
                                                parse_value
                                                if(buffer) if(!lookup_pla_GLTF_type(value, &out_gltf->accessors[a].type)) return pla_false;
                                        } else if(pla_str_is_equal(key, "bufferView") && buffer) {
                                                parse_value
                                                //TODO: check string can be int.
                                                if(buffer) out_gltf->accessors[a].buffer_view = pla_str_to_s64(value);
                                        } else if(pla_str_is_equal(key, "byteOffset") && buffer) {
                                                parse_value
                                                if(buffer) out_gltf->accessors[a].byte_offset = pla_str_to_s64(value);
                                        } else if(pla_str_is_equal(key, "min")){
                                                //TODO:
                                        } else if(pla_str_is_equal(key, "max")){
                                                //TODO:
                                        } else if(pla_str_is_equal(key, "count") && buffer){
                                                parse_value
                                                if(buffer) out_gltf->accessors[a].count = pla_str_to_s64(value);
                                        }
                                        c = parse_til_next_symbol(jsize, jdata, c);
                                        if(jdata[c] != ',' && jdata[c] != '}') return pla_false;
                                }
                                c = parse_til_next_symbol(jsize, jdata, c);
                                if(jdata[c] != ',' && jdata[c] != ']') return pla_false;
                        }
                } else if(pla_str_is_equal(root_object, "asset")){
                        parse(':');
                        u32 asset_items = 0;
                        if(try_count_items_in_array_or_object(jsize, jdata, c, &asset_items) == SIZE_MAX) return pla_false;
                        for(u32 i = 0; i < asset_items; ++i){
                                pla_str key = {0};
                                c = try_parse_string(jsize, jdata, c, &key);
                                if(c == SIZE_MAX) return pla_false;
                                if(pla_str_is_equal(key, "generator")){
                                        parse_value
                                        if(buffer) out_gltf->asset.generator = value;
                                }else if(pla_str_is_equal(key, "version")){
                                        parse_value
                                        if(buffer) out_gltf->asset.version = value;
                                }
                                c = parse_til_next_symbol(jsize, jdata, c);
                                if(jdata[c] != ',' && jdata[c] != '}') return pla_false;
                        }
                } else if(pla_str_is_equal(root_object, "bufferViews")){
                        parse_begining_of_root_array(buffer_views, buffer_view_count);
                        for(usize b; b < buffer_view_count; ++b){
                                u32 buffer_view_components = 0;
                                if(try_count_items_in_array_or_object(jsize, jdata, c, &buffer_view_components) == SIZE_MAX) return pla_false;
                                parse('{');
                                for(u32 comp = 0; comp < buffer_view_components; ++comp){
                                        parse_array_object_key
                                        if(pla_str_is_equal(key, "buffer")){
                                                parse_value
                                                if(buffer) out_gltf->buffer_views[b].buffer = pla_str_to_s64(value);
                                        }else if(pla_str_is_equal(key, "byteLength")){
                                                parse_value
                                                if(buffer) out_gltf->buffer_views[b].byte_length = pla_str_to_s64(value);
                                        }else if(pla_str_is_equal(key, "byteOffset")){
                                                parse_value
                                                if(buffer) out_gltf->buffer_views[b].byte_offset = pla_str_to_s64(value);
                                        }else if(pla_str_is_equal(key, "byteStride")){
                                                parse_value
                                                if(buffer) out_gltf->buffer_views[b].byte_stride = pla_str_to_s64(value);
                                        }else if(pla_str_is_equal(key, "target")){
                                                parse_value
                                                if(buffer) out_gltf->buffer_views[b].target = pla_str_to_s64(value);
                                        }
                                        c = parse_til_next_symbol(jsize, jdata, c);
                                        if(jdata[c] != ',' && jdata[c] != '}') return pla_false;
                                }
                                c = parse_til_next_symbol(jsize, jdata, c);
                                if(jdata[c] != ',' && jdata[c] != ']') return pla_false;
                        }
                } else if(pla_str_is_equal(root_object, "buffers")){
                        parse_begining_of_root_array(buffers, buffer_count)
                        for(usize i = 0; i < buffer_count; ++i){
                                u32 buffer_components = 0;
                                if(try_count_items_in_array_or_object(jsize, jdata, c, &buffer_components) == SIZE_MAX) return pla_false;
                                parse('{');
                                for(u32 comp = 0; comp < buffer_components; ++comp){
                                        parse_array_object_key
                                        parse_string_value_from_key("uri", uri)
                                        parse_s64_value_from_key("byteLength", byte_length)

                                        if(buffer && pla_str_is_equal(key, "uri")){
                                                parse_value
                                                out_gltf->buffers[i].uri = value;
                                        }
                                        else if(buffer && pla_str_is_equal(key, "byteLength")){
                                                parse_value
                                                out_gltf->buffers[i].byte_length = pla_str_to_s64(value);
                                        }
                                        c = parse_til_next_symbol(jsize, jdata, c);
                                        if(jdata[c] != ',' && jdata[c] != '}') return pla_false;
                                }
                                c = parse_til_next_symbol(jsize, jdata, c);
                                if(jdata[c] != ',' && jdata[c] != ']') return pla_false;
                        }
                } else if(pla_str_is_equal(root_object, "meshes")){
                        pla_str name;
                        pla_mesh_primitive *primitives;
                                u8 attribute_count;
                                pla_mesh_primitive_attribute * attributes;
                                        u32 accessor;
                                        pla_mesh_primitive_attribute_name name;
                                        s8 set_index;
                                u32 indices;
                                u32 material;
                                u32 mode;
                        u8 primitive_count;
                } else if(pla_str_is_equal(root_object, "nodes")){
                        pla_str name;
                        u32 mesh;
                        u32 skin;
                        // f32 translation[3];
                        // f32 rotation[4];
                        // f32 scale[3];
                        f32 matrix[4 * 4];
                        u32 *children;
                        u8 child_count;
                } else if(pla_str_is_equal(root_object, "scene")){
                        parse_value
                        out_gltf->scene = pla_str_to_s64(value);
                } else if(pla_str_is_equal(root_object, "scenes")){
                        pla_str name;
                        u32 *nodes;
                        u8 node_count;
                }

                c = parse_til_next_symbol(jsize, jdata, c);
                if(jdata[c] != ',' && jdata[c] != '}') return pla_false;
        }





        return pla_true;
}

// inline CONSTEXPR pla_bool pla_parse_GLTF(u32 raw_gltf_size, u8 const *raw_gltf_data, pla_GLTF *gltf, pla_allocator allocator) NOEXCEPT{
//         if (allocator.allocate && allocator.free) gltf->allocator = allocator; 
//         // Allocator is required right now.
//         else return pla_false;
//         if (raw_gltf_size < 20) return pla_false;
//
//         pla_header header;
//         pla_chunk json_chunk;
//         pla_chunk binary_chunk;
//
//         memcpy(&header.magic, raw_gltf_data, 4);
//         if (header.magic != glTF) return pla_false;
//         memcpy(&header.version, raw_gltf_data + 4, 4);
//         memcpy(&header.length, raw_gltf_data + 8, 4);
//         memcpy(&json_chunk.length, raw_gltf_data + 12, 4);
//         memcpy(&json_chunk.type, raw_gltf_data + 16, 4);
//         if (json_chunk.type != JSON) return pla_false;
//         json_chunk.data = raw_gltf_data + 20;
//         memcpy(&binary_chunk.length, raw_gltf_data + 20 + json_chunk.length, 4);
//         memcpy(&binary_chunk.type, raw_gltf_data + 20 + json_chunk.length + 4, 4);
//         if (binary_chunk.type != BIN) return pla_false;
//         binary_chunk.data = raw_gltf_data + 20 + json_chunk.length + 8;
//         gltf->data = binary_chunk.data;
//         gltf->data_length = binary_chunk.length;
//
//         usize json_sybmol_count = pla_count_json_symbols(json_chunk.length, json_chunk.data);
//         pla_symbol *json_symbols = (pla_symbol *)gltf->allocator.allocate(gltf->allocator.user_data, json_sybmol_count * sizeof(pla_symbol));
//         u8 const **symbol_char_locations = (u8 const **)gltf->allocator.allocate(gltf->allocator.user_data, json_sybmol_count * sizeof(u8 const *));
//         pla_parse_json_symbols(json_chunk.length, json_chunk.data, json_sybmol_count, json_symbols, symbol_char_locations);
//
//         usize token_count = pla_count_json_tokens(json_sybmol_count, json_symbols);
//         pla_token *tokens = (pla_token *)gltf->allocator.allocate(gltf->allocator.user_data, token_count * sizeof(pla_token));
//         pla_str *token_values = (pla_str *)gltf->allocator.allocate(gltf->allocator.user_data, token_count * sizeof(pla_str));
//         memset(token_values, 0, token_count * sizeof(pla_str));
//         pla_parse_json_tokens(json_sybmol_count, json_symbols, symbol_char_locations, tokens, token_values);
//
//         u8 root_objects_to_parse_count = pla_root_object_count;
//         pla_root_object root_objects_to_parse[pla_root_object_count] = {
//             pla_root_accessors,
//             pla_root_asset,
//             pla_root_bufferViews,
//             pla_root_buffers,
//             pla_root_meshes,
//             pla_root_nodes,
//             pla_root_scene,
//             pla_root_scenes,
//         };
//
//         //TODO: maybe this would be improved if instead each if statment called some function on a gltf_parse_state struct and kept track of its object depth with a stack.
//         usize token_index = 0;
//         while (token_index < token_count) {
//                 //TODO: this code should never evaluate to pla_true.
//                 if (tokens[token_index] != pla_token_key){
//                         ++token_index;
//                         continue;
//                 }
//                 s8 maybe_root_object = pla_parse_next_root_object_key(&root_objects_to_parse_count, root_objects_to_parse, token_values[token_index]);
//                 if(maybe_root_object < 0){
//                         ++token_index; continue;
//                 }
//                 pla_root_object root_object = (pla_root_object)maybe_root_object;
//                 ++token_index;
//                 
//                 if(root_object == pla_root_asset){
//                         ++token_index;
//                         while(tokens[token_index] != pla_token_end_object){
//                                 if(tokens[token_index] == pla_token_key){
//                                         pla_check_key_is_string_and_assign_value(token_values, &token_index, "generator", &gltf->asset.generator);
//                                         pla_check_key_is_string_and_assign_value(token_values, &token_index, "version", &gltf->asset.version);
//                                 }
//                         }
//                         ++token_index;
//                 } else if(root_object == pla_root_bufferViews){
//                         ++token_index;
//                         gltf->buffer_view_count = pla_count_objects_til_end_of_array(token_count, tokens, token_index);
//                         gltf->buffer_views = (pla_buffer_view * )gltf->allocator.allocate(gltf->allocator.user_data, gltf->buffer_view_count * sizeof(pla_buffer_view));
//                         memset(gltf->buffer_views, 0, gltf->buffer_view_count * sizeof(pla_buffer_view));
//                         usize buffer_view_index = 0;
//                         while(tokens[token_index] != pla_token_end_array){
//                                 if(tokens[token_index] == pla_token_begin_object) ++token_index;
//                                 pla_check_key_is_string_and_assign_u32_value(token_values, &token_index, "buffer", &gltf->buffer_views[buffer_view_index].buffer);
//                                 pla_check_key_is_string_and_assign_u32_value(token_values, &token_index, "byteLength", &gltf->buffer_views[buffer_view_index].byte_length);
//                                 pla_check_key_is_string_and_assign_u32_value(token_values, &token_index, "byteOffset", &gltf->buffer_views[buffer_view_index].byte_offset);
//                                 pla_check_key_is_string_and_assign_u32_value(token_values, &token_index, "byteStride", &gltf->buffer_views[buffer_view_index].byte_stride);
//                                 pla_check_key_is_string_and_assign_u32_value(token_values, &token_index, "target", &gltf->buffer_views[buffer_view_index].target);
//                                 if(tokens[token_index] == pla_token_end_object)++buffer_view_index; 
//                                 ++token_index;
//                         }
//                         ++token_index;
//                 } else if(root_object == pla_root_buffers){
//                         ++token_index;
//                         gltf->buffer_count = pla_count_objects_til_end_of_array(token_count, tokens, token_index);
//                         gltf->buffers = (pla_buffer *)gltf->allocator.allocate(gltf->allocator.user_data, gltf->buffer_count * sizeof(pla_buffer));
//                         usize buffer_index = 0;
//                         while(tokens[token_index] != pla_token_end_array){
//                                 if(tokens[token_index] == pla_token_begin_object) ++token_index;
//                                 pla_check_key_is_string_and_assign_u64_value(token_values, &token_index, "byteLength", &gltf->buffers[buffer_index].byte_length);
//                                 if(pla_str_is_equal(token_values[token_index], "uri")){
//                                         ++token_index;
//                                         gltf->buffers[buffer_index].uri = token_values[token_index];
//                                         ++token_index;
//                                 }else{
//                                         pla_str uri = {.data = PLA_NULL, .length = 0};
//                                         gltf->buffers[buffer_index].uri = uri;
//                                 }
//                                 if(tokens[token_index] == pla_token_end_object) ++buffer_index;
//                                 ++token_index;
//                         }
//                 } else if(root_object == pla_root_meshes){
//                         ++token_index;
//                         gltf->mesh_count = pla_count_objects_til_end_of_array(token_count, tokens, token_index);
//                         gltf->meshes = (pla_mesh *)gltf->allocator.allocate(gltf->allocator.user_data, sizeof(pla_mesh) * gltf->mesh_count);
//                         memset(gltf->meshes, 0, sizeof(pla_mesh) * gltf->mesh_count);
//                         u32 mesh_index = 0;
//                         while(tokens[token_index] != pla_token_end_array){
//                                 if(tokens[token_index] == pla_token_begin_object) ++token_index;
//                                 pla_check_key_is_string_and_assign_value(token_values, &token_index, "name", &gltf->meshes[mesh_index].name);
//                                 if(pla_str_is_equal(token_values[token_index], "primitives")){
//                                         ++token_index;
//                                         gltf->meshes[mesh_index].primitive_count = pla_count_objects_til_end_of_array(token_count, tokens, token_index);
//                                         gltf->meshes[mesh_index].primitives = (pla_mesh_primitive *)gltf->allocator.allocate(gltf->allocator.user_data, sizeof(pla_mesh_primitive) * gltf->meshes[mesh_index].primitive_count);
//                                         memset(gltf->meshes[mesh_index].primitives, 0, sizeof(pla_mesh_primitive) * gltf->meshes[mesh_index].primitive_count);
//                                         ++token_index;
//                                         u8 primitive_index = 0;
//                                         while(tokens[token_index] != pla_token_end_array){
//                                                 if(tokens[token_index] == pla_token_begin_object)++token_index;
//                                                 if(pla_str_is_equal(token_values[token_index], "attributes")){
//                                                         token_index +=2;
//                                                         u32 attribute_count = 0;
//                                                         usize counting_index = token_index;
//                                                         while(tokens[counting_index] != pla_token_end_object){
//                                                                 if(tokens[counting_index] == pla_token_key){
//                                                                         ++attribute_count;
//                                                                         ++counting_index;
//                                                                 }
//                                                                 ++counting_index;
//                                                         }
//
//                                                         gltf->meshes[mesh_index].primitives[primitive_index].attribute_count = attribute_count;
//                                                         gltf->meshes[mesh_index].primitives[primitive_index].attributes = (pla_mesh_primitive_attribute *)gltf->allocator.allocate(gltf->allocator.user_data, sizeof(pla_mesh_primitive_attribute) * attribute_count);
//                                                         pla_mesh_primitive_attribute * attributes = gltf->meshes[mesh_index].primitives[primitive_index].attributes;
//                                                         memset(attributes, 0, sizeof(pla_mesh_primitive_attribute) * attribute_count);
//
//                                                         u8 attribute_index = 0;
//                                                         while(tokens[token_index] != pla_token_end_object){
//                                                                 if(pla_str_is_equal(token_values[token_index], "POSITION")){
//                                                                         ++token_index;
//                                                                         attributes[attribute_index].name = pla_POSITION;
//                                                                         attributes[attribute_index].set_index = -1;
//                                                                         attributes[attribute_index].accessor = pla_str_to_s64(token_values[token_index]);
//                                                                         ++attribute_index;
//                                                                        ++token_index; 
//                                                                 } 
//                                                                 if(pla_str_is_equal(token_values[token_index], "NORMAL")){
//                                                                         ++token_index;
//                                                                         attributes[attribute_index].name = pla_NORMAL;
//                                                                         attributes[attribute_index].set_index = -1;
//                                                                         attributes[attribute_index].accessor = pla_str_to_s64(token_values[token_index]);
//                                                                         ++attribute_index;
//                                                                         ++token_index;
//                                                                 } 
//                                                                 if(pla_str_is_equal(token_values[token_index], "TANGENT")){
//                                                                         ++token_index;
//                                                                         attributes[attribute_index].name = pla_TANGENT;
//                                                                         attributes[attribute_index].set_index = -1;
//                                                                         attributes[attribute_index].accessor = pla_str_to_s64(token_values[token_index]);
//                                                                         ++attribute_index;
//                                                                         ++token_index;
//                                                                 } 
//                                                                 s8 texcoord_set_index = pla_check_value_is_mesh_primitive_attribute_name_get_set_index(token_values[token_index], "TEXCOORD");
//                                                                 if(texcoord_set_index >=0 ){
//                                                                         ++token_index;
//                                                                         attributes[attribute_index].name = pla_TEXCOORD;
//                                                                         attributes[attribute_index].set_index = texcoord_set_index;
//                                                                         attributes[attribute_index].accessor = pla_str_to_s64(token_values[token_index]);
//                                                                         ++attribute_index;
//                                                                         ++token_index;
//                                                                 } 
//                                                                 s8 color_set_index = pla_check_value_is_mesh_primitive_attribute_name_get_set_index(token_values[token_index], "COLOR");
//                                                                 if(color_set_index >=0 ){
//                                                                         ++token_index;
//                                                                         attributes[attribute_index].name = pla_COLOR;
//                                                                         attributes[attribute_index].set_index = color_set_index;
//                                                                         attributes[attribute_index].accessor = pla_str_to_s64(token_values[token_index]);
//                                                                         ++attribute_index;
//                                                                         ++token_index;
//                                                                 } 
//                                                                 s8 joints_set_index = pla_check_value_is_mesh_primitive_attribute_name_get_set_index(token_values[token_index], "JOINTS");
//                                                                 if(joints_set_index >=0 ){
//                                                                         ++token_index;
//                                                                         attributes[attribute_index].name = pla_JOINTS;
//                                                                         attributes[attribute_index].set_index = joints_set_index;
//                                                                         attributes[attribute_index].accessor = pla_str_to_s64(token_values[token_index]);
//                                                                         ++attribute_index;
//                                                                         ++token_index;
//                                                                 } 
//                                                                 s8 weights_set_index = pla_check_value_is_mesh_primitive_attribute_name_get_set_index(token_values[token_index], "WEIGHTS");
//                                                                 if(weights_set_index >=0 ){
//                                                                         ++token_index;
//                                                                         attributes[attribute_index].name = pla_WEIGHTS;
//                                                                         attributes[attribute_index].set_index = weights_set_index;
//                                                                         attributes[attribute_index].accessor = pla_str_to_s64(token_values[token_index]);
//                                                                         ++attribute_index;
//                                                                         ++token_index;
//                                                                 } 
//                                                                 if(tokens[token_index] == pla_token_end_object){
//                                                                         break;
//                                                                 }
//                                                                 ++token_index;
//                                                         }
//                                                         ++token_index;
//                                                 }
//                                                 pla_check_key_is_string_and_assign_u32_value(token_values, &token_index, "indices", &gltf->meshes[mesh_index].primitives[primitive_index].indices);
//                                                 pla_check_key_is_string_and_assign_u32_value(token_values, &token_index, "material", &gltf->meshes[mesh_index].primitives[primitive_index].material);
//                                                 pla_check_key_is_string_and_assign_u32_value(token_values, &token_index, "mode", &gltf->meshes[mesh_index].primitives[primitive_index].mode);
//                                                 if(tokens[token_index] == pla_token_end_object){
//                                                         ++primitive_index;
//                                                 }
//                                                 ++token_index;
//                                         }
//                                         ++token_index;
//                                 }
//                                 if(pla_str_is_equal(token_values[token_index], "weights")){
//
//                                 }
//                                 if(tokens[token_index] == pla_token_end_object) ++mesh_index;
//                                 ++token_index;
//                         }
//                         ++token_index;
//                 } else if(root_object == pla_root_nodes){
//                         ++token_index;
//                         gltf->node_count = pla_count_objects_til_end_of_array(token_count, tokens, token_index);
//                         gltf->nodes = (pla_node *)gltf->allocator.allocate(gltf->allocator.user_data, sizeof(pla_node) * gltf->node_count); 
//                         memset(gltf->nodes, 0, sizeof(pla_node) * gltf->node_count);
//                         u32 node_index =0;
//                         while(tokens[token_index] != pla_token_end_array){
//                                 if(tokens[token_index] == pla_token_begin_object)++token_index;
//                                 pla_check_key_is_string_and_assign_u32_value(token_values, &token_index, "mesh", &gltf->nodes[node_index].mesh);
//                                 pla_check_key_is_string_and_assign_u32_value(token_values, &token_index, "skin", &gltf->nodes[node_index].mesh);
//                                 pla_check_key_is_string_and_assign_value(token_values, &token_index, "name", &gltf->nodes[node_index].name);
//                                 if(tokens[token_index] == pla_token_end_object)++node_index;
//                                 ++token_index;
//                         }
//                         ++token_index;
//                 } else if(root_object == pla_root_scene){
//                         gltf->scene = pla_str_to_s64(token_values[token_index]);
//                         ++token_index;
//                 } else if(root_object == pla_root_scenes){
//                         ++token_index;
//                         gltf->scene_count = pla_count_objects_til_end_of_array(token_count, tokens, token_index);
//                         gltf->scenes = (pla_scene *)gltf->allocator.allocate(gltf->allocator.user_data, sizeof(pla_scene) * gltf->scene_count);
//                         usize scene_index = 0;
//                         for(;tokens[token_index] != pla_token_end_array; ++token_index){
//                                 if(tokens[token_index] == pla_token_begin_object) ++token_index;
//                                 if(tokens[token_index] == pla_token_key){
//                                         pla_check_key_is_string_and_assign_value(token_values, &token_index, "name", &gltf->scenes[scene_index].name );
//                                         if(pla_str_is_equal(token_values[token_index], "nodes")){
//                                                 token_index+=2;
//                                                 usize node_count = pla_count_tokens_til_end_of_array(token_count, tokens, token_index);
//                                                 gltf->scenes[scene_index].node_count = node_count;
//                                                 gltf->scenes[scene_index].nodes = (u32 *)gltf->allocator.allocate(gltf->allocator.user_data, sizeof(u32) * node_count);
//                                                 memset(gltf->scenes[scene_index].nodes, 0, sizeof(u32) * node_count);
//                                                 for(usize node_index = 0; node_index < node_count; ++node_index){
//                                                         gltf->scenes[scene_index].nodes[node_index] = pla_str_to_s64(token_values[token_index]);
//                                                         ++token_index;
//                                                 }
//                                                 ++token_index;
//                                         }
//                                 }
//                         }
//                 } else if (root_object == pla_root_accessors) {
//                         ++token_index;
//                         gltf->accessor_count = pla_count_objects_til_end_of_array(token_count, tokens, token_index);
//                         gltf->accessors = (pla_accessor *) gltf->allocator.allocate(gltf->allocator.user_data, sizeof(pla_accessor) * gltf->accessor_count);
//                         memset(gltf->accessors, 0, sizeof(pla_accessor) * gltf->accessor_count);
//                         u32 accessor_index = 0;
//                         s64 min_array_begin_index =-1; 
//                         s64 max_array_begin_index =-1; 
//                         pla_bool has_component_type = pla_false;
//                         pla_bool has_type = pla_false;
//                         while(tokens[token_index] != pla_token_end_array){
//                                 if(tokens[token_index] == pla_token_begin_object) ++token_index;
//                                 if(tokens[token_index] == pla_token_key){
//                                         pla_check_key_is_string_and_assign_u32_value(token_values, &token_index, "bufferView", &gltf->accessors[accessor_index].buffer_view);
//                                         if(pla_str_is_equal(token_values[token_index], "componentType")){
//                                                 ++token_index;
//                                                 s8 maybe_component_type = lookup_component_type(token_values[token_index]);
//                                                 if(maybe_component_type >= 0){
//                                                         has_component_type = pla_true;
//                                                         gltf->accessors[accessor_index].component_type = (pla_GLTF_component_type)maybe_component_type;
//                                                 }else{
//                                                         //TODO: log this is bad somewhere.
//                                                 }
//                                                 ++token_index;
//                                         }
//                                         pla_check_key_is_string_and_assign_u32_value(token_values, &token_index, "count", &gltf->accessors[accessor_index].count);
//                                         if(pla_str_is_equal(token_values[token_index], "max")){
//                                                 ++token_index;
//                                                 max_array_begin_index = token_index;
//                                                 while(tokens[token_index] != pla_token_end_array) ++token_index;
//                                                 ++token_index;
//                                         }  
//                                         if(pla_str_is_equal(token_values[token_index], "min")){
//                                                 ++token_index;
//                                                 min_array_begin_index = token_index;
//                                                 while(tokens[token_index] != pla_token_end_array) ++token_index;
//                                                 ++token_index;
//                                         } 
//                                         if(pla_str_is_equal(token_values[token_index], "type")){
//                                                 ++token_index;
//                                                 s8 maybe_type = lookup_pla_GLTF_component(token_values[token_index]);
//                                                 if(maybe_type >= 0){
//                                                         gltf->accessors[accessor_index].type = (pla_GLTF_type)maybe_type;
//                                                         has_type = pla_true;
//                                                 }else{
//                                                         //TODO: log this error.
//                                                 }
//                                                 ++token_index;
//                                         }
//                                 }  
//                                 if(tokens[token_index] == pla_token_end_object){
//                                         if(has_component_type && has_type && min_array_begin_index >= 0 && max_array_begin_index >= 0){
//                                                 //TODO: figure out wtf the padding bullshit was that they were talking about in the spec.
//                                                 usize component_byte_count = pla_GLTF_component_type_byte_count[gltf->accessors[accessor_index].component_type];
//                                                 usize type_component_count = pla_GLTF_type_component_count[gltf->accessors[accessor_index].type];
//                                                 usize total_byte_count = type_component_count * component_byte_count;
//                                                 gltf->accessors[accessor_index].min_values = gltf->allocator.allocate(gltf->allocator.user_data, total_byte_count);
//                                                 gltf->accessors[accessor_index].max_values = gltf->allocator.allocate(gltf->allocator.user_data, total_byte_count);
//                                                 usize min_token_index = min_array_begin_index+1;
//                                                 usize max_token_index = max_array_begin_index+1;
//                                                 switch(gltf->accessors[accessor_index].component_type){
//                                                         case pla_GLTF_component_type_none:{
//                                                         //TODO: error
//                                                         break;
//                                                         } 
//                                                         case pla_GLTF_component_type_s8:
//                                                         case pla_GLTF_component_type_u8:{
//                                                                 for(usize i = 0; i < type_component_count; ++i){
//                                                                         ((s8 *)gltf->accessors[accessor_index].min_values)[i] = (s8)pla_str_to_s64(token_values[min_token_index]);
//                                                                         ((s8 *)gltf->accessors[accessor_index].max_values)[i] = (s8)pla_str_to_s64(token_values[max_token_index]);
//                                                                         ++min_token_index;
//                                                                         ++max_token_index;
//                                                                 }
//                                                                 break;
//                                                         }
//                                                         case pla_GLTF_component_type_s16:
//                                                         case pla_GLTF_component_type_u16:{
//                                                                 for(usize i = 0; i < type_component_count; ++i){
//                                                                         ((s16 *)gltf->accessors[accessor_index].min_values)[i] = (s16)pla_str_to_s64(token_values[min_token_index]);
//                                                                         ((s16 *)gltf->accessors[accessor_index].max_values)[i] = (s16)pla_str_to_s64(token_values[max_token_index]);
//                                                                         ++min_token_index;
//                                                                         ++max_token_index;
//                                                                 }
//                                                                 break;
//                                                         }
//                                                         case pla_GLTF_component_type_u32:{
//                                                                 for(usize i = 0; i < type_component_count; ++i){
//                                                                         ((s32 *)gltf->accessors[accessor_index].min_values)[i] = (u32)pla_str_to_s64(token_values[min_token_index]);
//                                                                         ((s32 *)gltf->accessors[accessor_index].max_values)[i] = (u32)pla_str_to_s64(token_values[max_token_index]);
//                                                                         ++min_token_index;
//                                                                         ++max_token_index;
//                                                                 }
//                                                                 break;
//                                                         }
//                                                         case pla_GLTF_component_type_f32:{
//                                                                 for(usize i = 0; i < type_component_count; ++i){
//                                                                         ((s32 *)gltf->accessors[accessor_index].min_values)[i] = pla_str_to_f32(token_values[min_token_index]);
//                                                                         ((s32 *)gltf->accessors[accessor_index].max_values)[i] = pla_str_to_f32(token_values[max_token_index]);
//                                                                         ++min_token_index;
//                                                                         ++max_token_index;
//                                                                 }
//                                                                 break;
//                                                         }
//                                                 }
//                                         }else{
//                                                 //TODO: error incomplete data to parse max and min values.
//                                         }
//                                         min_array_begin_index =-1; 
//                                         max_array_begin_index =-1; 
//                                         has_component_type = pla_false;
//                                         has_type = pla_false;
//                                         ++accessor_index;
//                                 } 
//                                 ++token_index;
//                         }
//                         ++token_index;
//                 }
//         }
//
//         gltf->allocator.free(gltf->allocator.user_data, token_values);
//         gltf->allocator.free(gltf->allocator.user_data, tokens);
//         gltf->allocator.free(gltf->allocator.user_data, symbol_char_locations);
//         gltf->allocator.free(gltf->allocator.user_data, json_symbols);
//         return pla_true;
// }

// inline CONSTEXPR void pla_free_GLTF(pla_GLTF *gltf) NOEXCEPT {
//         if(gltf->scenes){
//                 for(u32 scene_index = 0; scene_index < gltf->scene_count; ++scene_index){
//                         if(gltf->scenes[scene_index].nodes) gltf->allocator.free(gltf->allocator.user_data, gltf->scenes[scene_index].nodes);
//                 }
//                 gltf->allocator.free(gltf->allocator.user_data, gltf->scenes);
//         }
//         if(gltf->nodes){
//                 for(u32 node_index = 0; node_index < gltf->node_count; ++node_index){
//                         if(gltf->nodes[node_index].children) gltf->allocator.free(gltf->allocator.user_data, gltf->nodes[node_index].children);
//                 }
//                 gltf->allocator.free(gltf->allocator.user_data, gltf->nodes);
//         }
//         if(gltf->meshes){
//                 for(u32 mesh_index = 0; mesh_index < gltf->mesh_count; ++mesh_index){
//                         if(gltf->meshes[mesh_index].primitives){
//                                 for(u8 primitive_index = 0; primitive_index < gltf->meshes[mesh_index].primitive_count; ++primitive_index){
//                                         if(gltf->meshes[mesh_index].primitives[primitive_index].attributes){
//                                                 gltf->allocator.free(gltf->allocator.user_data, gltf->meshes[mesh_index].primitives[primitive_index].attributes);
//                                         }
//                                 }
//                                 gltf->allocator.free(gltf->allocator.user_data, gltf->meshes[mesh_index].primitives);
//                         }
//                 }
//                 gltf->allocator.free(gltf->allocator.user_data, gltf->meshes);
//         }
//         if(gltf->accessors){
//                 for(u32 accessor_index = 0; accessor_index < gltf->accessor_count; ++accessor_index){
//                         if(gltf->accessors[accessor_index].min_values){
//                                 gltf->allocator.free(gltf->allocator.user_data, gltf->accessors[accessor_index].min_values);
//                         }
//                         if(gltf->accessors[accessor_index].max_values){
//                                 gltf->allocator.free(gltf->allocator.user_data, gltf->accessors[accessor_index].max_values);
//                         }
//                 }
//                 gltf->allocator.free(gltf->allocator.user_data, gltf->accessors);
//
//         }
//         if(gltf->buffer_views) gltf->allocator.free(gltf->allocator.user_data, gltf->buffer_views);
//         if(gltf->buffers) gltf->allocator.free(gltf->allocator.user_data, gltf->buffers);
// }

#ifdef __cplusplus
}
#endif
