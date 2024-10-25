#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "WorkDiv.h"
#include "config.h"
#include "kernel.h"
#include <alpaka/alpaka.hpp>

#ifdef __linux__
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define FMT_HEADER_ONLY
#include "fmt/color.h"
#include "fmt/core.h"

using namespace std::literals;

struct Image {
  unsigned char *data_ = nullptr;
  int width_ = 0;
  int height_ = 0;
  int channels_ = 0;

  Image() {}

  Image(std::string const &filename) { open(filename); }

  Image(int width, int height, int channels, unsigned char *data)
      : width_(width), height_(height), channels_(channels) {
    size_t size = width_ * height_ * channels_;
    data_ = static_cast<unsigned char *>(stbi__malloc(size));
    std::memcpy(data_, data, size);
  }

  ~Image() { close(); }

  void open(std::string const &filename) {
    data_ = stbi_load(filename.c_str(), &width_, &height_, &channels_, 0);
    if (data_ == nullptr) {
      throw std::runtime_error("Failed to load "s + filename);
    }
    std::cout << "Loaded image with " << width_ << " x " << height_
              << " pixels and " << channels_ << " channels from " << filename
              << '\n';
  }

  void write(std::string const &filename) {
    if (filename.ends_with(".png")) {
      int status = stbi_write_png(filename.c_str(), width_, height_, channels_,
                                  data_, 0);
      if (status == 0) {
        throw std::runtime_error("Error while writing PNG file "s + filename);
      }
    } else if (filename.ends_with(".jpg") or filename.ends_with(".jpeg")) {
      int status = stbi_write_jpg(filename.c_str(), width_, height_, channels_,
                                  data_, 95);
      if (status == 0) {
        throw std::runtime_error("Error while writing JPEG file "s + filename);
      }
    } else {
      throw std::runtime_error("File format "s + filename + "not supported"s);
    }
  }

  void close() {
    if (data_ != nullptr) {
      stbi_image_free(data_);
    }
    data_ = nullptr;
  }

  // show an image on the terminal, using up to max_width columns (with one
  // block per column) and up to max_height lines (with two blocks per line)
  void show(int max_width, int max_height) {
    if (data_ == nullptr) {
      return;
    }

    // two blocks per line
    max_height = max_height * 2;

    // find the best size given the max width and height and the image aspect
    // ratio
    int width, height;
    if (width_ * max_height > height_ * max_width) {
      width = max_width;
      height = max_width * height_ / width_;
    } else {
      width = max_height * width_ / height_;
      height = max_height;
    }

    // two blocks per line
    for (int j = 0; j < height; j += 2) {
      int y1 = j * height_ / height;
      int y2 = (j + 1) * height_ / height;
      // one block per column
      for (int i = 0; i < width; ++i) {
        int x = i * width_ / width;
        int p = (y1 * width_ + x) * channels_;
        int r = data_[p];
        int g = data_[p + 1];
        int b = data_[p + 2];
        auto style = fmt::fg(fmt::rgb(r, g, b));
        if (y2 < height_) {
          p = (y2 * width_ + x) * channels_;
          r = data_[p];
          g = data_[p + 1];
          b = data_[p + 2];
          style |= fmt::bg(fmt::rgb(r, g, b));
        }
        std::cout << fmt::format(style, "▀");
      }
      std::cout << '\n';
    }
  }
};

bool verbose = false;

int main(int argc, const char *argv[]) {
  const char *verbose_env = std::getenv("VERBOSE");
  if (verbose_env != nullptr and std::strlen(verbose_env) != 0) {
    verbose = true;
  }

  std::vector<std::string> files;
  if (argc == 1) {
    // no arguments, use a single default image
    files = {"image.png"s};
  } else {
    files.reserve(argc - 1);
    for (int i = 1; i < argc; ++i) {
      files.emplace_back(argv[i]);
    }
  }

  int rows = 80;
  int columns = 80;
#if defined(__linux__) && defined(TIOCGWINSZ)
  if (isatty(STDOUT_FILENO)) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if (w.ws_row > 1 and w.ws_col > 1) {
      rows = w.ws_row - 1;
      columns = w.ws_col - 1;
    }
  }
