/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [BSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#pragma once

#include <cstring>

#include "colortype.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct pixelcopy_t
  {
    static constexpr std::uint32_t FP_SCALE = 16;
    static constexpr std::uint32_t NON_TRANSP = 1 << 24;

    union {
      std::uint32_t positions[4] = {0};
      struct {
        std::uint32_t src_x32;
        std::uint32_t src_y32;
        std::uint32_t src_xe32;
        std::uint32_t src_ye32;
      };
      struct {
        std::uint16_t src_x_lo;
        std:: int16_t src_x;
        std::uint16_t src_y_lo;
        std:: int16_t src_y;
        std::uint16_t src_xe_lo;
        std:: int16_t src_xe;
        std::uint16_t src_ye_lo;
        std:: int16_t src_ye;
      };
    };

    std::uint32_t src_x32_add = 1 << FP_SCALE;
    std::uint32_t src_y32_add = 0;
    std::uint32_t src_bitwidth = 0;
    std::uint32_t src_width = 0;
    std::uint32_t src_height = 0;
    std::uint32_t transp   = NON_TRANSP;
    union
    {
      color_depth_t src_depth = rgb332_1Byte;
      struct
      {
        std::uint8_t src_bits;
        std::uint8_t src_attrib;
      };
    };
    union
    {
      color_depth_t dst_depth = rgb332_1Byte;
      struct
      {
        std::uint8_t dst_bits;
        std::uint8_t dst_attrib;
      };
    };
    const void* src_data = nullptr;
    const void* palette = nullptr;
    std::uint32_t (*fp_copy)(void*, std::uint32_t, std::uint32_t, pixelcopy_t*) = nullptr;
    std::uint32_t (*fp_skip)(       std::uint32_t, std::uint32_t, pixelcopy_t*) = nullptr;
    std::uint8_t src_mask  = ~0;
    std::uint8_t dst_mask  = ~0;
    bool no_convert = false;

    pixelcopy_t(void) = default;

    pixelcopy_t( const void* src_data
               , color_depth_t dst_depth
               , color_depth_t src_depth
               , bool dst_palette = false
               , const void* src_palette = nullptr
               , std::uint32_t src_transp = NON_TRANSP
               )
    : transp    ( src_transp )
/*
    , src_bits  ( src_depth > 8 ? (src_depth + 7) & ~7 : src_depth)
    , dst_bits  ( dst_depth > 8 ? (dst_depth + 7) & ~7 : dst_depth)
*/
    , src_depth ( src_depth )
    , dst_depth ( dst_depth )
    , src_data  ( src_data   )
    , palette   ( src_palette)
    , src_mask  ( (1 << src_bits) - 1 )
    , dst_mask  ( (1 << dst_bits) - 1 )
    , no_convert( src_depth == dst_depth )
    {
      if (dst_palette || dst_bits < 8) {
        if (src_palette && (dst_bits == 8) && (src_bits == 8)) {
          fp_copy = pixelcopy_t::copy_rgb_affine<rgb332_t, rgb332_t>;
          fp_skip = pixelcopy_t::skip_rgb_affine<rgb332_t>;
        } else {
          fp_copy = pixelcopy_t::copy_bit_affine;
          fp_skip = pixelcopy_t::skip_bit_affine;
        }
      } else
      if (src_palette || src_bits < 8) {
        fp_copy = pixelcopy_t::get_fp_copy_palette_affine<bgr888_t>(dst_depth);
        fp_skip = pixelcopy_t::skip_bit_affine;
      } else {
        if (src_bits > 16) {
          fp_skip = pixelcopy_t::skip_rgb_affine<bgr888_t>;
          if (src_depth == rgb888_3Byte) {
            fp_copy = pixelcopy_t::get_fp_copy_rgb_affine<bgr888_t>(dst_depth);
          } else if (src_depth == rgb666_3Byte) {
            fp_copy = pixelcopy_t::get_fp_copy_rgb_affine<bgr666_t>(dst_depth);
          }
        } else {
          if (src_depth == rgb565_2Byte) {
            fp_copy = pixelcopy_t::get_fp_copy_rgb_affine<swap565_t>(dst_depth);
            fp_skip = pixelcopy_t::skip_rgb_affine<swap565_t>;
          } else { // src_depth == rgb332_1Byte:
            fp_copy = pixelcopy_t::get_fp_copy_rgb_affine<rgb332_t >(dst_depth);
            fp_skip = pixelcopy_t::skip_rgb_affine<rgb332_t>;
          }
        }
      }
    }

    template<typename TSrc>
    static auto get_fp_copy_rgb_affine(color_depth_t dst_depth) -> std::uint32_t(*)(void*, std::uint32_t, std::uint32_t, pixelcopy_t*)
    {
      return (dst_depth == rgb565_2Byte) ? copy_rgb_affine<swap565_t, TSrc>
           : (dst_depth == rgb332_1Byte ) ? copy_rgb_affine<rgb332_t , TSrc>
           : (dst_depth == rgb888_3Byte) ? copy_rgb_affine<bgr888_t, TSrc>
           : (dst_depth == rgb666_3Byte) ? (std::is_same<bgr666_t, TSrc>::value
                                           ? copy_rgb_affine<bgr888_t, bgr888_t>
                                           : copy_rgb_affine<bgr666_t, TSrc>)
           : nullptr;
    }

    template<typename TDst>
    static auto get_fp_copy_rgb_affine_dst(color_depth_t src_depth) -> std::uint32_t(*)(void*, std::uint32_t, std::uint32_t, pixelcopy_t*)
    {
      return (src_depth == rgb565_2Byte) ? copy_rgb_affine<TDst, swap565_t>
           : (src_depth == rgb332_1Byte ) ? copy_rgb_affine<TDst, rgb332_t >
           : (src_depth == rgb888_3Byte) ? copy_rgb_affine<TDst, bgr888_t >
                                         : (std::is_same<bgr666_t, TDst>::value)
                                           ? copy_rgb_affine<bgr888_t, bgr888_t>
                                           : copy_rgb_affine<TDst, bgr666_t>;
    }

    template<typename TPalette>
    static auto get_fp_copy_palette_affine(color_depth_t dst_depth) -> std::uint32_t(*)(void*, std::uint32_t, std::uint32_t, pixelcopy_t*)
    {
      return (dst_depth == rgb565_2Byte) ? copy_palette_affine<swap565_t, TPalette>
           : (dst_depth == rgb332_1Byte ) ? copy_palette_affine<rgb332_t , TPalette>
           : (dst_depth == rgb888_3Byte) ? copy_palette_affine<bgr888_t, TPalette>
           : (dst_depth == rgb666_3Byte) ? copy_palette_affine<bgr666_t, TPalette>
           : nullptr;
    }

    static std::uint32_t copy_bit_fast(void* __restrict__ dst, std::uint32_t index, std::uint32_t last, pixelcopy_t* __restrict__ param)
    {
      auto dst_bits = param->dst_bits;
      auto shift = ((~index) * dst_bits) & 7;
      auto s = static_cast<const std::uint8_t*>(param->src_data);
      auto d = &(static_cast<std::uint8_t*>(dst)[(index * dst_bits) >> 3]);

      std::uint32_t i = param->positions[0] * param->src_bits;
      param->positions[0] += last - index;
      do {
        std::uint32_t raw = s[i >> 3];
        i += param->src_bits;
        raw = (raw >> (-i & 7)) & param->src_mask;
        *d = (*d & ~(param->dst_mask << shift)) | ((param->dst_mask & raw) << shift);
        if (!shift) ++d;
        shift = (shift - dst_bits) & 7;
      } while (++index != last);
      return last;
    }

    template <typename TDst, typename TPalette>
    static std::uint32_t copy_palette_fast(void* __restrict__ dst, std::uint32_t index, std::uint32_t last, pixelcopy_t* __restrict__ param)
    {
      auto s = static_cast<const std::uint8_t*>(param->src_data);
      auto d = static_cast<TDst*>(dst);
      auto pal = static_cast<const TPalette*>(param->palette);
      std::uint32_t i = param->positions[0] * param->src_bits;
      param->positions[0] += last - index;
      do {
        std::uint32_t raw = s[i >> 3];
        i += param->src_bits;
        raw = (raw >> (-i & 7)) & param->src_mask;
        d[index] = pal[raw];
      } while (++index != last);
      return index;
    }

    template <typename TDst, typename TSrc>
    static std::uint32_t copy_rgb_fast(void* dst, std::uint32_t index, std::uint32_t last, pixelcopy_t* param)
    {
      auto s = &static_cast<const TSrc*>(param->src_data)[param->positions[0] - index];
      auto d = static_cast<TDst*>(dst);
      param->positions[0] += last - index;
      if (std::is_same<TDst, TSrc>::value)
      {
        memcpy(reinterpret_cast<void*>(&d[index]), reinterpret_cast<const void*>(&s[index]), (last - index) * sizeof(TSrc));
      }
      else
      {
        do {
          d[index] = s[index];
        } while (++index != last);
      }
      return last;
    }

    static std::uint32_t copy_bit_affine(void* __restrict__ dst, std::uint32_t index, std::uint32_t last, pixelcopy_t* __restrict__ param)
    {
      auto s = static_cast<const std::uint8_t*>(param->src_data);
      auto d = static_cast<std::uint8_t*>(dst);

      do {
        std::uint32_t i = (param->src_x + param->src_y * param->src_bitwidth) * param->src_bits;
        param->src_x32 += param->src_x32_add;
        param->src_y32 += param->src_y32_add;
        std::uint32_t raw = (s[i >> 3] >> (-(i + param->src_bits) & 7)) & param->src_mask;
        if (raw != param->transp) {
          auto dstidx = index * param->dst_bits;
          auto shift = (-(dstidx + param->dst_bits)) & 7;
          auto tmp = &d[dstidx >> 3];
          *tmp = (*tmp & ~(param->dst_mask << shift)) | ((param->dst_mask & raw) << shift);
        }
      } while (++index != last);
      return index;
    }

    template <typename TDst, typename TPalette>
    static std::uint32_t copy_palette_affine(void* __restrict__ dst, std::uint32_t index, std::uint32_t last, pixelcopy_t* __restrict__ param)
    {
      auto s = static_cast<const std::uint8_t*>(param->src_data);
      auto d = static_cast<TDst*>(dst);
      auto pal = static_cast<const TPalette*>(param->palette);
      auto transp     = param->transp;
      do {
        std::uint32_t i = (param->src_x + param->src_y * param->src_bitwidth) * param->src_bits;
        std::uint32_t raw = (s[i >> 3] >> (-(i + param->src_bits) & 7)) & param->src_mask;
        if (raw == transp) break;
        d[index] = pal[raw];
        param->src_x32 += param->src_x32_add;
        param->src_y32 += param->src_y32_add;
      } while (++index != last);
      return index;
    }

    template <typename TDst, typename TSrc>
    static std::uint32_t copy_rgb_affine(void* __restrict__ dst, std::uint32_t index, std::uint32_t last, pixelcopy_t* __restrict__ param)
    {
      auto s = static_cast<const TSrc*>(param->src_data);
      auto d = static_cast<TDst*>(dst);
      auto src_x32_add = param->src_x32_add;
      auto src_y32_add = param->src_y32_add;
      do {
        std::uint32_t i = param->src_x + param->src_y * param->src_bitwidth;
        if (s[i] == param->transp) break;
        d[index] = s[i];
        param->src_x32 += src_x32_add;
        param->src_y32 += src_y32_add;
      } while (++index != last);
      return index;
    }

    template <typename TPalette>
    static std::uint32_t copy_palette_antialias(void* __restrict__ dst, std::uint32_t index, std::uint32_t last, pixelcopy_t* __restrict__ param)
    {
      auto s = static_cast<const std::uint8_t*>(param->src_data);
      auto d = static_cast<argb8888_t*>(dst);
      auto pal = static_cast<const TPalette*>(param->palette);
      auto src_bitwidth= param->src_bitwidth;
      auto src_width   = param->src_width;
      auto src_height  = param->src_height;
      auto transp      = param->transp;
      auto src_bits    = param->src_bits;
      auto src_mask    = param->src_mask;

      param->src_x32 -= param->src_x32_add;
      param->src_xe32 -= param->src_x32_add;
      param->src_y32 -= param->src_y32_add;
      param->src_ye32 -= param->src_y32_add;
      do
      {
        param->src_x32 += param->src_x32_add;
        param->src_xe32 += param->src_x32_add;
        param->src_y32 += param->src_y32_add;
        param->src_ye32 += param->src_y32_add;

        std::int32_t x = param->src_x;
        std::int32_t y = param->src_y;
        if (param->src_x == param->src_xe && param->src_y == param->src_ye && static_cast<std::uint32_t>(param->src_x) < src_width && static_cast<std::uint32_t>(param->src_y) < src_height)
        {
          std::uint32_t i = (x + y * src_bitwidth) * src_bits;
          std::uint32_t raw = (s[i >> 3] >> (-(i + src_bits) & 7)) & src_mask;
          if (!(raw == transp))
          {
            d[index].set(pal[raw].R8(), pal[raw].G8(), pal[raw].B8());
          }
          else
          {
            d[index] = 0u;
          }
        }
        else
        {
          std::uint32_t argb[5] = {0};
          {
            std::uint32_t rate_x = 256u - (param->src_x_lo >> 8);
            std::uint32_t rate_y = 256u - (param->src_y_lo >> 8);
            std::uint32_t i = y * src_bitwidth;
            for (;;)
            {
              std::uint32_t rate = rate_x * rate_y;
              argb[4] += rate;
              if (static_cast<std::uint32_t>(y) < src_height
               && static_cast<std::uint32_t>(x) < src_width)
              {
                std::uint32_t k = (i + x) * src_bits;
                std::uint32_t raw = (s[k >> 3] >> (-(k + src_bits) & 7)) & src_mask;
                if (!(raw == transp))
                {
                  if (std::is_same<TPalette, argb8888_t>::value) { rate *= pal[raw].A8(); }
                  argb[3] += rate;
                  argb[2] += pal[raw].R8() * rate;
                  argb[1] += pal[raw].G8() * rate;
                  argb[0] += pal[raw].B8() * rate;
                }
              }
              if (++x <= param->src_xe)
              {
                rate_x = (x == param->src_xe) ? (param->src_xe_lo >> 8) + 1 : 256u;
              }
              else
              {
                if (++y > param->src_ye) break;
                rate_y = (y == param->src_ye) ? (param->src_ye_lo >> 8) + 1 : 256u;
                x = param->src_x;
                i += src_bitwidth;
                rate_x = 256u - (param->src_x_lo >> 8);
              }
            }
          }
          std::uint32_t a = argb[3];
          if (!a)
          {
            d[index] = 0u;
          }
          else
          {
            d[index].set( (std::is_same<TPalette, argb8888_t>::value ? a : (a * 255)) / argb[4]
                        , argb[2] / a
                        , argb[1] / a
                        , argb[0] / a
                        );
          }
        }
      } while (++index != last);
      return last;
    }

    template <typename TSrc>
    static std::uint32_t copy_rgb_antialias(void* __restrict__ dst, std::uint32_t index, std::uint32_t last, pixelcopy_t* __restrict__ param)
    {
      auto s = static_cast<const TSrc*>(param->src_data);
      auto d = static_cast<argb8888_t*>(dst);
      auto src_width   = param->src_width;
      auto src_height  = param->src_height;

      param->src_x32 -= param->src_x32_add;
      param->src_xe32 -= param->src_x32_add;
      param->src_y32 -= param->src_y32_add;
      param->src_ye32 -= param->src_y32_add;
      do
      {
        param->src_x32 += param->src_x32_add;
        param->src_xe32 += param->src_x32_add;
        param->src_y32 += param->src_y32_add;
        param->src_ye32 += param->src_y32_add;

        std::int32_t x = param->src_x;
        std::int32_t y = param->src_y;
        auto color = &s[x + y * src_width];
        if (param->src_x == param->src_xe && param->src_y == param->src_ye && static_cast<std::uint32_t>(param->src_x) < src_width && static_cast<std::uint32_t>(param->src_y) < src_height)
        {
          if (!(*color == param->transp))
          {
            d[index].set(color->R8(), color->G8(), color->B8());
          }
          else
          {
            d[index] = 0u;
          }
        }
        else
        {
          std::uint32_t argb[5] = {0};
          {
            std::uint32_t rate_y = 256u - (param->src_y_lo >> 8);
            std::uint32_t rate_x = 256u - (param->src_x_lo >> 8);
            for (;;)
            {
              std::uint32_t rate = rate_x * rate_y;
              argb[4] += rate;
              if (static_cast<std::uint32_t>(y) < src_height
               && static_cast<std::uint32_t>(x) < src_width
               && !(*color == param->transp))
              {
                if (std::is_same<TSrc, argb8888_t>::value) { rate *= color->A8(); }
                argb[3] += rate;
                argb[2] += color->R8() * rate;
                argb[1] += color->G8() * rate;
                argb[0] += color->B8() * rate;
              }
              if (x != param->src_xe)
              {
                ++color;
                rate_x = (++x == param->src_xe) ? (param->src_xe_lo >> 8) + 1 : 256u;
              }
              else
              {
                if (++y > param->src_ye) break;
                rate_y = (y == param->src_ye) ? (param->src_ye_lo >> 8) + 1 : 256u;
                x = param->src_x;
                color += x + src_width - param->src_xe;
                rate_x = 256u - (param->src_x_lo >> 8);
              }
            }
          }
          std::uint32_t a = argb[3];
          if (!a)
          {
            d[index] = 0u;
          }
          else
          {
            d[index].set( (std::is_same<TSrc, argb8888_t>::value ? a : (a * 255)) / argb[4]
                        , argb[2] / a
                        , argb[1] / a
                        , argb[0] / a
                        );
          }
        }
//d[index].a = 255;
//d[index].b = 255;
      } while (++index != last);
      return last;
    }


    static std::uint32_t blend_palette_fast(void* __restrict__ dst, std::uint32_t index, std::uint32_t last, pixelcopy_t* __restrict__ param)
    {
      auto dst_bits = param->dst_bits;
      auto dst_mask = param->dst_mask;
      std::uint32_t k = (dst_bits == 1) ? 0xFF
                      : (dst_bits == 2) ? 0x55
                      : (dst_bits == 4) ? 0x11
                                        : 0x01
                                        ;
      auto shift = ((~index) * dst_bits) & 7;
      auto d = &(static_cast<std::uint8_t*>(dst)[(index * dst_bits) >> 3]);
      auto src_x32_add = param->src_x32_add;
      auto src_y32_add = param->src_y32_add;
/*
      if (src_y32_add == 0 && src_x32_add == (1<<FP_SCALE))
      {
        auto s = &(static_cast<const argb8888_t*>(param->src_data)[param->src_x + param->src_y * param->src_bitwidth - index]);
        do {
          std::uint_fast16_t a = s[index].a;
          if (a)
          {
            std::uint32_t raw = (s[index].R8() + (s[index].G8()<<1) + s[index].B8()) >> 2;
            if (a != 255)
            {
              std::uint_fast16_t inv = (256 - a) * k;
              raw = (((*d >> shift) & dst_mask) * inv + raw * ++a) >> 8;
            }
            *d = (*d & ~(dst_mask << shift)) | (dst_mask & (raw >> (8 - dst_bits))) << shift;
          }
          if (!shift) ++d;
          shift = (shift - dst_bits) & 7;
        } while (++index != last);
        return last;
      }
//*/
      auto s = static_cast<const argb8888_t*>(param->src_data);
      do {
        std::uint32_t i = param->src_x + param->src_y * param->src_bitwidth;
        std::uint_fast16_t a = s[i].a;
        if (a)
        {
          std::uint32_t raw = (s[i].R8() + (s[i].G8()<<1) + s[i].B8()) >> 2;
          if (a != 255)
          {
            std::uint_fast16_t inv = (256 - a) * k;
            raw = (((*d >> shift) & dst_mask) * inv + raw * ++a) >> 8;
          }
          *d = (*d & ~(dst_mask << shift)) | (dst_mask & (raw >> (8 - dst_bits))) << shift;
        }
        if (!shift) ++d;
        shift = (shift - dst_bits) & 7;
        param->src_x32 += src_x32_add;
        param->src_y32 += src_y32_add;
      } while (++index != last);
      return last;
    }

    template <typename TDst>
    static std::uint32_t blend_rgb_fast(void* __restrict__ dst, std::uint32_t index, std::uint32_t last, pixelcopy_t* __restrict__ param)
    {
      auto d = static_cast<TDst*>(dst);
      auto src_x32_add = param->src_x32_add;
      auto src_y32_add = param->src_y32_add;
/*
      if (src_y32_add == 0 && src_x32_add == (1<<FP_SCALE))
      {
        auto s = &(static_cast<const argb8888_t*>(param->src_data)[param->src_x + param->src_y * param->src_bitwidth - index]);
        for (;;) {
          std::uint_fast16_t a = s[index].a;
          if (a)
          {
            if (a == 255)
            {
              d[index].set(s[index].r, s[index].g, s[index].b);
              if (++index == last) return last;
              continue;
            }

            std::uint_fast16_t inv = 256 - a;
            ++a;
            d[index].set( (d[index].R8() * inv + s[index].R8() * a) >> 8
                        , (d[index].G8() * inv + s[index].G8() * a) >> 8
                        , (d[index].B8() * inv + s[index].B8() * a) >> 8
                        );
          }
          if (++index == last) return last;
        }
      }
//*/
      auto s = static_cast<const argb8888_t*>(param->src_data);
      for (;;) {
        std::uint32_t i = param->src_x + param->src_y * param->src_bitwidth;
        std::uint_fast16_t a = s[i].a;
        if (a)
        {
          if (a == 255)
          {
            d[index].set(s[i].r, s[i].g, s[i].b);
            param->src_x32 += src_x32_add;
            param->src_y32 += src_y32_add;
            if (++index == last) return last;
            continue;
          }

          std::uint_fast16_t inv = 256 - a;
          ++a;
          d[index].set( (d[index].R8() * inv + s[i].R8() * a) >> 8
                      , (d[index].G8() * inv + s[i].G8() * a) >> 8
                      , (d[index].B8() * inv + s[i].B8() * a) >> 8
                      );
        }
        param->src_x32 += src_x32_add;
        param->src_y32 += src_y32_add;
        if (++index == last) return last;
      }
    }

    static std::uint32_t skip_bit_affine(std::uint32_t index, std::uint32_t last, pixelcopy_t* param)
    {
      auto s = static_cast<const std::uint8_t*>(param->src_data);
      auto src_x32     = param->src_x32;
      auto src_y32     = param->src_y32;
      auto src_x32_add = param->src_x32_add;
      auto src_y32_add = param->src_y32_add;
      auto src_bitwidth= param->src_bitwidth;
      auto transp      = param->transp;
      auto src_bits    = param->src_bits;
      auto src_mask    = param->src_mask;
      do {
        std::uint32_t i = ((src_x32 >> FP_SCALE) + (src_y32 >> FP_SCALE) * src_bitwidth) * src_bits;
        std::uint32_t raw = (s[i >> 3] >> (-(i + src_bits) & 7)) & src_mask;
        if (raw != transp) break;
        src_x32 += src_x32_add;
        src_y32 += src_y32_add;
      } while (++index != last);
      param->src_x32 = src_x32;
      param->src_y32 = src_y32;
      return index;
    }

    template <typename TSrc>
    static std::uint32_t skip_rgb_affine(std::uint32_t index, std::uint32_t last, pixelcopy_t* param)
    {
      auto s = static_cast<const TSrc*>(param->src_data);
      auto src_x32     = param->src_x32;
      auto src_y32     = param->src_y32;
      auto src_x32_add = param->src_x32_add;
      auto src_y32_add = param->src_y32_add;
      auto src_bitwidth= param->src_bitwidth;
      auto transp      = param->transp;
      do {
        std::uint32_t i = (src_x32 >> FP_SCALE) + (src_y32 >> FP_SCALE) * src_bitwidth;
        if (!(s[i] == transp)) break;
        src_x32 += src_x32_add;
        src_y32 += src_y32_add;
      } while (++index != last);
      param->src_x32 = src_x32;
      param->src_y32 = src_y32;
      return index;
    }

    template <typename TSrc>
    static std::uint32_t compare_rgb_fast(void* __restrict__ dst, std::uint32_t index, std::uint32_t last, pixelcopy_t* __restrict__ param)
    {
      auto s = static_cast<const TSrc*>(param->src_data);
      auto d = static_cast<bool*>(dst);
      auto transp = param->transp;
      s += param->src_x + param->src_y * param->src_bitwidth - index;
      do {
        d[index] = s[index] == transp;
      } while (++index != last);
      return index;
    }

    static std::uint32_t compare_bit_fast(void* __restrict__ dst, std::uint32_t index, std::uint32_t last, pixelcopy_t* __restrict__ param)
    {
      auto s = static_cast<const std::uint8_t*>(param->src_data);
      auto d = static_cast<bool*>(dst);
      auto transp      = param->transp;
      auto src_bits    = param->src_bits;
      auto src_mask    = param->src_mask;
      std::uint32_t i = (param->src_x + param->src_y * param->src_bitwidth) * src_bits;
      do {
        d[index] = transp == ((s[i >> 3] >> (-(i + src_bits) & 7)) & src_mask);
        i += src_bits;
      } while (++index != last);
      return index;
    }
  };

//----------------------------------------------------------------------------
 }
}


