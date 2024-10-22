#include <alpaka/alpaka.hpp>

namespace math {
  template<typename T>
  ALPAKA_FN_ACC T clamp(T const& value, T const& low, T const& high){
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
          auto x = ndindex[0];
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
          auto x = ndindex[1] * out->width_ + ndindex[0];
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

struct TintKernel
{
    template<typename TAcc, typename T>
	ALPAKA_FN_ACC void operator()(
        TAcc const& acc,
        T const* __restrict__ in,
        T* __restrict__ out,
        const int r,
        const int g,
        const int b) const
	{
        out->height_ = in->height_;
        out->width_ = in->width_;
        out->channels_ = in->channels_;
        for(auto ndindex : alpaka::uniformElementsND(acc, Vec2D{out->width_, out->height_}))
		{
          auto x = ndindex[1] * out->width_ + ndindex[0];
            int p = x * out->channels_;
            int r0 = out->data_[p];
            int g0 = out->data_[p + 1];
            int b0 = out->data_[p + 2];
            out->data_[p] = r0 * r / 255;
            out->data_[p + 1] = g0 * g / 255;
            out->data_[p + 2] = b0 * b / 255;
        }
	}

};

template<typename TQueue, typename Image, typename T>
void WriteTo (TQueue& queue, Image const& img, T& in, T& out, const int x, const int y, const float scale)
{
  const auto w = img.width_;
  const auto h = img.height_;
  const auto c = img.channels_;
  // the whole source image would fall outside of the target image along the X axis
  if ((x + (int)(scale*w) < 0) or (x >= w))
    return;
  // the whole source image would fall outside of the target image along the Y axis
  if ((y + (int)(scale*h) < 0) or (y >= h))
	return;

  // find the valid range for the overlapping part of the images along the X and Y axes
  int src_x_from = std::max(0, -x);
  int src_x_to = std::min((int)(w*scale), w - x);
  int dst_x_from = std::max(0, x);
  int x_width = src_x_to - src_x_from;

  int src_y_from = std::max(0, -y);
  int src_y_to = std::min((int)(h*scale), h - y);
  int dst_y_from = std::max(0, y);
  int y_height = src_y_to - src_y_from;

  for (int y = 0; y < y_height; ++y) {
    int src_p = ((src_y_from + y) * (int)(w*scale) + src_x_from) * c;
    int dst_p = ((dst_y_from + y) * w + dst_x_from) * c;
	auto const& subViewIn = alpaka::createSubView(in, Vec1D{x_width * c}, Vec1D{src_p});
	auto subViewOut = alpaka::createSubView(out, Vec1D{x_width * c}, Vec1D{dst_p});
	alpaka::memcpy(queue, std::move(subViewOut), subViewIn, Vec1D{x_width * c});
  }
}

//struct WriteToKernel
//{
//    template<typename TAcc, typename T>
//	ALPAKA_FN_ACC void operator()(
//        TAcc const& acc,
//        const int x,
//        const int y) const
//	{
//         // the whole source image would fall outside of the target image along the X axis
//         if ((x + in->width_ < 0) or (x >= out->width_)) {
//           return;
//         }
//
//         // the whole source image would fall outside of the target image along the Y axis
//         if ((y + in->height_ < 0) or (y >= out->height_)) {
//           return;
//         }
//
//         // find the valid range for the overlapping part of the images along the X and Y axes
//         int src_x_from = alpaka::math::max(acc, 0, -x);
//         int src_x_to = alpaka::math::min(acc, in->width_, out->width_ - x);
//         int dst_x_from = alpaka::math::max(acc, 0, x);
//         //int dst_x_to   = alpaka::math::min(acc, in->width_ + x, out->width_);
//         int x_width = src_x_to - src_x_from;
//
//         int src_y_from = alpaka::math::max(acc, 0, -y);
//         int src_y_to = alpaka::math::min(acc, in->height_, out->height_ - y);
//         int dst_y_from = alpaka::math::max(acc, 0, y);
//         //int dst_y_to   = std::min(in->height_ + y, out->height_);
//         int y_height = src_y_to - src_y_from;
//
//         for(auto y : alpaka::uniformElements(acc, out->height_)) {
//           int src_p = ((src_y_from + y) * in->width_ + src_x_from) * in->channels_;
//           int dst_p = ((dst_y_from + y) * out->width_ + dst_x_from) * out->channels_;
//		   out->data_[dst_p] = in->data_[src_p];
//		   //auto dest = out->data_ + dst_p;
//		   //dest = in->data_ + src_p; // FIXME
//         }
//	}
//};
