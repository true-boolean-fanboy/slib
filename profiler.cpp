#include "profiler.h"

#if PROFILE_ENABLE

#include <cstdio>
#include <cstring>

#include <slib/math.h>
#include <slib/metric.cpp>

u32 constexpr MAX_PROFILING_POINTS = 100;

struct profiler_t
{
    u64 start_time = 0;
    u64 end_time = 0;
    u32 current_scope = 0;

    profile_measure_point_t points[MAX_PROFILING_POINTS];
};
static profiler_t global_profiler;

profile_measure_scope_t::profile_measure_scope_t(const char* name, u32 idx, u64 byte_count)
{
    this->parent = global_profiler.current_scope;
    global_profiler.current_scope = idx;

    profile_measure_point_t* point = global_profiler.points + idx;
    this->old_total_time = point->total_time;
    point->processed_bytes += byte_count;

    this->id = idx;
    this->name = name;
    this->start = read_cpu_timer();
}

profile_measure_scope_t::~profile_measure_scope_t()
{
    u64 execution_time = read_cpu_timer() - start;

    profile_measure_point_t* point = global_profiler.points + id;

    point->name = name;
    point->calls += 1; 
    point->exclusive_time += execution_time;
    point->total_time = old_total_time + execution_time;

    profile_measure_point_t* parent_point = global_profiler.points + parent;
    parent_point->exclusive_time -= execution_time;

    global_profiler.current_scope = parent;
}

void
profiling_begin()
{
    global_profiler.start_time = read_cpu_timer();
}

constexpr u32 MAX_COL_WIDTH = 20;
constexpr u32 MAX_COLUMNS = 10;
constexpr u32 MAX_ROWS = 15;

enum table_cell_align_t
{
    LEFT,
    RIGHT,
    CENTER
};

struct table_cell_t
{
    table_cell_align_t alignment;
    char text[MAX_COL_WIDTH+1];
};

struct table_column_t
{
    i32 max_width;
    char header[MAX_COL_WIDTH];
    table_cell_t cells[MAX_ROWS];
};

struct table_t
{
    u32 num_rows;
    u32 num_cols;
    table_column_t columns[MAX_COLUMNS];
};

void add_column(table_t* table, const char* name)
{
    table_column_t* column = table->columns + table->num_cols;
    strncpy_s(column->header, name, MAX_COL_WIDTH);

    column->max_width = max(column->max_width, (i32)strlen(name));

    table->num_cols += 1;
}

void add_rows(table_t* table, u32 rows)
{
    table->num_rows += rows;
}

void set_cell(table_t* table, u32 col, u32 row, f64 value, table_cell_align_t align)
{
    table_column_t* column = table->columns + col;
    table_cell_t* cell = column->cells + row;

    cell->alignment = align;

    char format_str[100];
    sprintf_s(format_str, "%.4f", value);
    strncpy_s(cell->text, format_str, MAX_COL_WIDTH);

    column->max_width = max(column->max_width, (i32)strlen(cell->text));
}

void set_cell(table_t* table, u32 col, u32 row, u64 value, table_cell_align_t align)
{
    table_column_t* column = table->columns + col;
    table_cell_t* cell = column->cells + row;

    cell->alignment = align;

    char format_str[100];
    sprintf_s(format_str, "%llu", value);
    strncpy_s(cell->text, format_str, MAX_COL_WIDTH);

    column->max_width = max(column->max_width, (i32)strlen(cell->text));
}

void set_cell(table_t* table, u32 col, u32 row, const char* value, table_cell_align_t align)
{
    table_column_t* column = table->columns + col;
    table_cell_t* cell = column->cells + row;

    cell->alignment = align;

    char format_str[100];
    sprintf_s(format_str, "%s", value);
    strncpy_s(cell->text, format_str, MAX_COL_WIDTH);

    column->max_width = max(column->max_width, (i32)strlen(cell->text));
}

