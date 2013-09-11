//
//  Text/TextBuffer.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 26/09/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_TEXT_TEXTBUFFER_H
#define _DREAM_TEXT_TEXTBUFFER_H

#include "Font.h"
#include <Dream/Core/Strings.h>

namespace Dream
{
	namespace Text
	{
		class TextBuffer : public Object {
		protected:
			Ref<Font> _font;
			Ref<Image> _image;
			std::string _image_text;

			std::string _text;
			bool _text_updated;

			bool _use_static_size;
			Vec2u _size;

		public:
			TextBuffer (Ref<Font> font);
			virtual ~TextBuffer ();

			void set_text (const std::string & text);

			template <typename StringT>
			void append_text (const StringT & text)
			{
				set_text(_text + text);
			}

			template <typename StringT>
			void append_line (const StringT & str)
			{
				if (_text.size() > 0) {
					append_text("\n");
				}
				
				append_text(str);
			}

			void insert_character_at_offset (unsigned offset, unsigned character);
			unsigned offset_for_point (const Vec2u offset);

			void set_static_size (Vec2u size);
			void set_dynamic_size ();

			Ref<Image> render_text (bool & regenerated);
		};
	}
}

#endif
