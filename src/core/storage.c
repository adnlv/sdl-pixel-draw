#include "storage.h"

void init_storage(storage_t* dest,
                  const open_file_stream_func_t open_file_stream,
                  const close_file_stream_func_t close_file_stream,
                  const save_pixels_func_t save_pixels,
                  const read_pixels_func_t read_pixels)
{
    dest->stream = NULL;
    dest->open_file_stream = open_file_stream;
    dest->close_file_stream = close_file_stream;
    dest->save_pixels = save_pixels;
    dest->read_pixels = read_pixels;
}

void destroy_storage(storage_t* storage)
{
    if (storage->stream != NULL)
    {
        storage->close_file_stream(storage->stream);
        storage->stream = NULL;
    }

    storage->open_file_stream = NULL;
    storage->close_file_stream = NULL;
    storage->save_pixels = NULL;
    storage->read_pixels = NULL;
}
