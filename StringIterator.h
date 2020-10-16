/**
 * StringIterator.h
 *
 * Copyright 2019 mikee47 <mike@sillyhouse.net>
 *
 * This file is part of the RapidXML Library
 *
 * This library is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, version 3 or later.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with FlashString.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 ****/

#pragma once

#include <WString.h>

class StringIterator
{
public:
	StringIterator(String& string) : string(reinterpret_cast<StringBuffer*>(&string))
	{
	}

	operator char*()
	{
		if(!*string) {
			string->setLength(0);
		}
		return string->curPtr();
	}

	StringIterator& operator++()
	{
		string->inc();
		return *this;
	}

	char* operator++(int)
	{
		string->inc();
		return string->curPtr() - 1;
	}

private:
	class StringBuffer : public String
	{
	public:
		char* curPtr()
		{
			return begin() + length();
		}

		bool inc()
		{
			auto len = length() + 1;
			setlen(len);
			auto cap = capacity();
			return (length() < cap) ?: reserve(cap + 64);
		}
	};

	StringBuffer* string;
};