#endif

  // alpaka init
  HostPlatform platformHost{};
  Platform platform{};
  Device device = alpaka::getDevByIdx(platform, 0u);
  auto queue = Queue{device};

  auto start = std::chrono::steady_clock::now();
  // std::vector<Image> images;
  // images.resize(files.size());
  for (unsigned int i = 0; i < files.size(); ++i) {
    Image img;
    img.open(files[i]);
    img.show(columns, rows);

    const auto w = img.width_;
    const auto h = img.height_;
    const auto c = img.channels_;
    const auto size = w * h * c;
    const float scale = 0.5f;
    const auto sizeSmall = w * scale * h * scale * c;

    auto in_h = alpaka::allocMappedBuf<unsigned char, uint32_t>(
        alpaka::getDevByIdx(platformHost, 0u), platform, Vec1D{size});
    auto out_h = alpaka::allocMappedBuf<unsigned char, uint32_t>(
        alpaka::getDevByIdx(platformHost, 0u), platform, Vec1D{size});

    std::memcpy(in_h.data(), img.data_, size);

    // device allocations
    auto in_d =
        alpaka::allocAsyncBuf<unsigned char, uint32_t>(queue, Vec1D{size});
    auto out_d =
        alpaka::allocAsyncBuf<unsigned char, uint32_t>(queue, Vec1D{size});

    auto out1_d =
        alpaka::allocAsyncBuf<unsigned char, uint32_t>(queue, Vec1D{sizeSmall});
    auto out2_d =
        alpaka::allocAsyncBuf<unsigned char, uint32_t>(queue, Vec1D{sizeSmall});
    auto out3_d =
        alpaka::allocAsyncBuf<unsigned char, uint32_t>(queue, Vec1D{sizeSmall});
    auto out4_d =
        alpaka::allocAsyncBuf<unsigned char, uint32_t>(queue, Vec1D{sizeSmall});

    // copy to device
    alpaka::memcpy(queue, in_d, in_h);

    // process on device
    auto const &workDiv = makeWorkDiv<Acc2D>(
        Vec2D{(w + 32 - 1) / 32., (h + 32 - 1) / 32.}, Vec2D{32, 32});
    alpaka::exec<Acc2D>(queue, workDiv, ScaleKernel{}, in_d.data(),
                        out1_d.data(), w, h, c, scale);
    alpaka::exec<Acc2D>(queue, workDiv, GrayScaleKernel{}, out1_d.data(),
                        w * scale, h * scale, c);
    alpaka::exec<Acc2D>(queue, workDiv, TintKernel{}, out1_d.data(),
                        out2_d.data(), 168, 56, 172, w * scale, h * scale, c);
    alpaka::exec<Acc2D>(queue, workDiv, TintKernel{}, out1_d.data(),
                        out3_d.data(), 100, 143, 47, w * scale, h * scale, c);
    alpaka::exec<Acc2D>(queue, workDiv, TintKernel{}, out1_d.data(),
                        out4_d.data(), 255, 162, 36, w * scale, h * scale, c);

    WriteTo(queue, out1_d, out_d, 0, 0, w, h, c, scale);
    WriteTo(queue, out2_d, out_d, w * 0.5, 0, w, h, c, scale);
    WriteTo(queue, out3_d, out_d, 0, h * 0.5, w, h, c, scale);
    WriteTo(queue, out4_d, out_d, w * 0.5, h * 0.5, w, h, c, scale);

    // copy to host
    alpaka::memcpy(queue, out_h, out_d);
    alpaka::wait(queue);
    unsigned char *out_data = alpaka::getPtrNative(out_h);

    Image out(w, h, c, out_data);

    std::cout << '\n';
    out.show(columns, rows);
    out.write(fmt::format("out{:02d}.jpg", i));
  }
  auto finish = std::chrono::steady_clock::now();
  float ms =
      std::chrono::duration_cast<std::chrono::duration<float>>(finish - start)
          .count() *
      1000.f;
  if (true) {
    std::cerr << fmt::format("total:      {:6.2f}", ms) << " ms\n";
  }

  return 0;
}
