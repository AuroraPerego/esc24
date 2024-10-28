#include <alpaka/alpaka.hpp>
#include <iostream>

namespace math {
template <typename T>
ALPAKA_FN_ACC T clamp(T const &value, T const &low, T const &high) {
  return (value < low) ? low : (high < value) ? high : value;
}
} // namespace math

struct ScaleKernel {
  template <typename TAcc, typename T>
  ALPAKA_FN_ACC void operator()(TAcc const &acc, T const *__restrict__ in,
                                T *__restrict__ out, const int widthIn,
                                const int heightIn, const int channels,
                                const float scale) const {
    if (scale == 1) {
      // if the dimensions are the same, nothing to be done
      *out = *in;
      return;
    }
    const int widthOut = widthIn * scale;
    const int heightOut = heightIn * scale;
    for (auto ndindex :
         alpaka::uniformElementsND(acc, Vec2D{widthOut, heightOut})) {
      auto y = ndindex[1];
      auto x = ndindex[0];
      // map the row of the scaled image to the nearest rows of the original
      // image
      float yp = static_cast<float>(y) * heightIn / heightOut;
      int y0 = math::clamp(static_cast<int>(alpaka::math::floor(acc, yp)), 0,
                           heightIn - 1);
      int y1 = math::clamp(static_cast<int>(alpaka::math::ceil(acc, yp)), 0,
                           heightIn - 1);

      // interpolate between y0 and y1
      float wy0 = yp - y0;
      float wy1 = y1 - yp;
      // if the new y coorindate maps to an integer coordinate in the original
      // image, use a fake distance from identical values corresponding to it
      if (y0 == y1) {
        wy0 = 1.f;
        wy1 = 1.f;
      }
      float dy = wy0 + wy1;

      int p = (y * widthOut + x) * channels;

      // map the column of the scaled image to the nearest columns of the
      // original image
      float xp = static_cast<float>(x) * widthIn / widthOut;
      int x0 = math::clamp(static_cast<int>(alpaka::math::floor(acc, xp)), 0,
                           widthIn - 1);
      int x1 = math::clamp(static_cast<int>(alpaka::math::ceil(acc, xp)), 0,
                           widthIn - 1);

      // interpolate between x0 and x1
      float wx0 = xp - x0;
      float wx1 = x1 - xp;
      // if the new x coordinate maps to an integer coordinate in the original
      // image, use a fake distance from identical values corresponding to it
      if (x0 == x1) {
        wx0 = 1.f;
        wx1 = 1.f;
      }
      float dx = wx0 + wx1;

      // bi-linear interpolation of all channels
      int p00 = (y0 * widthIn + x0) * channels;
      int p10 = (y1 * widthIn + x0) * channels;
      int p01 = (y0 * widthIn + x1) * channels;
      int p11 = (y1 * widthIn + x1) * channels;

      for (int c = 0; c < channels; ++c) {
        out[p + c] = static_cast<unsigned char>(alpaka::math::round(
            acc, (in[p00 + c] * wx1 * wy1 + in[p10 + c] * wx1 * wy0 +
                  in[p01 + c] * wx0 * wy1 + in[p11 + c] * wx0 * wy0) /
                     (dx * dy)));
      }
    }
  }
};

struct GrayScaleKernel {
  template <typename TAcc, typename T>
  ALPAKA_FN_ACC void operator()(TAcc const &acc, T *__restrict__ out,
                                const int width, const int height,
                                const int channels) const {
    for (auto ndindex : alpaka::uniformElementsND(acc, Vec2D{width, height})) {
      auto x = ndindex[1] * width + ndindex[0];
      int p = x * channels;
      int r = out[p];
      int g = out[p + 1];
      int b = out[p + 2];
      // NTSC values for RGB to grayscale conversion
      int y = (299 * r + 587 * g + 114 * b) / 1000;
      out[p] = y;
      out[p + 1] = y;
      out[p + 2] = y;
    }
  }
};

struct TintKernel {
  template <typename TAcc, typename T>
  ALPAKA_FN_ACC void operator()(TAcc const &acc, T const *__restrict__ in,
                                T *__restrict__ out, const int r, const int g,
                                const int b, const int width, const int height,
                                const int channels) const {
    for (auto ndindex : alpaka::uniformElementsND(acc, Vec2D{width, height})) {
      auto x = ndindex[1] * width + ndindex[0];
      int p = x * channels;
      int r0 = in[p];
      int g0 = in[p + 1];
      int b0 = in[p + 2];
      out[p] = r0 * r / 255;
      out[p + 1] = g0 * g / 255;
      out[p + 2] = b0 * b / 255;
    }
  }
};

template <typename TQueue, typename T>
void WriteTo(TQueue &queue, T &in, T &out, const int x, const int y,
             const int w, const int h, const int c, const float scale) {
  // the whole source image would fall outside of the target image along the X
  // axis
  if ((x + (int)(scale * w) < 0) or (x >= w))
    return;
  // the whole source image would fall outside of the target image along the Y
  // axis
  if ((y + (int)(scale * h) < 0) or (y >= h))
    return;

  // find the valid range for the overlapping part of the images along the X and
  // Y axes
  int src_x_from = std::max(0, -x);
  int src_x_to = std::min((int)(w * scale), w - x);
  int dst_x_from = std::max(0, x);
  int x_width = src_x_to - src_x_from;

  int src_y_from = std::max(0, -y);
  int src_y_to = std::min((int)(h * scale), h - y);
  int dst_y_from = std::max(0, y);
  int y_height = src_y_to - src_y_from;

  for (int y = 0; y < y_height; ++y) {
    int src_p = ((src_y_from + y) * (int)(w * scale) + src_x_from) * c;
    int dst_p = ((dst_y_from + y) * w + dst_x_from) * c;
    auto subViewIn =
        alpaka::createSubView(in, Vec1D{x_width * c}, Vec1D{src_p});
    auto subViewOut =
        alpaka::createSubView(out, Vec1D{x_width * c}, Vec1D{dst_p});
    alpaka::memcpy(queue, std::move(subViewOut), subViewIn, Vec1D{x_width * c});
  }
}
