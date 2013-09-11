//
//  Text/TextBuffer.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 26/09/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#include "TextBuffer.h"
#include "TextBlock.h"

namespace Dream
{
	namespace Text
	{
		using namespace Euclid::Numerics::Constants;

// MARK: -

		TextBuffer::TextBuffer (Ref<Font> font) : _font(font), _text_updated(true), _use_static_size(false)
		{
		}

		TextBuffer::~TextBuffer ()
		{
		}

		void TextBuffer::set_static_size (Vec2u size) {
			_size = size;
			_use_static_size = true;
		}

		void TextBuffer::set_dynamic_size () {
			_size = 0;
			_use_static_size = false;
		}

		void TextBuffer::set_text (const std::string & text)
		{
			if (_text != text) {
				_text = text;
				_text_updated = true;
			}
		}

		void TextBuffer::insert_character_at_offset (unsigned offset, unsigned character) {
			_text_updated = true;
			_text.insert(_text.begin()+offset, character);
		}

		unsigned TextBuffer::offset_for_point (const Vec2u offset) {
			return _text.size();
		}

		Ref<Image> TextBuffer::render_text (bool & regenerated)
		{
			regenerated = false;

			if (_text_updated) {
				if (_image_text != _text) {
					TextBlock text_block(_font->font_face());

					if (_use_static_size) {
						text_block.set_line_width(_size[X]);
					}

					text_block.set_text(_text);
					Vec2u text_block_size = text_block.calculate_size();

					if (_image)
						_image->resize(text_block_size, PixelFormat::L, DataType::BYTE);
					else
						_image = new Image(text_block_size, PixelFormat::L, DataType::BYTE);

					_image->fill();

					text_block.render(_image);
					regenerated = true;
				}

				_image_text = _text;
				_text_updated = false;
			}

			return _image;
		}
	}
}
