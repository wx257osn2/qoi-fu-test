#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_LINEAR
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

extern "C" {

#include "QOI-stdio.h"

}

#include <bit>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string_view>

static void convert_format(void* buf, int w, int h){
  if(buf == nullptr)
    return;
  const auto area = static_cast<std::size_t>(w) * static_cast<std::size_t>(h);
  std::uint8_t*const ptr = reinterpret_cast<std::uint8_t*>(buf);
  switch(std::endian::native){
  case std::endian::little:
    // convert rgba to bgra
    for(std::size_t i = 0; i < area; ++i)
      std::swap(ptr[i*4], ptr[i*4+2]);
    break;
  case std::endian::big:
    // convert rgba to argb
    for(std::size_t i = 0; i < area; ++i){
      auto r = ptr[i*4];
      auto g = ptr[i*4+1];
      auto b = ptr[i*4+2];
      auto a = ptr[i*4+3];
      ptr[i*4] = a;
      ptr[i*4+1] = r;
      ptr[i*4+2] = g;
      ptr[i*4+3] = b;
    }
    break;
  default:
    throw std::runtime_error("unsupported endian");
  }
}

void revert_format(void* buf, int w, int h, bool has_alpha){
  if(buf == nullptr)
    return;
  const auto area = static_cast<std::size_t>(w) * static_cast<std::size_t>(h);
  std::uint8_t*const ptr = reinterpret_cast<std::uint8_t*>(buf);
  switch(std::endian::native){
  case std::endian::little:
    // convert bgra to rgba
    for(std::size_t i = 0; i < area; ++i)
      std::swap(ptr[i*4], ptr[i*4+2]);
    if(!has_alpha)
      for(std::size_t i = 0; i < area; ++i)
        ptr[i*3] = ptr[i*4],
        ptr[i*3+1] = ptr[i*4+1],
        ptr[i*3+2] = ptr[i*4+2];
    break;
  case std::endian::big:
    // convert argb to rgba
    for(std::size_t i = 0; i < area; ++i){
      auto a = ptr[i*4];
      auto r = ptr[i*4+1];
      auto g = ptr[i*4+2];
      auto b = ptr[i*4+3];
      if(has_alpha){
        ptr[i*4] = r;
        ptr[i*4+1] = g;
        ptr[i*4+2] = b;
        ptr[i*4+3] = a;
      }
      else{
        ptr[i*3] = r;
        ptr[i*3+1] = g;
        ptr[i*3+2] = b;
      }
    }
    break;
  default:
    throw std::runtime_error("unsupported endian");
  }
}

int main(int argc, char **argv) {
  using namespace std::literals::string_view_literals;

  if (argc < 3) {
    std::puts("Usage: qoiconv <infile> <outfile>");
    std::puts("Examples:");
    std::puts("  qoiconv input.png output.qoi");
    std::puts("  qoiconv input.qoi output.png");
    std::exit(1);
  }

  void *pixels = nullptr;
  int w, h, channels;
  const std::string_view argv_1{argv[1]};
  if (argv_1.ends_with(".png")) {
    if(!::stbi_info(argv[1], &w, &h, &channels)) {
      std::printf("Couldn't read header %s\n", argv[1]);
      std::exit(1);
    }

    // Force all odd encodings to be RGBA
    if(channels != 3) {
      channels = 4;
    }

    // Force all encodings to be RGBA
    pixels = stbi_load(argv[1], &w, &h, NULL, 4);
    convert_format(pixels, w, h);
  }
  else if (argv_1.ends_with(".qoi")) {
    auto dec = std::unique_ptr<::QOIDecoder, decltype(&::QOIDecoder_Delete)>{QOIDecoder_LoadFile(argv[1]), &::QOIDecoder_Delete};
    if(dec){
      channels = ::QOIDecoder_HasAlpha(dec.get()) ? 4 : 3;
      w = QOIDecoder_GetWidth(dec.get());
      h = QOIDecoder_GetHeight(dec.get());
      pixels = std::malloc(w * h * 4);
      if (pixels == nullptr)
        std::exit(1);
      std::memcpy(pixels, ::QOIDecoder_GetPixels(dec.get()), w * h * 4);
      revert_format(pixels, w, h, channels == 4);
    }
  }

  if (pixels == nullptr) {
    std::printf("Couldn't load/decode %s\n", argv[1]);
    std::exit(1);
  }

  const std::string_view argv_2{argv[2]};
  int encoded = 0;
  if (argv_2.ends_with(".png")) {
    encoded = stbi_write_png(argv[2], w, h, channels, pixels, 0);
  }
  else if (argv_2.ends_with(".qoi")) {
    auto enc = std::unique_ptr<::QOIEncoder, decltype(&::QOIEncoder_Delete)>{::QOIEncoder_New(), &::QOIEncoder_Delete};
    encoded = ::QOIEncoder_Encode(enc.get(), w, h, reinterpret_cast<const int*>(pixels), channels == 4, false);
    if(encoded)
      ::QOIEncoder_SaveFile(enc.get(), argv[2]);
  }

  if (!encoded) {
    std::printf("Couldn't write/encode %s\n", argv[2]);
    std::exit(1);
  }

  std::free(pixels);
  return 0;
}
