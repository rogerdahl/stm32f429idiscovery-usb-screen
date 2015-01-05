#include "stdafx.h"

#include <csetjmp>

#include "jpeg.h"

extern "C" {
#include <jpeglib.h>
#include <jerror.h>
}

using namespace std;


struct jpegErrorManager {
  /* "public" fields */
  struct jpeg_error_mgr pub;
  /* for return to caller */
  jmp_buf setjmp_buffer;
};

char jpegLastErrorMsg[JMSG_LENGTH_MAX];

void jpegErrorExit (j_common_ptr cinfo)
{
  /* cinfo->err actually points to a jpegErrorManager struct */
  jpegErrorManager* myerr = (jpegErrorManager*) cinfo->err;
  /* note : *(cinfo->err) is now equivalent to myerr->pub */

  /* output_message is a method to print an error message */
  /*(* (cinfo->err->output_message) ) (cinfo);*/

  /* Create the message */
  ( *(cinfo->err->format_message) ) (cinfo, jpegLastErrorMsg);

  /* Jump to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);

}

Image decompress_jpeg(char* compressed_frame, int compressed_frame_size)
{
  struct jpeg_decompress_struct cinfo;
  jpegErrorManager jerr;
  JSAMPROW row_pointer[1];

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = jpegErrorExit;
    
  // Error handler.
  if (setjmp(jerr.setjmp_buffer)) {
    cout << "Error: Couldn't decompress JPEG: " << jpegLastErrorMsg << endl;
    jpeg_destroy_decompress(&cinfo);


    exit(0); //////////////////////

    
    Image m;
    m.error = true;
    return m;
  }
  
  jpeg_create_decompress(&cinfo);

  jpeg_mem_src(&cinfo, (unsigned char*)compressed_frame, compressed_frame_size);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  row_pointer[0] = (unsigned char *)malloc(cinfo.output_width * cinfo.num_components);

  ImageData image_data;
  image_data.resize(cinfo.output_width * cinfo.output_height * cinfo.num_components);

  u8* image_data_ptr = &image_data[0];
  while (cinfo.output_scanline < cinfo.image_height) {
    jpeg_read_scanlines(&cinfo, row_pointer, 1);
    for (u32 i(0); i < cinfo.image_width * cinfo.num_components; i++) {
      *image_data_ptr++ = row_pointer[0][i];
    }
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  Image m;
  m.w = cinfo.output_width;
  m.h = cinfo.output_height;
  m.image_data = image_data;
  m.bytes_per_pixel = cinfo.num_components;
  m.error = false;

  return m;
}
