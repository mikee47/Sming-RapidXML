/**
 * PrintBuffer.h
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

#include <Print.h>

class PrintBuffer
{
public:
	PrintBuffer(Print& out) : out(out)
	{
	}

	~PrintBuffer()
	{
		flushAll();
	}

	char* curPtr()
	{
		return &buf[len];
	}

	void inc()
	{
		++len;
		assert(len < bufSize);
	}

	void flush()
	{
		constexpr auto min = bufSize - 4;
		if(len >= min) {
			count_ += out.write(buf, min);
			len -= min;
			memmove(buf, &buf[min], len);
		}
	}

	void flushAll()
	{
		if(len > 0) {
			count_ += out.write(buf, len);
			len = 0;
		}
	}

	unsigned count()
	{
		return count_ + len;
	}

private:
	Print& out;
	static constexpr unsigned bufSize = 64;
	char buf[bufSize];
	uint16_t count_ = 0;
	uint8_t len = 0;
};
