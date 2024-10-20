#include <alpaka/alpaka.hpp>

namespace math {
  template<typename T>
  T clamp(T value, T low, T high){
    return (value < low) ? low : (high < value) ? high : value;
  }
}

struct ScaleKernel
{
    template<typename TAcc, typename T>
    ALPAKA_FN_ACC void operator()(
        TAcc const& acc,
        T const* __restrict__ in,
        T* __restrict__ out,
        const int width,
        const int height) const
    {
        if (width == in->width_ and height == in->height_) {
          // if the dimensions are the same, return a copy of the image
          *out = *in;
          return;
        }
        out->height_ = height;
        out->width_ = width;
        out->channels_ = in->channels_;
        for(auto ndindex : alpaka::uniformElementsND(acc, Vec2D{width, height}))
        {
          auto y = ndindex[1];
          auto x = ndindex[0]; // * width + ndindex[1];
    // map the row of the scaled image to the nearest rows of the original image
    float yp = static_cast<float>(y) * in->height_ / height;
    int y0 = math::clamp(static_cast<int>(alpaka::math::floor(acc, yp)), 0, in->height_ - 1);
    int y1 = math::clamp(static_cast<int>(alpaka::math::ceil(acc, yp)), 0, in->height_ - 1);

    // interpolate between y0 and y1
    float wy0 = yp - y0;
    float wy1 = y1 - yp;
    // if the new y coorindate maps to an integer coordinate in the original image, use a fake distance from identical values corresponding to it
    if (y0 == y1) {
      wy0 = 1.f;
      wy1 = 1.f;
    }
    float dy = wy0 + wy1;

      int p = (y * out->width_ + x) * out->channels_;

      // map the column of the scaled image to the nearest columns of the original image
      float xp = static_cast<float>(x) * in->width_ / width;
      int x0 = math::clamp(static_cast<int>(alpaka::math::floor(acc, xp)), 0, in->width_ - 1);
      int x1 = math::clamp(static_cast<int>(alpaka::math::ceil(acc, xp)), 0, in->width_ - 1);

      // interpolate between x0 and x1
      float wx0 = xp - x0;
      float wx1 = x1 - xp;
      // if the new x coordinate maps to an integer coordinate in the original image, use a fake distance from identical values corresponding to it
      if (x0 == x1) {
        wx0 = 1.f;
        wx1 = 1.f;
      }
      float dx = wx0 + wx1;

      // bi-linear interpolation of all channels
      int p00 = (y0 * in->width_ + x0) * in->channels_;
      int p10 = (y1 * in->width_ + x0) * in->channels_;
      int p01 = (y0 * in->width_ + x1) * in->channels_;
      int p11 = (y1 * in->width_ + x1) * in->channels_;

      for (int c = 0; c < in->channels_; ++c) {
        out->data_[p + c] =
            static_cast<unsigned char>(alpaka::math::round(acc, (in->data_[p00 + c] * wx1 * wy1 + in->data_[p10 + c] * wx1 * wy0 +
                                                   in->data_[p01 + c] * wx0 * wy1 + in->data_[p11 + c] * wx0 * wy0) /
                                                  (dx * dy)));
      }
        }
    }
};

struct GrayScaleKernel
{
    template<typename TAcc, typename T>
    ALPAKA_FN_ACC void operator()(
        TAcc const& acc,
        T* __restrict__ out) const
    {
        for(auto ndindex : alpaka::uniformElementsND(acc, Vec2D{out->width_, out->height_}))
        {
          auto x = ndindex[0] * out->width_ + ndindex[1];
      int p = x * out->channels_;
      int r = out->data_[p];
      int g = out->data_[p + 1];
      int b = out->data_[p + 2];
      // NTSC values for RGB to grayscale conversion
      int y = (299 * r + 587 * g + 114 * b) / 1000;
      out->data_[p] = y;
      out->data_[p + 1] = y;
      out->data_[p + 2] = y;
        }
    }
};
