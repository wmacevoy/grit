#pragma once

#include <vector>
#include <assert.h>

#include <string.h>

template <class T, unsigned int staticCount, class Alloc = std::allocator<T> >
class CM_Array {

protected:

	unsigned int m_count;

	T* m_ptr;

	T m_static[staticCount];
	std::vector<T, Alloc> m_vector;

public:

	CM_Array(unsigned int count = 0)
	{
		setCount(count);
	}
	virtual ~CM_Array()
	{}

	inline T& operator[](int i)
	{
		return operator[]((unsigned int)i);
	}
	inline T& operator[](unsigned int i)
	{
		assert(i < m_count);
		return m_ptr[i];
	}

	inline T* getData()
	{
		return m_ptr;
	}

	inline int getCount()
	{
		return m_count;
	}
	inline void setCount(int newCount)
	{
		return setCount((unsigned int)newCount);
	}
	inline void setCount(unsigned int newCount)
	{
		if (m_ptr == m_static && newCount > staticCount)
		{
			m_vector.resize(newCount);
			memcpy(m_vector.data(), m_static, m_count);
			m_ptr = m_vector.data();

			assert(false);  // for debugging to ensure that everything is fitting on the stack
		}
		else
		{
			m_vector.resize(newCount);
			m_ptr = m_vector.data();
		}
		m_count = newCount;
	}

	std::size_t getSize()
	{
		return sizeof(T) * (std::size_t)m_count;
	}

};
