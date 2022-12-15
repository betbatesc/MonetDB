#ifndef MONET_PARQUET
#define MONET_PARQUET

#include <parquet-glib/arrow-file-reader.h>

typedef struct parquet_file {
    char* filename;
    GParquetArrowFileReader *reader;
} parquet_file;

typedef struct parquet_table_metadata {
    char* table_name;
    int n_row;
} parquet_table_metadata;

/* Opens the file, and returns a struct containing the reader. */
parquet_file open_file(char* filename);

parquet_table_metadata get_table_metadata(parquet_file *file);

#endif
