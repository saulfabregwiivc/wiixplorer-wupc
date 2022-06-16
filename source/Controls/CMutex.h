/****************************************************************************
 * Copyright (C) 2013 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#ifndef CMUTEX_H_
#define CMUTEX_H_

#include <ogc/mutex.h>

class CMutex
{
public:
	CMutex()
		: m_mutex(LWP_MUTEX_NULL)
	{
		LWP_MutexInit(&m_mutex, false);
	}

	CMutex(bool bRecursive)
		: m_mutex(LWP_MUTEX_NULL)
	{
		LWP_MutexInit(&m_mutex, bRecursive);
	}

	virtual ~CMutex() {
		if(LWP_MUTEX_NULL != m_mutex) {
			LWP_MutexUnlock(m_mutex);
			LWP_MutexDestroy(m_mutex);
		}
	}

	void lock(void) {
		if(LWP_MUTEX_NULL != m_mutex)
			LWP_MutexLock(m_mutex);
	}

	void unlock(void) {
		if(LWP_MUTEX_NULL != m_mutex)
			LWP_MutexUnlock(m_mutex);
	}

	void trylock(void) {
		if(LWP_MUTEX_NULL != m_mutex)
			LWP_MutexTryLock(m_mutex);
	}
private:
	mutex_t m_mutex;
};

#endif /* CMUTEX_H_ */
