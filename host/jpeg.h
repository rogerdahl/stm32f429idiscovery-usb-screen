#include "stdafx.h"

#pragma once

typedef std::vector<u8> ImageData;

struct Image {
  u32 w;
  u32 h;
  u32 bytes_per_pixel;
  ImageData image_data;
  bool error;
};

Image decompress_jpeg(char* compressed_frame, int compressed_frame_size);

