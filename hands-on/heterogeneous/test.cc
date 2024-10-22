#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <alpaka/alpaka.hpp>
#include "config.h"
#include "WorkDiv.h"
#include "kernel.h"

#ifdef __linux__
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define FMT_HEADER_ONLY
#include "fmt/core.h"
#include "fmt/color.h"

using namespace std::literals;

struct Image {
  unsigned char* data_ = nullptr;
  int width_ = 0;
  int height_ = 0;
  int channels_ = 0;

  Image() {}

  Image(std::string const& filename) { open(filename); }

  ~Image() { close(); }

  void open(std::string const& filename) {
    data_ = stbi_load(filename.c_str(), &width_, &height_, &channels_, 0);
    if (data_ == nullptr) {
      throw std::runtime_error("Failed to load "s + filename);
    }
    std::cout << "Loaded image with " << width_ << " x " << height_ << " pixels and " << channels_ << " channels from "
              << filename << '\n';
  }

  void write(std::string const& filename) {
    if (filename.ends_with(".png")) {
      int status = stbi_write_png(filename.c_str(), width_, height_, channels_, data_, 0);
      if (status == 0) {
        throw std::runtime_error("Error while writing PNG file "s + filename);
      }
    } else if (filename.ends_with(".jpg") or filename.ends_with(".jpeg")) {
      int status = stbi_write_jpg(filename.c_str(), width_, height_, channels_, data_, 95);
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

  // show an image on the terminal, using up to max_width columns (with one block per column) and up to max_height lines (with two blocks per line)
  void show(int max_width, int max_height) {
    if (data_ == nullptr) {
      return;
    }

    // two blocks per line
    max_height = max_height * 2;

    // find the best size given the max width and height and the image aspect ratio
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

int main(int argc, const char* argv[]) {
  const char* verbose_env = std::getenv("VERBOSE");
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
  std::vector<Image> images;
  images.resize(files.size());
  for (unsigned int i = 0; i < files.size(); ++i) {
    // host allocations
    auto in_h = alpaka::allocMappedBuf<Image, uint32_t>(alpaka::getDevByIdx(platformHost, 0u), platform, Scalar{});
    auto out_h = alpaka::allocMappedBuf<Image, uint32_t>(alpaka::getDevByIdx(platformHost, 0u), platform, Scalar{});
    auto& img = *(in_h.data());
    img = images[i];
    img.open(files[i]);
    img.show(columns, rows);

    // device allocations
    auto in_d = alpaka::allocAsyncBuf<Image, uint32_t>(queue, Scalar{});
    auto out1_d = alpaka::allocAsyncBuf<Image, uint32_t>(queue, Scalar{});
    auto out2_d = alpaka::allocAsyncBuf<Image, uint32_t>(queue, Scalar{});
    auto out3_d = alpaka::allocAsyncBuf<Image, uint32_t>(queue, Scalar{});
    auto out4_d = alpaka::allocAsyncBuf<Image, uint32_t>(queue, Scalar{});
    auto out_d = alpaka::allocAsyncBuf<Image, uint32_t>(queue, Scalar{});

    // copy to device
    alpaka::memcpy(queue, in_d, in_h);
    const auto w = img.width_;
    const auto h = img.height_;
    const auto c = img.channels_;
	const float scale = 0.5f;
    const auto sizeSmall = w * scale * h * scale * c;
    auto out1_data =  alpaka::allocAsyncBuf<unsigned char, uint32_t>(queue, Vec1D{sizeSmall});
    out1_d->data_ = out1_data.data();
    auto out2_data =  alpaka::allocAsyncBuf<unsigned char, uint32_t>(queue, Vec1D{sizeSmall});
    out2_d->data_ = out2_data.data();
    auto out3_data =  alpaka::allocAsyncBuf<unsigned char, uint32_t>(queue, Vec1D{sizeSmall});
    out3_d->data_ = out3_data.data();
    auto out4_data =  alpaka::allocAsyncBuf<unsigned char, uint32_t>(queue, Vec1D{sizeSmall});
    out4_d->data_ = out4_data.data();

    // process on device
    auto const& workDiv = makeWorkDiv<Acc2D>(Vec2D{(w+32-1)/32.,32},Vec2D{(h+32-1)/32.,32});
    alpaka::exec<Acc2D>(
            queue, workDiv, ScaleKernel{}, in_d.data(), out1_d.data(), img.width_ * 0.5, img.height_ * 0.5);
    alpaka::exec<Acc2D>(
            queue, workDiv, GrayScaleKernel{}, out1_d.data());
    alpaka::exec<Acc2D>(
            queue, workDiv, TintKernel{}, out1_d.data(), out2_d.data(), 168, 56, 172);
    alpaka::exec<Acc2D>(
            queue, workDiv, TintKernel{}, out1_d.data(), out3_d.data(), 100, 143, 47);
    alpaka::exec<Acc2D>(
            queue, workDiv, TintKernel{}, out1_d.data(), out4_d.data(), 255, 162, 36);

    auto out_data =  alpaka::allocAsyncBuf<unsigned char, uint32_t>(queue, Vec1D{h*w*c});
	out_d->data_ = out_data.data();
    WriteTo(queue, img, out1_data, out_data, 0, 0, scale);
    WriteTo(queue, img, out2_data, out_data, w * 0.5, 0, scale);
    WriteTo(queue, img, out3_data, out_data, 0, h * 0.5, scale);
    WriteTo(queue, img, out4_data, out_data, w * 0.5, h * 0.5, scale);

    // copy to host
    auto out_h_data = alpaka::allocMappedBuf<unsigned char, uint32_t>(alpaka::getDevByIdx(platformHost, 0u), platform, Vec1D{w*h*c});
    alpaka::memcpy(queue, out_h, out_d);
    alpaka::memcpy(queue, out_h_data, out_data);
	out_h->data_ = out_h_data.data();
    alpaka::wait(queue);
    Image* const outgpu = alpaka::getPtrNative(out_h);

    std::cout << '\n';
    outgpu->show(columns, rows);
    outgpu->write(fmt::format("out{:02d}.jpg", i));
  }
  auto finish = std::chrono::steady_clock::now();
  float ms = std::chrono::duration_cast<std::chrono::duration<float>>(finish - start).count() * 1000.f;
  if (true) {
    std::cerr << fmt::format("total:      {:6.2f}", ms) << " ms\n";
  }

  return 0;
}
