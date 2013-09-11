//
//  Text/FontFace.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 30/11/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "FontFace.h"
#include "TextBlock.h"
#include <Dream/Events/Logger.h>

namespace Dream
{
	namespace Text
	{
		namespace Detail
		{
			using namespace Events::Logging;

// MARK: -
			FontGlyph::~FontGlyph ()
			{
				FT_Done_Glyph(glyph);
			}

			void FontGlyph::composite_to_buffer(Vec2u origin, Ref<Image> img) const
			{
				DREAM_ASSERT(is_bitmap());
				FT_BitmapGlyph bm = (FT_BitmapGlyph)glyph;

				if (bm->bitmap.buffer != NULL) {
					// 1 byte per pixel:
					auto bitmap_reader = reader(bm->bitmap.buffer, Vec2u(bm->bitmap.width, bm->bitmap.rows), 1);
					auto size = bitmap_reader.size();

					auto image_writer = writer(*img);

					// This blit copies the pixels flipped, e.g. the rows are copied in reverse order. That is because this text library assumes top-to-bottom text means that the origin goes from small to big, which implies that for traditional western text, the origin of the image should be at the bottom, which isn't typical, except in OpenGL.
					Vec2u offset(size[X], 0);
					for (std::size_t i = 0; i < size[Y]; i += 1) {
						Vec2u source_row(0, size[Y] - (i + 1));
						Vec2u destination_row(origin[X], origin[Y] + i);

						std::copy(bitmap_reader[source_row], bitmap_reader[source_row + offset], image_writer[destination_row]);
					}
				}
			}

			Vec2u FontGlyph::calculate_character_origin (Vec2u pen) const
			{
				DREAM_ASSERT(is_bitmap());
				FT_BitmapGlyph bm = (FT_BitmapGlyph)glyph;

				Vec2u origin;
				origin[X] = (pen[X] >> 6) + bm->left;
				origin[Y] = (pen[Y] >> 6) - (bm->bitmap.rows - bm->top);

				return origin;
			}

			void FontGlyph::get_cbox(FT_UInt bbox_mode, FT_BBox * acbox) const
			{
				FT_Glyph_Get_CBox(glyph, bbox_mode, acbox);
			}

			bool FontGlyph::is_bitmap () const
			{
				return glyph->format == FT_GLYPH_FORMAT_BITMAP;
			}

			bool FontGlyph::is_outline () const
			{
				return glyph->format == FT_GLYPH_FORMAT_OUTLINE;
			}

			unsigned FontGlyph::hinting_adjustment(unsigned prev_rsbdelta) const
			{
				if (prev_rsbdelta - lsb_delta >= 32)
					return 64;
				else if (prev_rsbdelta - lsb_delta < -32)
					return 64;

				return 0;
			}

// MARK: -

			FontFace::FontFace (FT_Face _face, PixelFormat _fmt) : _face(_face), _pixel_format(_fmt)
			{
			}

			FontFace::~FontFace ()
			{
				int count = 0;

				for (auto glyph : _glyph_cache) {
					delete glyph.second;
					count += 1;
				}

				FT_Done_Face(_face);

				log_debug("Freed", count, "cached glyphs.");
			}

			bool FontFace::has_kerning ()
			{
				return FT_HAS_KERNING(_face);
			}

			// These three functions need >> 6 to convert to pixels
			unsigned FontFace::line_offset () const
			{
				return _face->size->metrics.height >> 6;
			}

			unsigned FontFace::ascender_offset () const
			{
				return _face->size->metrics.ascender >> 6;
			}

			unsigned FontFace::descender_offset() const
			{
				return std::abs(_face->size->metrics.descender >> 6);
			}

			FT_UInt FontFace::get_char_index (FT_UInt c)
			{
				return FT_Get_Char_Index(_face, c);
			}

			FT_Face FontFace::face ()
			{
				return _face;
			}

			PixelFormat FontFace::pixel_format ()
			{
				return _pixel_format;
			}

			FontGlyph * FontFace::load_glyph_for_index (FT_UInt idx)
			{
				//FT_UInt idx = FT_Get_Char_Index(_face, c);
				GlyphMapT::iterator itr = _glyph_cache.find(idx);

				if (itr != _glyph_cache.end()) {
					return itr->second;
				}

				FT_Error err = FT_Load_Glyph(_face, idx, FT_LOAD_RENDER);
				if (err) throw TypographyException(err);

				FontGlyph * cache = new FontGlyph;

				// Copy glyph into cache
				FT_Get_Glyph(_face->glyph, &cache->glyph);
				cache->advance = _face->glyph->advance;
				cache->lsb_delta = _face->glyph->lsb_delta;
				cache->rsb_delta = _face->glyph->rsb_delta;

				_glyph_cache[idx] = cache;

				return cache;
			}

			Vec2u FontFace::process_text(const std::string& text, Ref<Image> dst)
			{
				TextBlock block(this);

				block.set_text(text);

				if (dst) {
					block.render(dst);
				}

				Vec2u size = block.calculate_size();

				return size;
			}
		}
	}
}
