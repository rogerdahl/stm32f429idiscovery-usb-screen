#include "stdafx.h"

#include "jpeg.h"
#include "stm32f4_discovery_usb_screen.h"

// astyle --pad-oper --indent=spaces=2 --pad-header analyze_mjpeg.cpp

using namespace std;

#include <boost/algorithm/string.hpp>
using namespace std;
using namespace boost;
using namespace boost::algorithm;
using boost::asio::ip::tcp;
using boost::lexical_cast;


int run(int argc, char* argv[]);
void reduce_and_convert(u16* dst, int dst_w, int dst_h, const u8* src, int src_w, int src_h);

int main(int argc, char* argv[])
{
  if (argc != 4) {
    cout << "Usage: " << argv[0] << " <server> <port> <path>\n" << endl;
    return 1;
  }

  try {
    return run(argc, argv);
  }
  catch (std::exception& e) {
    cout << "Exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}


int run(int argc, char* argv[])
{
  init_usb();
  //uint16_t b[320*200*2];
  //send_screen(b);

  boost::asio::io_service io_service;

  // Get a list of endpoints corresponding to the server name.
  tcp::resolver resolver(io_service);
  tcp::resolver::query query(argv[1], argv[2]);
  tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
  tcp::resolver::iterator end;

  // Try each endpoint until we successfully establish a connection.
  tcp::socket socket(io_service);
  boost::system::error_code error = boost::asio::error::host_not_found;
  while (error && endpoint_iterator != end) {
    //cout << "trying..." << endl;
    socket.close();
    socket.connect(*endpoint_iterator++, error);
  }
  if (error) {
    throw boost::system::system_error(error);
  }

  //cout << "connected" << endl;

  // Form the request.
  boost::asio::streambuf request;
  ostream request_stream(&request);
  request_stream << "GET " << argv[3] << " HTTP/1.1\r\n";
  // Some buggy cams don't work without a user-agent. The header SHOULD be
  // included according to the HTTP spec.
  request_stream << "User-Agent: MJPEGviewer\r\n";
  request_stream << "Host: " << argv[1] << ":" << argv[2] << "\r\n";
  request_stream << "Accept: */*\r\n";
  request_stream << "\r\n";

  // Send the request.
  boost::asio::write(socket, request);

  //cout << "sent" << endl;

  // Read the response status line. The response streambuf will automatically
  // grow to accommodate the entire line. The growth may be limited by passing
  // a maximum size to the streambuf constructor.
  boost::asio::streambuf response;
  boost::asio::read_until(socket, response, "\r\n");

  //cout << "got response" << endl;

  // Check that response is OK.
  istream response_stream(&response);
  string http_version;
  response_stream >> http_version;
  unsigned int status_code;
  response_stream >> status_code;
  string status_message;
  getline(response_stream, status_message);
  if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
    cout << "Invalid response\n";
    return 1;
  }
  if (status_code != 200) {
    cout << "Response returned with status code " << status_code << "\n";
    return 1;
  }

  //cout << "response is ok" << endl;

  // Read the response headers, which are terminated by a blank line.
  boost::asio::read_until(socket, response, "\r\n\r\n");

  //cout << "header read ok" << endl;

  // Get the MIME multipart boundary from the headers.
  regex rx_content_type("Content-Type:.*boundary=(.*)", regex::icase);
  regex rx_content_length("Content-length: (.*)", regex::icase);

  smatch match;
  string header;
  string boundary;

  while (getline(response_stream, header) && header != "\r") {
    //cout << "HTTP HEADER: " << header << endl;
    if (regex_search(header, match, rx_content_type)) {
      boundary = match[1];
      //cout << "BOUNDARY SELECTED: " << boundary << endl;
    }
  }

  //cout << "got mime ok" << endl;
  //cout << "boundary: " << boundary << endl;

  // Abort if a boundary was not found.
  if (boundary == "") {
    cout << "Not a valid MJPEG stream" << endl;
    return false;
  }

  u32 buf_size(0);
  char* buf(0);

  u32 cycles = 0;

  while (1) {
    //cout << "-- START CYCLE --" << endl;

    //cout << "RESPONSE SIZE, BEFORE READ TO BOUNDARY: " << response.size() << endl;

    // Read until there is a boundary in the response. If there is already a
    // boundary in the buffer, this is a no-op.
    boost::asio::read_until(socket, response, boundary);

    //cout << "RESPONSE SIZE, AFTER READ TO BOUNDARY: " << response.size() << endl;

    // Remove everything up to and including the boundary that is now known to
    // be there.
    while (getline(response_stream, header)) {
      //cout << "BOUNDARY SEARCH: " << header << endl;
      if (header == boundary) {
        //cout << "BOUNDARY FOUND: " << header << endl;
        break;
      }
    }

    // Read the headers that follow the boundary. These always end with a blank
    // line. Content-Length must be one of the header lines, and the size of the
    // compressed jpeg is read from it.

    //cout << "RESPONSE SIZE, AFTER BOUNDARY SEARCH: " << response.size() << endl;

    u32 content_length;
    while (getline(response_stream, header) && header != "\r") {
      trim(header);
      //cout << "MM HEADER: " << header << endl;
      if (regex_search(header, match, rx_content_length)) {
        content_length = lexical_cast<int>(match[1]);
        //cout << "MM HEADER CONTENT-LENGTH FOUND: " << content_length << endl;
      }
    }

    // Read until the entire jpeg is in the response.
    if (response.size() < content_length) {
      boost::asio::read(socket, response, boost::asio::transfer_at_least(
                          content_length - response.size()));
    }

    //cout << "RESPONSE SIZE, BEFORE SGETN: " << response.size() << endl;

    if (buf_size < content_length) {
      buf_size = content_length;
      if (buf) {
        free(buf);
      }
      buf = (char*)malloc(buf_size);
    }

    response.sgetn(buf, content_length);

    //cout << "RESPONSE SIZE, BEFORE JPEG CONSUME: " << response.size() << endl;

    //response.consume(content_length);

    //cout << "RESPONSE SIZE, AFTER JPEG CONSUME: " << response.size() << endl;

    // Dump JPG to file for debugging.
    //char buf2[10000] = {0};
    //response.sgetn(buf2, 1000);
    //ofstream o("out.bin", ios::binary);
    //o.write(buf2, 1000);

    Image image = decompress_jpeg(buf, content_length);
    if (!image.error) {
      const u8* image_ptr(&image.image_data[0]);
      uint16_t screen_buf[320 * 240];
      reduce_and_convert(screen_buf, 320, 240, image_ptr, image.w, image.h);
      send_screen(screen_buf);      
    }

    ++cycles;
    //cout << "cycles: " << cycles << endl;
  }

  free(buf);

  deinit_usb();

  return 0;
}

// - Convert from 24-bit RGB to 16-bit RGB.
// - Reduce size from native web cam to 320x240 using pixel averages. If source
//   size does not divide evenly by destination size on an axis, part of the
//   source is unused in that axis.
// - Rotate image 90 deg to compensate for portrait memory layout on Discovery
//   board.
void reduce_and_convert(u16* dst, int dst_w, int dst_h, const u8* src, int src_w, int src_h)
{
  int w_ratio = src_w / dst_w;
  int h_ratio = src_h / dst_h;
  int ratio = w_ratio * h_ratio;
  for (int dst_y = 0; dst_y < dst_h; ++dst_y) {
    for (int dst_x = 0; dst_x < dst_w; ++dst_x) {
      int r = 0;
      int g = 0;
      int b = 0;
      for (int src_y = 0; src_y < h_ratio; ++src_y) {
        int pos_y = (dst_y * h_ratio + src_y) * src_w * 3;
        for (int src_x = 0; src_x < w_ratio; ++src_x) {
          int pos_x = (dst_x * w_ratio + src_x) * 3;
          int offset = pos_x + pos_y;
          r += src[offset];
          g += src[++offset];
          b += src[++offset];
        }
      }
      r /= ratio;
      g /= ratio;
      b /= ratio;
      dst[dst_y + dst_h * (dst_w - 1 - dst_x)] = (r >> 3) << 11 | (g >> 2) << 5 | (b >> 3);
    }
  }
}
