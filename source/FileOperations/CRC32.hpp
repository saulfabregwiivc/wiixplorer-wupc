/***************************************************************************
 * Copyright (C) 2010
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 * for WiiXplorer 2010
 ***************************************************************************/
#ifndef CRC32_HPP_
#define CRC32_HPP_

class CRC32
{
	public:
		CRC32(const unsigned char *s = 0, unsigned int size = 0) { init(); calc_crc32(s, size); }
		void init() { crc32hash = 0; }
		unsigned long calc_crc32(const unsigned char *s, unsigned int size);
		unsigned long getCRC() const { return crc32hash; }
		static unsigned long CalcComplete(const unsigned char *s, unsigned int size) { CRC32 Calc(s, size); return Calc.getCRC(); }
	private:
		unsigned long crc32hash;
};

#endif