void print_table(table_t* table)
{
    char format_str[1024];

    for(u32 i = 0; i < table->num_cols; ++i)
    {
        sprintf_s(format_str, "%%s| %%-%ds", table->columns[i].max_width);
        printf(format_str, 
               i == 0 ? "" : " ",
               table->columns[i].header);
    }
    printf(" |\n");
    
    u32 border = 0;
    memset(format_str, '-', ARRAY_ELEMENTS(format_str));
    for(u32 i = 0; i < table->num_cols; ++i)
    {
        format_str[border] = '|';
        border += 3; 
        border += table->columns[i].max_width;
    }
    format_str[border] = '|';
    format_str[border+1] = '\0';

    printf("%s\n", format_str);

    for(u32 row = 0; row < table->num_rows; ++row)
    {
        for(u32 i = 0; i < table->num_cols; ++i)
        {
            table_column_t* column = table->columns + i;
            table_cell_t* cell = column->cells + row;

            switch(cell->alignment)
            {
                case LEFT: 
                    sprintf_s(format_str, "%%s| %%-%ds", column->max_width);
                    break;
                case RIGHT: 
                    sprintf_s(format_str, "%%s| %%%ds", column->max_width);
                    break;
                case CENTER: 
                    {
                        u32 len = (u32)strlen(cell->text);
                        u32 padding = 
                            (column->max_width - len) / 2;
                        sprintf_s(format_str,
                                "%%s| %*s%%s%*s",
                                padding, "",
                                (column->max_width - len) - padding, "");
                    } break;
            }
            printf(format_str, i == 0 ? "" : " ", cell->text);
        }
        printf(" |\n");
    }
}

void
profiling_end()
{
    u64 total_end = read_cpu_timer();
    u64 total_time = total_end - global_profiler.start_time;

    u64 cpu_freq = get_cpu_freq();
    f64 time_ms = ((f64)total_time / (f64)cpu_freq) * 1000.;

    table_t total_table{};
    add_column(&total_table, "Time (ms)");
    add_column(&total_table, "Ticks");
    add_column(&total_table, "Freq");
    add_rows(&total_table, 1);
    set_cell(&total_table, 0, 0, time_ms, LEFT);
    set_cell(&total_table, 1, 0, total_time, RIGHT);
    set_cell(&total_table, 2, 0, cpu_freq, RIGHT);
    print_table(&total_table);

    printf("\n");

    table_t table{};
    add_column(&table, "Name");
    add_column(&table, "Calls");
    add_column(&table, "Time (ms)");
    add_column(&table, "Avg.");
    add_column(&table, "Exclusive");
    add_column(&table, "Exclusive %");
    add_column(&table, "Total");
    add_column(&table, "Total %");
    add_column(&table, "Data");
    add_column(&table, "GB/s");

    for(u32 i = 0; i < ARRAY_ELEMENTS(global_profiler.points); ++i)
    {
        profile_measure_point_t* point = global_profiler.points + i;
        if(point->total_time == 0)
        {
            continue;
        }

        u32 row = table.num_rows;
        add_rows(&table, 1);
        
        set_cell(&table, 0, row, point->name, LEFT);
        set_cell(&table, 1, row, point->calls, RIGHT);
        set_cell(&table, 2, row, (f64)point->total_time / (f64)cpu_freq * 1000., RIGHT);
        set_cell(&table, 3, row, point->total_time / point->calls, RIGHT);
        set_cell(&table, 4, row, point->exclusive_time, RIGHT);
        set_cell(&table, 5, row, ((f64)point->exclusive_time / (f64)total_time) * 100., RIGHT);
        set_cell(&table, 6, row, point->total_time, RIGHT);
        set_cell(&table, 7, row, ((f64)point->total_time / (f64)total_time) * 100., RIGHT);

        if(point->processed_bytes)
        {
            set_cell(&table, 8, row, point->processed_bytes, RIGHT);

            f64 megabyte =  1024.f * 1024.f;
            f64 gigabyte = megabyte * 1024.f;
            f64 seconds = (f64)point->total_time / (f64)cpu_freq;
            f64 bytes_per_second = (f64)point->processed_bytes / seconds;
            f64 gigabytes_per_second = bytes_per_second / gigabyte;
            set_cell(&table, 8, row, gigabytes_per_second, RIGHT);
        }
        else
        {
            set_cell(&table, 7, row, "N/A", RIGHT);
            set_cell(&table, 8, row, "N/A", RIGHT);
        }

        if(table.num_rows >= MAX_ROWS)
        {
            break;
        }
    }

    print_table(&table);
}

#endif /* PROFILE_ENABLE */
