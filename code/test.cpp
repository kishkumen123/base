#include "base_inc.h"
#include "win32_base_inc.h"

bool PRINT_ALL = 0;

Arena* fail_arena = {};

// maybe push String8 instead, not sure
static void
push_message(const char* message, u32 size){
    u8* message_string = (u8*)push_array(fail_arena, u8, size + 3);

    *message_string++ = ' ';
    *message_string++ = ' ';
    for(s32 i = 0; i < size; ++i){
        *message_string++ = *message++;
    }
    *message_string++ = '\n';
}


u32 fail_count = 0;
static void check(bool cond, s32 line, const char* msg, size_t size){
    if(!(cond)){
        fail_count++;
        push_message(msg, size);
    }
    if(PRINT_ALL){
        print("%d - %s - %s\n", line, (cond ? "SUCCEED" : " FAILED"), msg);
    }
}
#define eval(cond) check(cond, __LINE__, #cond, (sizeof(#cond) - 1))


s32 main(s32 argc, char** argv){

    String8 com_verbos = str8_literal("-v");
    String8 verbos;
    if(argc > 1){
        verbos = str8(argv[1], 2);
        if(com_verbos == verbos){
            PRINT_ALL = 1;
        }
    }

    // init arena for failed messages to print out at the end
	void* memory = calloc((KB(1) + sizeof(Arena)), 1);
	fail_arena = (Arena*)memory;
	fail_arena->base = (u8*)memory + sizeof(Arena);
	fail_arena->size = KB(1);
	fail_arena->used = 0;

    // base_types.h
    {
        //ArrayCount
        s32 array[10];
        eval(ArrayCount(array) == 10);

        // MIN - do I need to test every type?
        eval(MIN(1, 2) == 1);
        eval(MIN(2, 1) == 1);

        // MAX
        eval(MAX(1, 2) == 2);
        eval(MAX(2, 1) == 2);

        // CLAMP
        eval(CLAMP(2,5,4) == 4);
        eval(CLAMP(2,1,4) == 2);

        // ABS
        eval(ABS(-1) == 1);
        eval(ABS(-1.1) == 1.1);

        // KB/MB/GB/TB
        eval(KB(1) == 1024LL);
        eval(MB(1) == (1024LL * 1024LL));
        eval(GB(1) == (1024LL * 1024LL * 1024LL));
        eval(TB(1) == (1024LL * 1024LL * 1024LL * 1024LL));

        // HU/TH/MI/BI/TR
        eval(HUNDRED(1) == 100);
        eval(THOUSAND(1) == 1000);
        eval(MILLION(1) == 1000000);
        eval(BILLION(1) == 1000000000);
        eval(TRILLION(1) == 1000000000000);

        // type_min_max
        eval(s8_min == -128);
        eval(s16_min == -32768);
        eval(s32_min == -2147483648);
        eval(s64_min == -9223372036854775808ll);
        eval(s8_max == 127);
        eval(s16_max == 32767);
        eval(s32_max == 2147483647);
        eval(s64_max == 9223372036854775807ll);
        eval(u8_max == 255);
        eval(u16_max == 65535);
        eval(u32_max == 4294967295);
        eval(u64_max == 18446744073709551615ull);
    }

    // base_vector.h
    {
        // compound type operators
        v2s32 _v2s32 = {1, 2};
        eval((_v2s32 + _v2s32) == ((v2s32){2, 4}));
        eval((_v2s32 - _v2s32) == ((v2s32){0, 0}));
        eval((3 * _v2s32) == ((v2s32){3, 6}));
        eval((_v2s32 * 3) == ((v2s32){3, 6}));
        eval(_v2s32 == _v2s32);
        eval(!(_v2s32 != _v2s32));

        v2 _v2 = {1, 2};
        eval((_v2 + _v2) == ((v2){2, 4}));
        eval((_v2 - _v2) == ((v2){0, 0}));
        eval((3 * _v2) == ((v2){3, 6}));
        eval((_v2 * 3) == ((v2){3, 6}));
        eval(_v2 == _v2);
        eval(!(_v2 != _v2));

        v3 _v3 = {1, 2, 0.5f};
        eval((_v3 + _v3) == ((v3){2, 4, 1.0f}));
        eval((_v3 - _v3) == ((v3){0, 0, 0}));
        eval((3 * _v3) == ((v3){3, 6, 1.5f}));
        eval((_v3 * 3) == ((v3){3, 6, 1.5f}));
        eval(_v3 == _v3);
        eval(!(_v3 != _v3));

        v4 _v4 = {1, 2, 0.5f, 3};
        eval((_v4 + _v4) == ((v4){2, 4, 1.0f, 6}));
        eval((_v4 - _v4) == ((v4){0, 0, 0, 0}));
        eval((3 * _v4) == ((v4){3, 6, 1.5f, 9}));
        eval((_v4 * 3) == ((v4){3, 6, 1.5f, 9}));
        eval(_v4 == _v4);
        eval(!(_v4 != _v4));

        // vector2 math
        eval(round_v2(((v2){1.1f, 1.9f})) == ((v2){1.0f, 2.0f}));
        eval(round_v2_v2s32(((v2){1.1f, 1.9f})) == ((v2s32){1, 2}));
        eval(inner_product_v2((v2){1.0f, 0.0f}, (v2){0.0f, 1.0f}) == 0);
        eval(is_perpendicular_v2((v2){1.0f, 0.0f}, (v2){0.0f, 1.0f}) == 1);
        eval(right_direction_v2(((v2){1.0f, 0.5f}), ((v2){1.0f, 0.8f})) == 1);
        eval(same_direction_v2((v2){1.0f, 0.5f}, (v2){1.0f, 0.8f}) == 1);
        eval(left_direction_v2(((v2){1.0f, 0.5f}), ((v2){-1.0f, 0.8f})) == 1);
        eval(opposite_direction_v2((v2){1.0f, 0.5f}, (v2){-1.0f, 0.8f}) == 1);
        eval(magnitude_sqrt_v2((v2){3.0f, 0.0f}) == 9);
        eval(magnitude_v2((v2){3.0f, 0.0f}) == 3);
        eval(distance_v2((v2){0.0f, 0.0f}, (v2){3.0f, 3.0f}) == 4.2426405f);
        eval(distance_v2((v2){0.0f, 0.0f}, (v2){3.0f, 0.0f}) == 3);
        eval(normalized_v2((v2){100.0f, 200.0f}) == ((v2){0.44721359f, 0.89442718f}));
        eval(direction_v2((v2){0.0f, 0.0f}, (v2){3.0f, 3.0f}) == ((v2){0.707106829f, 0.707106829f}));
        eval(angle_v2((v2){1.0f, 0.0f}, (v2){-1.0f, 0.0f}) == PI_f32);
        eval(angle_v2((v2){1.0f, 0.0f}, (v2){0.0f, 1.0f}) == PI_f32/2);
        eval(angle_v2((v2){1.0f, 0.0f}, (v2){1.0f, 1.0f}) == (PI_f32)/4);
        eval(angle_v2((v2){1.0f, 0.0f}, (v2){1.0f, -1.0f}) == (PI_f32)/4);
        eval(project_v2((v2){0.707f, 0.707f}, (v2){0.5f, 0.0f}) == ((v2){0.707, 0.0f}));
        eval(perpendicular_v2((v2){0.707f, 0.707f}, (v2){0.5f, 0.0f}) == ((v2){0.0, 0.707}));
        eval(reflection_v2((v2){-1.0f, -1.0f}, (v2){1.0f, 0.0f}) == ((v2){1.0, -1.0}));
        eval(reflection_v2((v2){0.0f, 1.0f}, (v2){1.0f, 0.0f}) == ((v2){0.0, 1.0})); // QUESTION: but why
        eval(reflection_v2((v2){1.0f, 0.0f}, (v2){1.0f, 0.0f}) == ((v2){-1.0, 0.0}));

        // vector3 math
        //eval(round_v3(((v3){1.1f, 1.9f, 2.5f})) == ((v3){1.0f, 2.0f, 2.0f})); not implemented yet
        eval(inner_product_v3((v3){1.0f, 0.0f, 0.0f}, (v3){0.0f, 1.0f, 0.0f}) == 0);
        eval(is_perpendicular_v3((v3){1.0f, 0.0f, 0.0f}, (v3){0.0f, 1.0f, 0.0f}) == 1);
        eval(right_direction_v3(((v3){1.0f, 0.5f, 0.5f}), ((v3){1.0f, 0.8f, 0.5f})) == 1);
        eval(same_direction_v3((v3){1.0f, 0.5f, 0.5f}, (v3){1.0f, 0.8f, 0.3f}) == 1);
        eval(opposite_direction_v3((v3){1.0f, 0.5f, 0.5f}, (v3){-1.0f, 0.8f, 0.5f}) == 1);
        eval(left_direction_v3(((v3){1.0f, 0.5f, 0.5f}), ((v3){-1.0f, 0.8f, 0.5f})) == 1);
        eval(magnitude_sqrt_v3((v3){3.0f, 0.0f, 3.0f}) == 18);
        eval(magnitude_v3((v3){3.0f, 0.0f, 3.0f}) == 4.2426405f);
        eval(distance_v3((v3){0.0f, 0.0f, 0.0f}, (v3){3.0f, 3.0f, 3.0f}) == 5.19615221f);
        eval(distance_v3((v3){0.0f, 0.0f, 0.0f}, (v3){3.0f, 0.0f, 0.0f}) == 3);
        eval(normalized_v3((v3){100.0f, 200.0f, 400.0f}) == ((v3){0.218217894f, 0.436435789f, 0.872871578f}));
        eval(direction_v3((v3){0.0f, 0.0f, 0.0f}, (v3){3.0f, 3.0f, 3.0f}) == ((v3){0.577350259f, 0.577350259f, 0.577350259f}));
        eval(cross_product_v3((v3){1.0f, 0.0f, 0.0f}, (v3){0.0f, 1.0f, 0.0f}) == ((v3){0.0f, 0.0f, 1.0f}));
        eval(angle_v3((v3){1.0f, 0.0f, 0.0f}, (v3){-1.0f, 0.0f, 0.0f}) == PI_f32);
        eval(angle_v3((v3){1.0f, 0.0f, 0.0f}, (v3){0.0f, 1.0f, 0.0f}) == PI_f32/2);
        eval(angle_v3((v3){1.0f, 1.0f, 0.0f}, (v3){1.0f, 1.0f, 1.0f}) == 0.615479767f);
        eval(angle_v3((v3){1.0f, 0.0f, 0.0f}, (v3){1.0f, -1.0f, 1.0f}) == 0.955316603f);
        eval(project_v3((v3){0.707f, 0.707f, 0.707f}, (v3){0.5f, 0.0f, 0.0f}) == ((v3){0.707f, 0.0f, 0.0f}));
        eval(perpendicular_v3((v3){0.707f, 0.707f, 0.707f}, (v3){0.5f, 0.5f, 0.0f}) == ((v3){0.0f, 0.0f, 0.707}));
        eval(reflection_v3((v3){-1.0f, -1.0f, 0.0f}, (v3){1.0f, 0.0f}) == ((v3){1.0f, -1.0f, 0.0f}));
        eval(reflection_v3((v3){0.0f, 1.0f, 0.0f}, (v3){1.0f, 0.0f, 0.0f}) == ((v3){0.0f, 1.0f, 0.0f})); // QUESTION: but why
        eval(reflection_v3((v3){1.0f, 0.0f, 0.0f}, (v3){1.0f, 0.0f, 0.0f}) == ((v3){-1.0f, 0.0f, 0.0f}));
    }
    // base_math.h
    {
        // INCOMPLETE: These will become functions, test accordingly
        eval(AlignUpPow2(19, 4) == 20);
        eval(AlignDownPow2(19, 4) == 16);

        // trig
        eval(deg_to_rad(1) == 0.017453292f);
        eval(rad_to_deg(RAD_f32) == 1);
        eval(sin_f32(RAD_f32 * 90.0f) == 1.0f);
        eval(cos_f32(RAD_f32 * 90.0f) == 0.0000001f);
        eval(atan_f32(0.707f, 0.707f) == 1.0f); // not sure if this is a good test
        // STUDY: atan2 returns values in -180..180 range

        eval(round_f64_s64(sin_f64((PI_f64/2))) == 1);
        eval(round_f64_s64(cos_f64((PI_f64*2))) == 1);
        eval(round_f64_s64(tan_f64(PI_f32)) == 0);
        eval(atan_f64(0, 0) == 0); // not sure if this is a good test

        eval(rad_to_dir(0) == ((v2){1, 0}));
        eval(dir_to_rad((v2){1, 0}) == 0);

        // abs/round/clamp/truncate/floor/ceil/sqrt
        eval(sqrt_f32(16.0f) == 4.0f);
        eval(sqrt_f64(16.0) == 4.0);
        eval(abs_f32(-1.0123f) == 1.0123f);
        eval(abs_f64(-1.0123) == 1.0123);
        eval(abs_s32(-1) == 1);
        eval(abs_s64(-1) == 1);
        eval(round_f32(1.1f) == 1.0f);
        eval(round_f32(1.9f) == 2.0f);
        eval(round_f32_s32(1.1f) == 1);
        eval(round_f32_s32(1.9f) == 2);
        eval(round_f32_u32(1.1f) == 1);
        eval(round_f32_u32(1.9f) == 2);
        eval(truncate_f32(1.9120939f) == 1.0f);
        eval(truncate_f32_s32(1.910293f) == 1);
        eval(floor_f32(1.910293f) == 1.0f);
        eval(floor_f32_s32(1.110293f) == 1);
        eval(ceil_f32(1.910293f) == 2.0f);
        eval(ceil_f32_s32(1.110293f) == 1);
        eval(clamp_f32(1.0f, 3.0f, 2.0f) == 2.0f);
        eval(clamp_f32(1.0f, 0.5f, 2.0f) == 1.0f);
        eval(clamp_f32(1.0f, 2.0f, 3.0f) == 2.0f);

        // lerp stuff
        eval(lerp(0.0f, 0.5f, 1.0f) == 0.5f);
        eval(unlerp(0.0f, 0.5f, 1.0f) == 0.5f);

        // TODO: I use this, but dont understand it.
        // Need to understand it before I can test it.
        //static f32 lerp_rad(f32 a, f32 t, f32 b){
    }

    // base_memory.h
    {
        typedef struct Test{ s32 a; s32 b; f32 d; } Test; // size 12

        // arena allocation
        eval(alloc_arena(10)->size == 10);

        // arena init
        void* base = malloc(MB(1));
        Arena* arena = (Arena*)base;
        arena_init(arena, ((u8*)base + sizeof(Arena)), (MB(1) - sizeof(Arena)));
        eval(arena->base == ((u8*)base + sizeof(Arena)));
        eval(arena->size == (MB(1) - sizeof(Arena)));
        eval(arena->used == 0);

        // push size/type/array
        push_struct(arena, Test);
        push_struct(arena, Test);
        eval(arena->used == 24);
        push_array(arena, Test, 2);
        eval(arena->used == 48);
        arena_free(arena);
        eval(arena->used == 0);

        // push_arena
        Arena* new_arena = push_arena(arena, 100);
        eval(arena->used == 124); // size of arena 24
        eval(new_arena->size == 100);
        arena_free(arena);

        // pop_array
        {
            Arena* arena = os_alloc_arena(100);
            push_array(arena, u32, 20);
            pop_array(arena, u32, 10);
            eval(arena->used == 40);
            pop_array(arena, u32, 10);
            eval(arena->used == 0);
        }

        // scratch
        push_array(arena, Test, 6);
        ScratchArena scratch = get_scratch(arena);
        eval(scratch.arena->used == 72);
        push_struct(scratch.arena, Test);
        push_struct(scratch.arena, Test);
        eval(scratch.arena->used == 96);
        push_array(scratch.arena, Test, 2);
        eval(scratch.arena->used == 120);
        eval(scratch.used == 72);
        end_scratch(scratch);
        eval(scratch.arena->used == 72);
        arena_free(arena);

        // begin_scratch
        // CONSIDER: NOTE: not sure if I want to use conflict arena or new version without conflict arena.
        // Need to figure out use case to understand what to test, but maybe for now use
        // no conflict version.
        ScratchArena scratch1 = begin_scratch(0);
        ScratchArena scratch2 = begin_scratch(1);
        push_array(scratch1.arena, Test, 6);
        push_array(scratch2.arena, Test, 1);
        eval(scratch1.arena->used == 72);
        eval(scratch2.arena->used == 12);
        end_scratch(scratch1);
        end_scratch(scratch2);
        eval(scratch1.arena->used == 0);
        eval(scratch2.arena->used == 0);
    }

    // base_linkedlist.h
    {
        typedef struct Data{ u32 id; } Data;

        // default node creation
        Arena* arena = alloc_arena(MB(1));
        Node* sentinel = push_node(arena);
        eval(sentinel->next == sentinel);
        eval(sentinel->prev == sentinel);

        Node* n0 = push_node(arena);
        Node* n1 = push_node(arena);
        Node* n2 = push_node(arena);

        n0->data = (Data*)push_struct(arena, Data);
        n1->data = (Data*)push_struct(arena, Data);
        n2->data = (Data*)push_struct(arena, Data);

        dll_push_front(sentinel, n0);
        dll_push_back(sentinel, n1);
        dll_push_back(sentinel, n2);

        // test push_front/back
        eval(sentinel->next == n0);
        eval(sentinel->prev == n2);
        eval(sentinel->prev->prev == n1);

        // test pop_front/back/remove
        dll_pop_front(sentinel);
        eval(sentinel->next == n1);
        dll_pop_back(sentinel);
        eval(sentinel->prev == n1);
        dll_remove(sentinel, n1);
        eval(sentinel->next == sentinel);
        eval(sentinel->prev == sentinel);

        // test reset
        dll_push_front(sentinel, n0);
        dll_push_back(sentinel, n1);
        dll_push_back(sentinel, n2);
        reset_sentinel(sentinel);
        eval(sentinel->next == sentinel);
        eval(sentinel->prev == sentinel);
    }

    // base_string.h
    {
        // str8/16/32 creation
        eval(str8_literal("haha").size == 4);
        eval(*str8_literal("haha").str == 'h');
        eval(str8("haha", 4).size == 4);
        eval(*str8("haha", 4).str == 'h');

        eval(str16(L"haha", 4).size == 4);
        eval(*str16(L"haha", 4).str == 'h');
        eval(str32(U"haha", 4).size == 4);
        eval(*str32(U"haha", 4).str == 'h');

        // == != operators
        String8 str8_1 = str8_literal("Hello World!");
        String8 str8_2 = str8_literal("Hello World!");
        String8 str8_3 = str8_literal("Hello orld!");
        String16 str16_1 = str16("Hello World!", 12);
        String16 str16_2 = str16("Hello World!", 12);
        String16 str16_3 = str16("Hello Word!", 12);
        eval(str8_1 == str8_2);
        eval(str8_1 != str8_3);
        eval((str16_1 == str16_2));
        eval((str16_1 != str16_3));

        // str8_concatenate
        ScratchArena scratch = begin_scratch(0);
        String8 left = str8_literal("Hello ");
        String8 middle = str8_literal("World");
        String8 right = str8_literal("!");
        String8 left_middle = str8_concatenate(scratch.arena, left, middle);
        eval((left.size + middle.size) == 11);
        eval(*(left_middle.str + 10) == 'd');
        String8 full = str8_concatenate(scratch.arena, left_middle, right);
        eval((left.size + middle.size + right.size) == 12);
        eval(*(full.str + 11) == '!');
        end_scratch(scratch);

        // str8_starts_with
        String8 source = str8_literal("Hello World!");
        String8 sub1 = str8_literal("Hello");
        String8 sub2 = str8_literal("Hello World!");
        String8 sub3 = str8_literal("Hello World! More");
        String8 sub4 = str8_literal("Helo");
        eval(str8_starts_with(source, sub1) == true);
        eval(str8_starts_with(source, sub2) == true);
        eval(str8_starts_with(source, sub3) == false);
        eval(str8_starts_with(source, sub4) == false);
    }

    // win32_memory.h
    {
        // os arena allocation
        eval(os_alloc_arena(10)->size == 10);
    }

    // win32_file.h
    {
        void* memory = malloc(KB(1));
        Arena* arena = (Arena*)memory;
        arena->base = (u8*)memory + sizeof(Arena);
        arena->size = MB(1) - sizeof(Arena);
        arena->used = 0;

        // utf8 to utf16 to utf8
        String8 string8_1 = str8_literal("test_string");
        String16 string16_1 = os_utf8_utf16(arena, string8_1);
        String16 string16_2 = str16(L"test_string", string16_1.size);
        String8 string8_2 = os_utf16_utf8(arena, string16_1);
        eval(string8_1 == string8_2);
        eval(string16_1 == string16_2);

        // cwd
        String8 cwd = os_get_cwd(arena);
        String8 cwd_literal = str8_literal("C:\\sh1tz\\apesticks\\cc++\\base");
        eval(cwd == cwd_literal);

        // os_file_read/os_file_write
        String8 dir_build = str8_concatenate(arena, cwd, str8_literal("\\build\\"));
        String8 test_file = str8_literal("test_file.bin");
        {
            String8 write_data_string8 = str8_literal("Some random data that I want in the file\n");
            FileData write_data = {write_data_string8.str, write_data_string8.size};
            os_file_write(write_data, dir_build, test_file, 0);

            FileData read_data = os_file_read(arena, dir_build, test_file);
            String8 read_data_string8 = str8(read_data.base, read_data.size);
            eval(read_data_string8 == write_data_string8);
        }

        {
            v4 write_data_v4 = {1.0f, 0.5f, 0.2f, 1.0f};
            FileData write_data = {&write_data_v4, sizeof(write_data_v4)};
            os_file_write(write_data, dir_build, test_file, 0);

            FileData read_data = os_file_read(arena, dir_build, test_file);
            v4* read_data_v4 = (v4*)read_data.base;
            eval(write_data_v4 == *read_data_v4);
        }

        // file_move + file_delete
        String8 new_test_file = str8_literal("new_test_file.bin");
        eval(os_file_move(dir_build, test_file, dir_build, new_test_file) == true);
        eval(os_file_delete(dir_build, new_test_file) == true);

        // dir_create + dir_delete
        String8 new_dir = str8_literal("\\new_dir");
        String8 new_path = str8_concatenate(arena, dir_build, new_dir);
        eval(os_dir_create(dir_build, new_dir) == true);
        eval(os_dir_delete(dir_build, new_dir) == true);
    }

    print("\n");
    print("------------------\n");
    print("FAILED TESTS (%d)|\n", fail_count);
    print("------------------\n");
	u8* string = (u8*)fail_arena->base;
    for(u32 i=0; i < fail_arena->used; ++i){
        print("%c", *string++);
    }
    return(0);
}
