/****************************************************************************
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
 * WiiXplorer 2010
 ***************************************************************************/
#ifndef _CPP_CALLBACK_HPP
#define _CPP_CALLBACK_HPP

class cCallback
{
	public:
		virtual void Execute(int Param) const =0;
};

template <class cInstance>
class TCallback : public cCallback
{
	public:
		TCallback()	// constructor
		{
			pFunction = 0;
		}

		typedef void (cInstance::*tFunction)(int Param);

		virtual void Execute(int Param) const
		{
			if (pFunction)
				(cInst->*pFunction)(Param);
		}

		void SetCallback (cInstance * cInstancePointer, tFunction pFunctionPointer)
		{
			cInst	 = cInstancePointer;
			pFunction = pFunctionPointer;
		}

	private:
		cInstance *cInst;
		tFunction pFunction;
};

#endif
