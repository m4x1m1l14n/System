#pragma once
#include <unknwnbase.h>

template <class T>
class ComPtrBase
{
protected:
	ComPtrBase() throw()
	{
		p = NULL;
	}

	ComPtrBase(T* lp) throw()
	{
		p = lp;
		if (p != NULL)
			p->AddRef();
	}

	void Swap(ComPtrBase& other)
	{
		T* pTemp = p;
		p = other.p;
		other.p = pTemp;
	}

public:
	~ComPtrBase() throw()
	{
		if (p)
			p->Release();
	}

	operator T*() const throw()
	{
		return p;
	}

	T& operator*() const
	{
		/*ATLENSURE(p != NULL);*/
		return *p;
	}
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	T** operator&() throw()
	{
		/*ATLASSERT(p == NULL);*/
		return &p;
	}

	T* operator->() const throw()
	{
		return p;
	}
	/*_NoAddRefReleaseOnCComPtr<T>* operator->() const throw()
	{
	ATLASSERT(p != NULL);
	return (_NoAddRefReleaseOnCComPtr<T>*)p;
	}*/

	bool operator!() const throw()
	{
		return (p == NULL);
	}

	bool operator<(T* pT) const throw()
	{
		return p < pT;
	}

	bool operator!=(T* pT) const
	{
		return !operator==(pT);
	}

	bool operator==(T* pT) const throw()
	{
		return p == pT;
	}

	// Release the interface and set to NULL
	void Release() throw()
	{
		T* pTemp = p;
		if (pTemp)
		{
			p = NULL;
			pTemp->Release();
		}
	}

	// Compare two objects for equivalence
	bool IsEqualObject(IUnknown* pOther) throw()
	{
		if (p == NULL && pOther == NULL)
			return true;	// They are both NULL objects

		if (p == NULL || pOther == NULL)
			return false;	// One is NULL the other is not

		ComPtr<IUnknown> punk1;
		ComPtr<IUnknown> punk2;

		p->QueryInterface(__uuidof(IUnknown), (void**)&punk1);
		pOther->QueryInterface(__uuidof(IUnknown), (void**)&punk2);

		return punk1 == punk2;
	}

	// Attach to an existing interface (does not AddRef)
	void Attach(T* p2) throw()
	{
		if (p)
		{
			ULONG ref = p->Release();
			(ref);
			// Attaching to the same object only works if duplicate references are being coalesced.  Otherwise
			// re-attaching will cause the pointer to be released and may cause a crash on a subsequent dereference.
			/*ATLASSERT(ref != 0 || p2 != p);*/
		}
		p = p2;
	}

	// Detach the interface (does not Release)
	T* Detach() throw()
	{
		T* pt = p;
		p = NULL;
		return pt;
	}

	T* p;
};


template <class T>
class ComPtr :
	public ComPtrBase < T >
{
public:
	ComPtr() throw()
	{
	}

	ComPtr(T* lp) throw()
		: ComPtrBase<T>(lp)
	{
	}

	ComPtr(const ComPtr<T>& lp) throw()
		: ComPtrBase<T>(lp.p)
	{
	}

	T* operator=(T* lp) throw()
	{
		if (*this != lp)
		{
			ComPtr(lp).Swap(*this);
		}

		return *this;
	}

	template <typename Q>
	T* operator=(const ComPtr<Q>& lp) throw()
	{
		if (!IsEqualObject(lp))
		{
			IUnknown* ppvObj = NULL;
			HRESULT hr = ((IUnknown*)&lp)->QueryInterface(__uuidof(T), (void**)&ppvObj);
			if (SUCCEEDED(hr))
			{
				((IUnknown*)p)->Release();
				p = ppvObj;
			}

			return static_cast<T*>(p);
			/*return static_cast<T*>(AtlComQIPtrAssign((IUnknown**)&p, lp, __uuidof(T)));*/
		}

		return *this;
	}

	T* operator=(const ComPtr<T>& lp) throw()
	{
		if (*this != lp)
		{
			ComPtr(lp).Swap(*this);
		}

		return *this;
	}

	ComPtr(ComPtr<T>&& lp) throw()
		: ComPtrBase<T>()
	{
		lp.Swap(*this);
	}

	T* operator=(ComPtr<T>&& lp) throw()
	{
		if (*this != lp)
		{
			ComPtr(static_cast<ComPtr&&>(lp)).Swap(*this);
		}

		return *this;
	}
};
