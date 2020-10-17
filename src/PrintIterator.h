/**
 * PrintIterator.h
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

#include "PrintBuffer.h"

class PrintIterator
{
public:
	PrintIterator(PrintBuffer& buffer) : buffer(&buffer)
	{
	}

	operator char*()
	{
		return buffer->curPtr();
	}

	PrintIterator& operator++()
	{
		buffer->inc();
		buffer->flush();
		return *this;
	}

	char* operator++(int)
	{
		buffer->flush();
		char* result = buffer->curPtr();
		buffer->inc();
		return result;
	}

private:
	PrintBuffer* buffer;
};
