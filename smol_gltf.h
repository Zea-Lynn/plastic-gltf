#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
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

inline bool smol_str_is_equal(smol_str stra, char const *const strb) {
        for(size_t i = 0; i < stra.length; ++i){
                if(stra.data[i] != strb[i]) return false;
        }
        return true;
}

inline bool smol_str_in(smol_str str, char const * const * strs, size_t count){
        for(size_t i = 0; i < count; ++i){
                if(smol_str_is_equal(str, strs[i])) return true;
        }
        return false;
}


inline s64 smol_str_to_s64(smol_str str){
        if(str.length > 20) return 0;
        s64 value = 0;
        s64 sign = 1;
        if(str.data[0] == '-') sign = -1;
        for(u64 position = 1; position < str.length; ++position){
                u64 digit = str.data[position] - '0';
                bool is_valid = digit >= 0 || digit <= 9;
                value = value * 10 + (digit * is_valid);
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
        POSITION,
        NORMAL,
        TANGENT,
        TEXCOORD_0,
        COLOR_0,
        JOINTS_0,
        WEIGHTS_0,
}smol_mesh_primitive_attribute_name;

typedef struct smol_mesh_primitive_attribute{
        u32 accessor;
        smol_mesh_primitive_attribute_name name;
}smol_mesh_primitive_attribute;

typedef struct smol_mesh_primitive{
        u8 attribute_count;
        smol_mesh_primitive_attribute attributes;
        u32 indices;
        u32 material;
        u32 mode;
}smol_mesh_primitive;

typedef struct smol_mesh {
        smol_str name;
        smol_mesh_primitive *primitives;
        u8 primitive_count;
} smol_mesh;

typedef enum smol_GLTF_component_type : u8 {
        smol_GLTF_component_type_s8,
        smol_GLTF_component_type_u8,
        smol_GLTF_component_type_s16,
        smol_GLTF_component_type_u16,
        smol_GLTF_component_type_u32,
        smol_GLTF_component_type_f32,
} smol_GLTF_component_type;

char const * const smol_GLTF_component_type_strings[6] = {"5120","5121","5122","5123","5125","5126"};

//returns -1 if the string is not a valid type code.
inline s8 lookup_component_type(smol_str value){
        for(size_t i = 0; i < 6; ++i){
                if(smol_str_is_equal(value, smol_GLTF_component_type_strings[i])) return i;
        }
        return -1;
}

u8 smol_GLTF_component_type_byte_count[6] = {1,1,2,2,4,4};

typedef enum smol_GLTF_type: u8{
        smol_GLTF_component_SCALAR,
        smol_GLTF_component_VEC2,
        smol_GLTF_component_VEC3,
        smol_GLTF_component_VEC4,
        smol_GLTF_component_MAT2,
        smol_GLTF_component_MAT3,
        smol_GLTF_component_MAT4,
} smol_GLTF_type;

char const * const smol_GLTF_type_strings[8] = { "SCALAR", "VEC2", "VEC3", "VEC4", "MAT2", "MAT3", "MAT4" };

//returns -1 if the string is not a valid component.
inline s8 lookup_smol_GLTF_component(smol_str str){
        for(size_t i = 0; i < 8; ++i){
                if(smol_str_is_equal(str, smol_GLTF_type_strings[i])) return i;
        }
        return -1;
}

u8 smol_GLTF_component_element_count[8] = {1,2,3,4,4,9,16};

typedef struct smol_accessors {
        smol_GLTF_component_type * component_types;
        smol_GLTF_type * types;
        u32 * buffer_views;
        u32 * byte_offsets;
        //must be cast based off the components array;
        void ** min_values;
        void ** max_values;
        u32 * counts;
} smol_accessors;

typedef struct smol_buffer_view {
        u32 buffer;
        u32 byte_length;
        u32 byte_offset;
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
        // smol_accessor *accessors;
        smol_accessors accessors;
        smol_buffer_view *buffer_views;
        smol_buffer *buffers;
        u32 accessor_count;
        u32 scene_count;
        u32 node_count;
        u32 mesh_count;
        u32 buffer_view_count;
        u32 buffer_count;
} smol_GLTF;

inline void smol_allocate_accessors(smol_GLTF * gltf){
        gltf->accessors.component_types = (smol_GLTF_component_type * )gltf->allocator.allocate(sizeof(smol_GLTF_component_type) * gltf->accessor_count);
        gltf->accessors.types = (smol_GLTF_type*)gltf->allocator.allocate(sizeof(smol_GLTF_type) * gltf->accessor_count);
        gltf->accessors.buffer_views = (u32*)gltf->allocator.allocate(sizeof(u32) * gltf->accessor_count);
        gltf->accessors.byte_offsets = (u32*)gltf->allocator.allocate(sizeof(u32) * gltf->accessor_count);
        gltf->accessors.min_values = (void**)gltf->allocator.allocate(sizeof(void *) * gltf->accessor_count);
        gltf->accessors.max_values = (void**)gltf->allocator.allocate(sizeof(void *) * gltf->accessor_count);
        for(size_t i = 0; i < gltf->accessor_count; ++i){
                gltf->accessors.min_values[i] = NULL;
                gltf->accessors.max_values[i] = NULL;
        }
}

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

static const char *smol_root_object_names[smol_root_object_count] = {
    "accessors",
    "asset",
    "bufferViews",
    "buffers",
    "meshes",
    "nodes",
    "scene",
    "scenes",
};

inline u64 smol_count_json_symbols(u64 json_size, u8 const *json) {
        u64 token_count;
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

inline void smol_parse_json_symbols(u64 json_size, u8 const * const json, u64 symbol_count, smol_symbol *symbols, u8 const **symbol_chars) {
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

inline size_t smol_count_json_tokens(size_t symbol_count, smol_symbol const *const symbols) {
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
                        if(symbols[i+1] == smol_symbol_comma){
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
                        if (next_symbol == smol_symbol_comma || next_symbol == smol_symbol_close_squigily) {
                                ++token_count;
                                ++i;
                        } else if (next_symbol == smol_symbol_begin_string)
                                continue;
                }else if(symbol == smol_symbol_colon && in_value_array){
                        ++token_count;
                }
        }
        return token_count;
}

typedef enum smol_json_parse_stack_type: u8{
        smol_none,
        smol_object,
        smol_array,
        smol_value_array,
        smol_single_or_empty_array,
}smol_json_parse_stack_type;

inline void smol_parse_json_tokens(size_t symbol_count, smol_symbol const *const symbols, u8 const *const *const symbol_locations, smol_token *tokens, smol_str *token_values) {
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
                        if (symbols[i+1] == smol_symbol_comma || symbols[i+1] == smol_symbol_close_squigily) {
                                tokens[token_index] = smol_token_value;
                                token_values[token_index].data = symbol_locations[i] + 1;
                                token_values[token_index].length = symbol_locations[i + 1] - token_values[token_index].data;
                                ++i;
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

inline size_t smol_count_objects_til_end_of_array(size_t token_count, smol_token const * tokens, size_t current_token_index){
        size_t object_count = 0;
        s64 nested_object_count = -1;
        do{
                if(tokens[current_token_index] == smol_token_begin_object){
                        ++nested_object_count;
                }else if(tokens[current_token_index] == smol_token_end_object){
                        --nested_object_count;
                        if(nested_object_count < 0) ++object_count;
                }else if(tokens[current_token_index] == smol_token_end_array  && nested_object_count < 0){
                        break;
                }
                ++current_token_index;
        }while(current_token_index < token_count);

        return object_count;
}

inline size_t smol_count_tokens_til_end_of_array(size_t token_count, smol_token const * tokens, size_t current_token_index){
        size_t starting_token_index = current_token_index;
        do ++current_token_index; while(tokens[current_token_index] != smol_token_end_array);
        return current_token_index - starting_token_index;
}

//TODO: decrease count and remove object from array by swapping it to the end.
inline s8 smol_parse_next_root_object_key(u8 * root_objects_to_parse_count, smol_root_object * root_objects_to_parse, smol_str token_value){
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
};

inline void smol_check_key_is_string_and_assign_value(smol_str const * token_values, size_t * token_index, char const * test_str,smol_str * assign_location){
        if(smol_str_is_equal(token_values[*token_index], test_str)){
                ++*token_index;
                *assign_location = token_values[*token_index];
                ++*token_index;
        }
}

inline void check_key_is_string_and_assign_s64_value(smol_str const * token_values, size_t * token_index, char const * test_str,s64 * assign_location){
        if(smol_str_is_equal(token_values[*token_index], test_str)){
                ++*token_index;
                *assign_location = smol_str_to_s64(token_values[*token_index]);
                ++*token_index;
        }
}

inline void smol_check_key_is_string_and_assign_u32_value(smol_str const * token_values, size_t * token_index, char const * test_str,u32 * assign_location){
        if(smol_str_is_equal(token_values[*token_index], test_str)){
                ++*token_index;
                *assign_location = smol_str_to_s64(token_values[*token_index]);
                ++*token_index;
        }
}

inline bool smol_parse_GLTF(u32 raw_gltf_size, u8 const *raw_gltf_data, smol_GLTF *gltf, smol_allocator allocator) {
        if (allocator.allocate && allocator.free) {
                gltf->allocator.allocate = allocator.allocate;
                gltf->allocator.free = allocator.free;
        } else {
                // Allocator is required right now.
                return false;
        }

        if (raw_gltf_size < 20)
                return false;

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

        size_t json_sybmol_count = smol_count_json_symbols(json_chunk.length, json_chunk.data);
        smol_symbol *json_symbols = (smol_symbol *)gltf->allocator.allocate(json_sybmol_count * sizeof(smol_symbol));
        u8 const **symbol_char_locations = (u8 const **)gltf->allocator.allocate(json_sybmol_count * sizeof(u8 const *));
        smol_parse_json_symbols(json_chunk.length, json_chunk.data, json_sybmol_count, json_symbols, symbol_char_locations);

        size_t token_count = smol_count_json_tokens(json_sybmol_count, json_symbols);
        smol_token *tokens = (smol_token *)gltf->allocator.allocate(token_count * sizeof(smol_token));
        smol_str *token_values = (smol_str *)gltf->allocator.allocate(token_count * sizeof(smol_str));
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

        size_t token_index = 0;
        while (token_index < token_count) {
                //TODO: this code should never evaluate to true.
                if (tokens[token_index] != smol_token_key){
                        ++token_index;
                        continue;
                }
                s8 maybe_root_object = smol_parse_next_root_object_key(&root_objects_to_parse_count, root_objects_to_parse, token_values[token_index]);
                if(maybe_root_object < 0){
                        ++token_index;
                        continue;
                }
                smol_root_object root_object = (smol_root_object)maybe_root_object;
                
                if(root_object == smol_root_asset){
                        token_index+=2;
                        while(tokens[token_index] != smol_token_end_object){
                                if(tokens[token_index] == smol_token_key){
                                        if(smol_str_is_equal(token_values[token_index], "generator")){
                                                ++token_index;
                                                gltf->asset.generator = token_values[token_index]; 
                                                ++token_index;
                                        }
                                        if(smol_str_is_equal(token_values[token_index], "version")){
                                                ++token_index;
                                                gltf->asset.version = token_values[token_index];
                                                ++token_index;
                                        }
                                }
                        }
                        ++token_index;
                } else if(root_object == smol_root_bufferViews){
                        token_index+=2;
                        gltf->buffer_view_count = smol_count_objects_til_end_of_array(token_count, tokens, token_index);
                        gltf->buffer_views = (smol_buffer_view * )gltf->allocator.allocate(gltf->buffer_view_count * sizeof(smol_buffer_view));
                        size_t buffer_view_index = 0;
                        while(tokens[token_index] != smol_token_end_array){
                                if(tokens[token_index] == smol_token_begin_object) ++token_index;
                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "buffer", &gltf->buffer_views[buffer_view_index].buffer);
                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "byteLength", &gltf->buffer_views[buffer_view_index].byte_length);
                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "byteOffset", &gltf->buffer_views[buffer_view_index].byte_offset);
                                if(tokens[token_index] == smol_token_end_object)++buffer_view_index; 
                                ++token_index;
                        }
                        ++token_index;
                } else if(root_object == smol_root_buffers){
                        token_index+=2;
                        gltf->buffer_count = smol_count_objects_til_end_of_array(token_count, tokens, token_index);
                        gltf->buffers = (smol_buffer *)gltf->allocator.allocate(gltf->buffer_count * sizeof(smol_buffer));
                        size_t buffer_index = 0;
                } else if(root_object == smol_root_meshes){
                        token_index+=2;
                        gltf->mesh_count = smol_count_objects_til_end_of_array(token_count, tokens, token_index);
                        gltf->meshes = (smol_mesh *)gltf->allocator.allocate(sizeof(smol_mesh) * gltf->mesh_count);
                        u32 mesh_index = 0;
                        while(tokens[token_index] != smol_token_end_array){
                                if(tokens[token_index] == smol_token_begin_object) ++token_index;
                                if(smol_str_is_equal(token_values[token_index], "primitives")){
                                        ++token_index;
                                        gltf->meshes[mesh_index].primitive_count = smol_count_objects_til_end_of_array(token_count, tokens, token_index);
                                        gltf->meshes[mesh_index].primitives = (smol_mesh_primitive *)gltf->allocator.allocate(sizeof(smol_mesh_primitive) * gltf->meshes[mesh_index].primitive_count);
                                        ++token_index;
                                        for(u8 primitive_index = 0; tokens[token_index] != smol_token_end_array; ++token_index){
                                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "indices", &gltf->meshes[mesh_index].primitives[primitive_index].indices);
                                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "material", &gltf->meshes[mesh_index].primitives[primitive_index].material);
                                                smol_check_key_is_string_and_assign_u32_value(token_values, &token_index, "mode", &gltf->meshes[mesh_index].primitives[primitive_index].mode);
                                                if(smol_str_is_equal(token_values[token_index], "attributes")){
                                                        ++token_index;
                                                        //TODO: figure this weird thing out.
                                                        for(;tokens[token_index] != smol_token_end_object;++token_index){
                                                                if(smol_str_is_equal(token_values[token_index], "NORMAL")){
                                                                        token_index+=2;
                                                                }
                                                        }
                                                }
                                                // if(smol_str_cmp(token_values[token_index], "indices")){
                                                //         ++token_index;
                                                //         gltf->meshes[mesh_index].primitives[primitive_index].indices = smol_str_to_s64(token_values[token_index]);
                                                //         ++token_index;
                                                // }
                                                // if(smol_str_cmp(token_values[token_index], "material")){
                                                //         ++token_index;
                                                //         gltf->meshes[mesh_index].primitives[primitive_index].material = smol_str_to_s64(token_values[token_index]);
                                                //         ++token_index;
                                                // }
                                                // if(smol_str_cmp(token_values[token_index], "mode")){
                                                //         ++token_index;
                                                //         gltf->meshes[mesh_index].primitives[primitive_index].mode = smol_str_to_s64(token_values[token_index]);
                                                //         ++token_index;
                                                // }

                                                if(tokens[token_index] == smol_token_end_object){
                                                        ++primitive_index;
                                                }
                                        }
                                }
                                if(smol_str_is_equal(token_values[token_index], "weights")){

                                }
                                if(tokens[token_index] == smol_token_end_object){
                                        ++mesh_index;
                                }
                                ++token_index;
                        }
                        ++token_index;
                } else if(root_object == smol_root_nodes){
                        token_index+=2;
                        gltf->node_count = smol_count_objects_til_end_of_array(token_count, tokens, token_index);
                        gltf->nodes = (smol_node *)gltf->allocator.allocate(sizeof(smol_node) * gltf->node_count); 
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
                        ++token_index;
                        gltf->scene = smol_str_to_s64(token_values[token_index]);
                        ++token_index;
                } else if(root_object == smol_root_scenes){
                        token_index+=2;
                        gltf->scene_count = smol_count_objects_til_end_of_array(token_count, tokens, token_index);
                        gltf->scenes = (smol_scene *)gltf->allocator.allocate(sizeof(smol_scene) * gltf->scene_count);
                        size_t scene_index = 0;
                        bool in_object = 0;
                        while(tokens[token_index] != smol_token_end_array){
                                if(tokens[token_index] == smol_token_begin_object){
                                        in_object = true;
                                        ++token_index;
                                }
                                if(tokens[token_index] == smol_token_key){
                                        if(smol_str_is_equal(token_values[token_index], "name")){
                                                ++token_index;
                                                gltf->scenes[scene_index].name = token_values[token_index];
                                                ++token_index;
                                        }
                                        if(smol_str_is_equal(token_values[token_index], "nodes")){
                                                token_index+=2;
                                                size_t node_count = smol_count_tokens_til_end_of_array(token_count, tokens, token_index);
                                                gltf->scenes[scene_index].node_count = node_count;
                                                gltf->scenes[scene_index].nodes = (u32 *)gltf->allocator.allocate(sizeof(u32) * node_count);
                                                for(size_t node_index = 0; node_index < node_count; ++node_index){
                                                        gltf->scenes[scene_index].nodes[node_index] = smol_str_to_s64(token_values[token_index]);
                                                        ++token_index;
                                                }
                                                ++token_index;
                                        }

                                }
                                if(tokens[token_index] == smol_token_end_object){
                                        in_object = false;
                                        ++token_index;
                                }
                        }
                        ++token_index;
                } else if (root_object == smol_root_accessors) {
                        gltf->accessor_count = 0;
                        // Should be a begin object after begin array.
                        token_index += 2;
                        {
                                size_t couting_index = token_index;
                                bool in_object;
                                while (tokens[couting_index] != smol_token_end_array) {
                                        if (!in_object && tokens[couting_index] == smol_token_begin_object)
                                                in_object = true;
                                        else if (in_object && tokens[couting_index] == smol_token_end_object) {
                                                ++gltf->accessor_count;
                                                in_object = false;
                                        }
                                        ++couting_index;
                                }
                                smol_allocate_accessors(gltf);
                        }
                        u32 accessor_index = 0;
                        bool in_object;
                        s32 min_index = -1;
                        s32 max_index = -1;
                        bool has_type = false;
                        bool has_component_type = false;

                        while(tokens[token_index] != smol_token_end_array){
                                if (!in_object && tokens[token_index] == smol_token_begin_object)
                                        in_object = true;
                                else if (in_object) {
                                        if(tokens[token_index] == smol_token_key){
                                                if(strcmp("bufferView", (char const *)token_values[token_index].data)){
                                                        ++token_index;
                                                        gltf->accessors.buffer_views[accessor_index] = smol_str_to_s64(token_values[token_index]);
                                                        ++token_index;
                                                } else if(strcmp("componentType", (char const *)token_values[token_index].data)){
                                                        ++token_index;
                                                        s8 maybe_component_type = lookup_component_type(token_values[token_index]);
                                                        if(maybe_component_type >= 0){
                                                                has_component_type = true;
                                                                gltf->accessors.component_types[accessor_index] = (smol_GLTF_component_type)maybe_component_type;
                                                        }else{
                                                                //TODO: log this is bad somewhere.
                                                        }
                                                        ++token_index;
                                                } else if(strcmp("count", (char const *)token_values[token_index].data)){
                                                        ++token_index;
                                                        gltf->accessors.counts[accessor_index] = smol_str_to_s64(token_values[token_index]);
                                                        ++token_index;
                                                } else if(strcmp("max", (char const *)token_values[token_index].data)){
                                                        max_index = token_index;
                                                        while(tokens[token_index] != smol_token_end_array) ++token_index;
                                                } else if(strcmp("min", (char const *)token_values[token_index].data)){
                                                        min_index = token_index;
                                                        while(tokens[token_index] != smol_token_end_array) ++token_index;
                                                } else if(strcmp("type", (char const *)token_values[token_index].data)){
                                                        ++token_index;
                                                        s8 maybe_type = lookup_smol_GLTF_component(token_values[token_index]);
                                                        if(maybe_type >= 0){
                                                                has_type = true;
                                                                gltf->accessors.types[accessor_index] = (smol_GLTF_type)maybe_type;
                                                        }else{
                                                                //TODO: log this error.
                                                        }
                                                        ++token_index;
                                                }

                                                // "componentType": 5126,
                                                // "count": 1984,
                                                // "max": [ 0.9999997019767761, 1, 0.9999993443489075 ],
                                                // "min": [ -0.9999990463256836, -1, -1 ],
                                                // "type": "VEC3"
                                        }
                                        if(tokens[token_index] == smol_token_end_object){
                                                if(min_index >= 0);
                                                if(max_index >= 0);
                                                ++accessor_index;
                                                ++token_index;
                                                //cleanup
                                                in_object = false;
                                                max_index = -1;
                                                min_index = -1;
                                                has_component_type = false;
                                                has_type = false;
                                        }
                                }
                        }
                }
        }

        gltf->allocator.free(token_values);
        gltf->allocator.free(tokens);
        gltf->allocator.free(symbol_char_locations);
        gltf->allocator.free(json_symbols);
        return true;
}

inline void smol_free_GLTF(smol_GLTF *gltf) {}

#ifdef __cplusplus
}
#endif
