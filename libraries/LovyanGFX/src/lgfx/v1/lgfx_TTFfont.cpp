#include "lgfx_TTFfont.hpp"

#include "platforms/common.hpp"
#include "misc/pixelcopy.hpp"
#include "LGFXBase.hpp"

#include <algorithm>

//#include "../lgfx_debug.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  TTFfont::~TTFfont()
  {
    unloadFont();
  }

  std::size_t TTFfont::drawChar(LGFXBase* gfx, std::int32_t x, std::int32_t y, std::uint16_t c, const TextStyle* style, FontMetrics* metrics, std::int32_t& filled_x) const
  {
// unimplemented
    return 0;
  }

  void TTFfont::getDefaultMetric(FontMetrics *metrics) const
  {
// unimplemented
metrics->x_offset = 0;
metrics->width = _horizontal.advance_Width_Max;
metrics->x_advance = _horizontal.advance_Width_Max;
metrics->y_advance = _vertical.advance_Height_Max;
metrics->y_offset = 0;
metrics->baseline = _vertical.Ascender;
  }

  static void _load_metrics(std::uint16_t *width, std::int16_t *bearing, DataWrapper* data, std::size_t glyph_index, std::size_t k, std::size_t table_pos, std::size_t table_size)
  {
    std::size_t table_end = table_pos + table_size;

    bool contain = glyph_index < k;
    table_pos += 4 * (contain ? glyph_index : (k - 1));

    if ( table_pos + 4 <= table_end )
    {
      data->seek( table_pos );
      *width = data->read16swap();
      bool has_data = contain;
      if (!contain)
      {
        table_pos += 4 + 2 * (glyph_index - k);
        if (table_pos + 2 <= table_end)
        {
          data->seek( table_pos );
          has_data = true;
        }
      }
      if (has_data)
      {
        *bearing = data->read16swap();
      }
    }
  }

  TTFfont::Metrics_t TTFfont::_get_metrics(std::uint32_t glyph_index) const
  {
// ---- tt_face_get_metrics
    Metrics_t res;
    res.advance_height = _header.Units_Per_EM;

    _load_metrics(&res.advance_width, &res.left_bearing, _fontData, glyph_index, _horizontal.number_Of_HMetrics, _horz_metrics_offset, _horz_metrics_size);
    if (_vertical_info)
      _load_metrics(&res.advance_height, &res.top_bearing, _fontData, glyph_index, _horizontal.number_Of_HMetrics, _vert_metrics_offset, _vert_metrics_size);

//  LGFX_DEBUG_LOG( "width:%d lb:%d height:%d tb:%d \r\n", res.advance_width, res.left_bearing, res.advance_height, res.top_bearing);

    return res;
  }

  bool TTFfont::updateFontMetric(FontMetrics *metrics, std::uint16_t uniCode) const
  {
//LGFX_DEBUG_LOG("uniCode:%04x \r\n", uniCode);
    std::uint16_t glyph_index;
    if (!_get_glyph_index(uniCode, &glyph_index)) return false;

    auto data = _fontData;
    data->preRead();
    auto ttf_mtx = _get_metrics(glyph_index);
    data->postRead();

metrics->x_advance = ttf_mtx.advance_width  >> 6;
metrics->width     = ttf_mtx.advance_width  >> 6;
metrics->y_advance = ttf_mtx.advance_height >> 6;
metrics->y_offset  = ttf_mtx.top_bearing    >> 6;
metrics->height    = ttf_mtx.advance_height >> 6;

    return true;
  }

  bool TTFfont::_get_glyph_index(std::uint16_t unicode, std::uint16_t *index) const
  {
    std::uint_fast16_t num_segs2      = _cmap.rawdata[3];
    //std::uint_fast16_t search_range   = _cmap.rawdata[4];
    //std::uint_fast16_t entry_selector = _cmap.rawdata[5];
    //std::uint_fast16_t range_shift    = _cmap.rawdata[6];
    std::uint_fast16_t num_segs = num_segs2 >> 1;

    auto charcode = unicode;

    auto end_array   = &_cmap.rawdata[7];
    auto start_array = &_cmap.rawdata[8 + num_segs];
    auto poi = std::lower_bound(end_array, &end_array[num_segs], unicode);
    auto idx = std::distance(end_array, poi);

    for (; start_array[idx] <= charcode && end_array[idx] >= charcode; ++idx)
    {
      auto delta  = start_array[idx + num_segs ];
      auto offset = &start_array[idx + num_segs2];
      if ( *offset == 0xFFFFu) continue;
      if ( *offset )
      {
        *index = offset[((*offset) >> 1) + charcode - start_array[idx]];
        if (*index)
        {
          *index = (*index + delta) & 0xFFFFu;
// LGFX_DEBUG_LOG("hasOffset:%d  idx:%d\r\n", *offset, *index);
        }
      }
      else
      {
        *index = (charcode + delta) & 0xFFFFu;
// LGFX_DEBUG_LOG("  idx:%d\r\n", *index);
      }
      return (*index <= _max_profile.numGlyphs);
    }
// LGFX_DEBUG_LOG("start_array:%04x end_array:%04x idx:%d \r\n", start_array[idx], end_array[idx], idx);
    return false;
  }

  bool TTFfont::loadFont(DataWrapper* data)
  {
    #define FT_MAKE_TAG( _x1, _x2, _x3, _x4 ) \
              ( ( (std::uint32_t)_x1 << 24 ) |     \
                ( (std::uint32_t)_x2 << 16 ) |     \
                ( (std::uint32_t)_x3 <<  8 ) |     \
                  (std::uint32_t)_x4         )

    static constexpr std::uint32_t TTAG_ttcf = FT_MAKE_TAG( 't', 't', 'c', 'f' );
    static constexpr std::uint32_t TTAG_true = FT_MAKE_TAG( 't', 'r', 'u', 'e' );
//    static constexpr std::uint32_t TTAG_OTTO = FT_MAKE_TAG( 'O', 'T', 'T', 'O' );
//    static constexpr std::uint32_t TTAG_typ1 = FT_MAKE_TAG( 't', 'y', 'p', '1' );

    static constexpr std::uint32_t TTAG_glyf = FT_MAKE_TAG( 'g', 'l', 'y', 'f' );
    static constexpr std::uint32_t TTAG_CFF  = FT_MAKE_TAG( 'C', 'F', 'F', ' ' );

    static constexpr std::uint32_t TTAG_head = FT_MAKE_TAG( 'h', 'e', 'a', 'd' );
    static constexpr std::uint32_t TTAG_maxp = FT_MAKE_TAG( 'm', 'a', 'x', 'p' );
    static constexpr std::uint32_t TTAG_cmap = FT_MAKE_TAG( 'c', 'm', 'a', 'p' );
//    static constexpr std::uint32_t TTAG_name = FT_MAKE_TAG( 'n', 'a', 'm', 'e' );
    static constexpr std::uint32_t TTAG_hhea = FT_MAKE_TAG( 'h', 'h', 'e', 'a' );
    static constexpr std::uint32_t TTAG_hmtx = FT_MAKE_TAG( 'h', 'm', 't', 'x' );
    static constexpr std::uint32_t TTAG_vhea = FT_MAKE_TAG( 'v', 'h', 'e', 'a' );
    static constexpr std::uint32_t TTAG_vmtx = FT_MAKE_TAG( 'v', 'm', 't', 'x' );
    static constexpr std::uint32_t TTAG_OS2  = FT_MAKE_TAG( 'O', 'S', '/', '2' );
//    static constexpr std::uint32_t TTAG_gasp = FT_MAKE_TAG( 'g', 'a', 's', 'p' );
//    static constexpr std::uint32_t TTAG_kern = FT_MAKE_TAG( 'k', 'e', 'r', 'n' );

    unloadFont();

    _fontData = data;

    std::uint32_t tag = data->read32swap();

    if (tag != 0x00010000UL
     && tag != 0x00020000UL
     && tag != TTAG_true
     && tag != TTAG_ttcf
//     && tag != TTAG_OTTO
//     && tag != TTAG_typ1 
    )
    {
      return false;
    }

    if ( tag == TTAG_ttcf )
    {
      _ttc_header.version = data->read32swap();
      _ttc_header.count   = data->read32swap();
      if (_ttc_header.count <= 0)
      {
        return false;
      }
      _ttc_header.offsets = (std::size_t*)lgfx::heap_alloc(4 * _ttc_header.count);
      for (std::uint32_t i = 0; i < _ttc_header.count; i++)
      {
        _ttc_header.offsets[i] = data->read32swap();
      }
    }
    else
    {
      _ttc_header.version = 1 << 16;
      _ttc_header.count   = 1;
      _ttc_header.offsets = (std::size_t*)lgfx::heap_alloc(4);
      _ttc_header.offsets[0] = 0;
    }

//-----

    if (_ttc_header.count >= face_index) face_index = 0;

    _sfnt.offset = _ttc_header.offsets[face_index];

    if ( ! data->seek( _sfnt.offset ) )
    {
      return false;
    }
    _sfnt.format_tag     = data->read32swap();
    _sfnt.num_tables     = data->read16swap();
    _sfnt.search_range   = data->read16swap();
    _sfnt.entry_selector = data->read16swap();
    _sfnt.range_shift    = data->read16swap();

    if (_sfnt.format_tag != 0x00010000UL
     && _sfnt.format_tag != 0x00020000UL
     && _sfnt.format_tag != TTAG_true)
    {
      return false;
    }

//LGFX_DEBUG_LOG("-- Number of tables: %10u ",    _sfnt.num_tables );
//LGFX_DEBUG_LOG("-- Format version:   0x%08lx", _sfnt.format_tag );
    // if ( _sfnt.format_tag != TTAG_OTTO ) { check_table_dir }
    if (_sfnt.num_tables <= 0)
    {
      return false;
    }

    _dir_tables = (TT_TableRec*)lgfx::heap_alloc(sizeof(TT_TableRec) * _sfnt.num_tables);
    if ( _dir_tables == nullptr )
    {
      return false;
    }

    auto entry = _dir_tables;

    int i = 0;
    do
    {
      entry[i].Tag      = data->read32swap();
      entry[i].CheckSum = data->read32swap();
      entry[i].Offset   = data->read32swap();
      entry[i].Length   = data->read32swap();
/*
      LGFX_DEBUG_LOG("  %c%c%c%c  %08lx  %08lx  %08lx",
                    (char)( entry[i].Tag >> 24 ),
                    (char)( entry[i].Tag >> 16 ),
                    (char)( entry[i].Tag >> 8  ),
                    (char)( entry[i].Tag       ),
                    entry[i].Offset,
                    entry[i].Length,
                    entry[i].CheckSum );
//*/
    } while (++i < _sfnt.num_tables);

// ---- sfnt_load_face
    // check has outline
    if ( tt_face_lookup_table( TTAG_glyf ) == nullptr
     &&  tt_face_lookup_table( TTAG_CFF )  == nullptr )
    {
//ESP_LOGI("lgfx_fonts","not outline font");
      return false;
    }

    if (!tt_face_goto_table( TTAG_head, data )) { return false; }
    _header.load(data);

//ESP_LOGI("lgfx_fonts", "Units per EM: %4u", _header.Units_Per_EM );
//ESP_LOGI("lgfx_fonts", "IndexToLoc:   %4d", _header.Index_To_Loc_Format );
    if (!tt_face_goto_table( TTAG_maxp, data )) { return false; }
    _max_profile.load(data);

    //if (!tt_face_goto_table( TTAG_cmap, data, &_cmap_size )) { return false; }
    if (!tt_face_goto_table( TTAG_cmap, data )) { return false; }

    if (!_cmap.load(data)) { return false; }
/*
    _cmap_table = (std::uint8_t*)lgfx::heap_alloc_psram(_cmap_size);
    if (_cmap_table == nullptr)
    {
      _cmap_table = (std::uint8_t*)lgfx::heap_alloc(_cmap_size);
    }
    if (_cmap_table == nullptr) { return false; }
    data->read(_cmap_table, _cmap_size);
//*/
//ESP_LOGI("lgfx_fonts", "cmap_size: %u", _cmap_size );
//ESP_LOGI("lgfx_fonts", "cmap_table: %04x", (int)_cmap_table );

//  if (!tt_face_goto_table( TTAG_name, data )) { return false; }

// ---- tt_face_load_hhea
    if (!tt_face_goto_table( TTAG_hhea, data )) { return false; }
    _horizontal.load(data);

    if (!tt_face_goto_table( TTAG_hmtx, data, &_horz_metrics_size)) { return false; }
    _horz_metrics_offset = data->tell();

    _vertical_info = false;
    if (tt_face_goto_table( TTAG_vhea, data ))
    {
      _vertical.load(data);
      if (tt_face_goto_table( TTAG_vmtx, data, &_vert_metrics_size))
      {
        _vert_metrics_offset = data->tell();
        _vertical_info = true;
      }
    }

// ---- tt_face_load_os2
    if (tt_face_goto_table( TTAG_OS2, data ))
    {
      _os2.load(data);
    }
    else
    {
      _os2.version = 0xFFFFU;
    }

// ---- tt_face_load_gasp
//  if (tt_face_goto_table( TTAG_gasp, data ))

// ---- tt_face_load_kern
//  if (tt_face_goto_table( TTAG_kern, data ))


// ---- flags setting



// ---- tt_face_load_hdmx




// ---- find_unicode_charmap


    return true;
  }

  bool TTFfont::unloadFont(void)
  {
    if (_ttc_header.offsets != nullptr) { heap_free(_ttc_header.offsets); _ttc_header.offsets = nullptr; }
    if (_dir_tables != nullptr) { heap_free(_dir_tables);  _dir_tables = nullptr; }
    if (_cmap.rawdata != nullptr) { heap_free(_cmap.rawdata);  _cmap.rawdata = nullptr; }

    _fontLoaded = false;

    return true;
  }

  TTFfont::TT_TableRec* TTFfont::tt_face_lookup_table(std::uint32_t tag)
  {
    TT_TableRec* entry = _dir_tables;
    TT_TableRec* limit = entry + _sfnt.num_tables;
    for ( ; entry < limit; entry++ )
    {
      if ( entry->Tag != tag ) continue;
      if ( entry->Length == 0 ) continue;
//    ESP_LOGI(( "found table.\n" ));
      return entry;
    }
    return nullptr;
  }

  bool TTFfont::tt_face_goto_table( std::uint32_t tag
                                  , DataWrapper* data
                                  , std::uint32_t* length )
  {
    auto table = tt_face_lookup_table( tag );
    if ( table )
    {
      if ( length )
        *length = table->Length;

      if ( data->seek( table->Offset ) )
        return true;
    }
    return false;
  }

  void TTFfont::TT_Header::load(DataWrapper* data)
  {
    data->read((std::uint8_t*)this, 54);
    Table_Version      = __builtin_bswap32(Table_Version      );
    Font_Revision      = __builtin_bswap32(Font_Revision      );
    CheckSum_Adjust    = __builtin_bswap32(CheckSum_Adjust    );
    Magic_Number       = __builtin_bswap32(Magic_Number       );
    Flags              = __builtin_bswap16(Flags              );
    Units_Per_EM       = __builtin_bswap16(Units_Per_EM       );
    Created[0]         = __builtin_bswap32(Created[0]         );
    Created[1]         = __builtin_bswap32(Created[1]         );
    Modified[0]        = __builtin_bswap32(Modified[0]        );
    Modified[1]        = __builtin_bswap32(Modified[1]        );
    xMin               = __builtin_bswap16(xMin               );
    yMin               = __builtin_bswap16(yMin               );
    xMax               = __builtin_bswap16(xMax               );
    yMax               = __builtin_bswap16(yMax               );
    Mac_Style          = __builtin_bswap16(Mac_Style          );
    Lowest_Rec_PPEM    = __builtin_bswap16(Lowest_Rec_PPEM    );
    Font_Direction     = __builtin_bswap16(Font_Direction     );
    Index_To_Loc_Format= __builtin_bswap16(Index_To_Loc_Format);
    Glyph_Data_Format  = __builtin_bswap16(Glyph_Data_Format  );
  }

  void TTFfont::TT_MaxProfile::load(DataWrapper* data)
  {
    memset(this, 0, sizeof(TTFfont::TT_MaxProfile));
    data->read((std::uint8_t*)this, 6);
    version              = __builtin_bswap32(version  );
    numGlyphs            = __builtin_bswap16(numGlyphs);
//ESP_LOGI("lgfx_fonts", "numGlyphs: %u", numGlyphs );
    if ( version >= 0x10000L )
    {
      data->read((std::uint8_t*)&maxPoints, 26);

      maxPoints            = __builtin_bswap16(maxPoints           );
      maxContours          = __builtin_bswap16(maxContours         );
      maxCompositePoints   = __builtin_bswap16(maxCompositePoints  );
      maxCompositeContours = __builtin_bswap16(maxCompositeContours);
      maxZones             = __builtin_bswap16(maxZones            );
      maxTwilightPoints    = std::min<std::uint16_t>( 0xFFFFu - 4, __builtin_bswap16(maxTwilightPoints));
      maxStorage           = __builtin_bswap16(maxStorage);
      maxFunctionDefs      = std::max<std::uint16_t>(64u, __builtin_bswap16(maxFunctionDefs));
      maxInstructionDefs   = __builtin_bswap16(maxInstructionDefs   );
      maxStackElements     = __builtin_bswap16(maxStackElements     );
      maxSizeOfInstructions= __builtin_bswap16(maxSizeOfInstructions);
      maxComponentElements = __builtin_bswap16(maxComponentElements );
      maxComponentDepth    = std::max<std::uint16_t>(100, __builtin_bswap16(maxComponentDepth));
    }
  }

  void TTFfont::TT_HoriHeader::load(DataWrapper* data)
  {
    data->read((std::uint8_t*)this, 36);
    Version                = __builtin_bswap32(Version               );
    Ascender               = __builtin_bswap16(Ascender              );
    Descender              = __builtin_bswap16(Descender             );
    Line_Gap               = __builtin_bswap16(Line_Gap              );
    advance_Width_Max      = __builtin_bswap16(advance_Width_Max     );
    min_Left_Side_Bearing  = __builtin_bswap16(min_Left_Side_Bearing );
    min_Right_Side_Bearing = __builtin_bswap16(min_Right_Side_Bearing);
    xMax_Extent            = __builtin_bswap16(xMax_Extent           );
    caret_Slope_Rise       = __builtin_bswap16(caret_Slope_Rise      );
    caret_Slope_Run        = __builtin_bswap16(caret_Slope_Run       );
    caret_Offset           = __builtin_bswap16(caret_Offset          );
    Reserved[0]            = __builtin_bswap16(Reserved[0]           );
    Reserved[1]            = __builtin_bswap16(Reserved[1]           );
    Reserved[2]            = __builtin_bswap16(Reserved[2]           );
    Reserved[3]            = __builtin_bswap16(Reserved[3]           );
    metric_Data_Format     = __builtin_bswap16(metric_Data_Format    );
    number_Of_HMetrics     = __builtin_bswap16(number_Of_HMetrics    );

    long_metrics  = nullptr;
    short_metrics = nullptr;
  }

  void TTFfont::TT_OS2::load(DataWrapper* data)
  {
    data->read((std::uint8_t*)this, 78);
    version             = __builtin_bswap16(version            );
    xAvgCharWidth       = __builtin_bswap16(xAvgCharWidth      );
    usWeightClass       = __builtin_bswap16(usWeightClass      );
    usWidthClass        = __builtin_bswap16(usWidthClass       );
    fsType              = __builtin_bswap16(fsType             );
    ySubscriptXSize     = __builtin_bswap16(ySubscriptXSize    );
    ySubscriptYSize     = __builtin_bswap16(ySubscriptYSize    );
    ySubscriptXOffset   = __builtin_bswap16(ySubscriptXOffset  );
    ySubscriptYOffset   = __builtin_bswap16(ySubscriptYOffset  );
    ySuperscriptXSize   = __builtin_bswap16(ySuperscriptXSize  );
    ySuperscriptYSize   = __builtin_bswap16(ySuperscriptYSize  );
    ySuperscriptXOffset = __builtin_bswap16(ySuperscriptXOffset);
    ySuperscriptYOffset = __builtin_bswap16(ySuperscriptYOffset);
    yStrikeoutSize      = __builtin_bswap16(yStrikeoutSize     );
    yStrikeoutPosition  = __builtin_bswap16(yStrikeoutPosition );
    sFamilyClass        = __builtin_bswap16(sFamilyClass       );
    // uint8 panose[0~9]
    ulUnicodeRange1     = __builtin_bswap32(ulUnicodeRange1    );
    ulUnicodeRange2     = __builtin_bswap32(ulUnicodeRange2    );
    ulUnicodeRange3     = __builtin_bswap32(ulUnicodeRange3    );
    ulUnicodeRange4     = __builtin_bswap32(ulUnicodeRange4    );
    // uint8 achVendID[0~3]
    fsSelection         = __builtin_bswap16(fsSelection        );
    usFirstCharIndex    = __builtin_bswap16(usFirstCharIndex   );
    usLastCharIndex     = __builtin_bswap16(usLastCharIndex    );
    sTypoAscender       = __builtin_bswap16(sTypoAscender      );
    sTypoDescender      = __builtin_bswap16(sTypoDescender     );
    sTypoLineGap        = __builtin_bswap16(sTypoLineGap       );
    usWinAscent         = __builtin_bswap16(usWinAscent        );
    usWinDescent        = __builtin_bswap16(usWinDescent       );

    ulCodePageRange1 = 0;
    ulCodePageRange2 = 0;
    sxHeight         = 0;
    sCapHeight       = 0;
    usDefaultChar    = 0;
    usBreakChar      = 0;
    usMaxContext     = 0;

    if ( version >= 0x0001 )
    {
      // only version 1 tables
      data->read((std::uint8_t*)&ulCodePageRange1, 8);
      ulCodePageRange1 = __builtin_bswap32(ulCodePageRange1);
      ulCodePageRange2 = __builtin_bswap32(ulCodePageRange2);

      if ( version >= 0x0002 )
      {
        // only version 2 tables
        data->read((std::uint8_t*)&sxHeight, 10);
        sxHeight      = __builtin_bswap16(sxHeight     );
        sCapHeight    = __builtin_bswap16(sCapHeight   );
        usDefaultChar = __builtin_bswap16(usDefaultChar);
        usBreakChar   = __builtin_bswap16(usBreakChar  );
        usMaxContext  = __builtin_bswap16(usMaxContext );
      }
    }
// Serial.printf("OS2 version:%d \r\n", version);
  }


  static constexpr std::uint16_t TT_PLATFORM_APPLE_UNICODE = 0;
  static constexpr std::uint16_t TT_PLATFORM_MICROSOFT     = 3;
  static constexpr std::uint16_t TT_APPLE_ID_UNICODE_32    = 4;
  static constexpr std::uint16_t TT_MS_ID_UCS_4            =10;
  static constexpr std::uint16_t TT_MS_ID_UNICODE_CS       = 1;

  bool TTFfont::cmap_t::load(DataWrapper* data)
  {
    auto cmap_pos = data->tell();
    std::uint16_t table_ver = data->read16swap();
    if (table_ver != 0) return false;

    std::uint16_t numTables = data->read16swap();

//Serial.printf("cmap table version:%d num:%d\r\n", table_ver, numTables);

/// Unicode かつ Format 4 の CMAP を探す
    for (std::size_t idx = 0; idx < numTables; ++idx)
    {
      std::uint16_t platform_id = data->read16swap();
      std::uint16_t encoding_id = data->read16swap();
      std::uint32_t offset      = data->read32swap();
//Serial.printf("pid:%d  eid:%d  offset:%d \r\n", platform_id, encoding_id, offset);

/// プラットフォームIDとエンコードIDの組み合わせからunicodeのものを探す
      if ((platform_id == TT_PLATFORM_MICROSOFT
       &&  encoding_id == TT_MS_ID_UCS_4)
        ||(platform_id == TT_PLATFORM_MICROSOFT
       &&  encoding_id == TT_MS_ID_UNICODE_CS)
        ||(platform_id == TT_PLATFORM_APPLE_UNICODE
       &&  encoding_id == TT_APPLE_ID_UNICODE_32))
      {
        auto cmap_prev_pos = data->tell();

        data->seek(cmap_pos + offset);
        std::uint16_t cmap_format = data->read16swap();
//Serial.printf("format: %d\r\n", cmap_format);
// CMAP FORMAT 4 か否か判定
        if (cmap_format == 4)
        {
          std::uint16_t length = data->read16swap();

          rawdata = (std::uint16_t*)lgfx::heap_alloc_psram(length);
          if (rawdata == nullptr)
          {
            rawdata = (std::uint16_t*)lgfx::heap_alloc(length);
          }
          if (rawdata == nullptr) return false;

          data->seek(cmap_pos + offset);
          data->read(reinterpret_cast<std::uint8_t*>(rawdata), length);
          length >>= 1;
          std::size_t idx = 0;
          do
          {
            rawdata[idx] = __builtin_bswap16(rawdata[idx]);
          } while (++idx != length);
/*
data->seek(cmap_pos + offset);
Serial.println("CMAP dump");
for (int j = 0; j < 8; ++j) {
for (int i = 0; i < 8; ++i) { Serial.printf(" %04x", rawdata[i+j*8]); }
Serial.println();
}
std::uint16_t num_segs       = rawdata[3]; // __builtin_bswap16(num_segs      ) >> 1;
std::uint16_t search_range   = rawdata[4]; // __builtin_bswap16(search_range  );
std::uint16_t entry_selector = rawdata[5]; // __builtin_bswap16(entry_selector);
std::uint16_t range_shift    = rawdata[6]; // __builtin_bswap16(range_shift   );
Serial.printf("len:%d num_segs:%d search_range:%d entry_selector:%d range_shift:%d \r\n", length, num_segs, search_range, entry_selector, range_shift);
//*/
          return true;
        }
        data->seek(cmap_prev_pos);
      }
    }

//Serial.println("cmap not found...");
    return false;
  }
/*
  bool TTFfont::tt_face_build_cmaps( void )
  {
    std::uint8_t* table = _cmap_table;
    std::uint8_t* limit = table + _cmap_size;
    unsigned int volatile num_cmaps;
    std::uint8_t* volatile  p = table;

    if ( !p || p + 4 > limit )
      return false;

    Serial.println("CMAP dump");
    for (int j = 0; j < 16; ++j)
    {
      for (int i = 0; i < 16; ++i)
      {
        Serial.printf(" %02x", table[i+j*16]);
      }
      Serial.println();
    }
    return true;
  }
//*/
//----------------------------------------------------------------------------
 }
}
