
#ifndef __DATASET__H__
#define __DATASET__H__
typedef int (*LPCompareItemVal)(void* pItem,void* pVal);

/*
	说明:
			AutoDeletePtr用于单个数据实例指针的自动释放,
		不能再赋予另一个AutoDeletePtr,所有从该实例获取到
		的指针必须在该AutoDeletePtr的生命期内使用

*/
template <typename T>
struct AutoDeletePtr
{
	T* _ptr;
public:
	AutoDeletePtr(T* lp=NULL)
	{
		_ptr=lp;			
	}
	~AutoDeletePtr()
	{
		if (_ptr)
		{
			delete _ptr;
			_ptr=0;
		}
	}
	operator T*() const
	{
		return (T*)_ptr;
	}
	T& operator*() const
	{
		ATLASSERT(_ptr!=NULL);
		return *_ptr;
	}
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	T** operator&()
	{
		ATLASSERT(_ptr==NULL);
		return &_ptr;
	}
	T* operator->() const
	{
		ATLASSERT(_ptr!=NULL);
		return _ptr;
	}
	T* operator=(T* lp)
	{
		if(_ptr) delete _ptr;
		_ptr=lp;
		return _ptr;
	}
	bool operator!() const
	{
		return (_ptr == NULL);
	}
	bool operator<(T* pT) const
	{
		return _ptr < pT;
	}
	bool operator==(T* pT) const
	{
		return _ptr == pT;
	}
	void Attach(T* p2)
	{
		if(_ptr) delete _ptr;
		_ptr=p2;
	}
	T* Detach()
	{
		T* lp=_ptr;
		if(_ptr) delete _ptr;
		return lp;
	}
};

template<class TYPE>
inline void DataSet_CopyElements(TYPE* pDest, const TYPE* pSrc, int nCount)
{
	while (nCount--)
		*pDest++ = *pSrc++;
}

#define BEFORE_START_POSPTR ((POSPTR)-1L)
struct __POSPTR {};
typedef __POSPTR* POSPTR;

template<typename TYPE,typename ARG_TYPE=TYPE&,int _GROWBY=0>
struct BaseArray
{
	BaseArray()
	{
		m_pData = NULL;
		m_nSize = m_nMaxSize = 0;

	}
	~BaseArray()
	{
		if (m_pData != NULL)
		{
			for( int i = 0; i < m_nSize; i++ )
				(m_pData + i)->~TYPE();
			delete[] (BYTE*)m_pData;
		}
	}

	TYPE& _NullItem()
	{
		static TYPE* _nullItem=0;
		return *_nullItem;
	}
	// Attributes
	int GetSize() const{ return m_nSize; };
	int GetCount() const{ return m_nSize; };
	bool IsEmpty() const{ return m_nSize == 0; };
	int GetUpperBound() const{ return m_nSize-1; };
	void SetSize(int nNewSize)
	{
		int nGrowBy=_GROWBY;
		ASSERT(nNewSize >= 0);

		if(nNewSize < 0 )
			return;


		if (nNewSize == 0)
		{
			// shrink to nothing
			if (m_pData != NULL)
			{
				for( int i = 0; i < m_nSize; i++ )
					(m_pData + i)->~TYPE();
				delete[] (BYTE*)m_pData;
				m_pData = NULL;
			}
			m_nSize = m_nMaxSize = 0;
		}
		else if (m_pData == NULL)
		{
			// create buffer big enough to hold number of requested elements or
			// m_nGrowBy elements, whichever is larger.
			size_t nAllocSize = max(nNewSize, nGrowBy);
			m_pData = (TYPE*) new BYTE[(size_t)nAllocSize * sizeof(TYPE)];
			memset((void*)m_pData, 0, (size_t)nAllocSize * sizeof(TYPE));
			for( int i = 0; i < nNewSize; i++ )
#pragma push_macro("new")
#undef new
				::new( (void*)( m_pData + i ) ) TYPE;
#pragma pop_macro("new")
			m_nSize = nNewSize;
			m_nMaxSize = nAllocSize;
		}
		else if (nNewSize <= m_nMaxSize)
		{
			// it fits
			if (nNewSize > m_nSize)
			{
				// initialize the new elements
				memset((void*)(m_pData + m_nSize), 0, (size_t)(nNewSize-m_nSize) * sizeof(TYPE));
				for( int i = 0; i < nNewSize-m_nSize; i++ )
#pragma push_macro("new")
#undef new
					::new( (void*)( m_pData + m_nSize + i ) ) TYPE;
#pragma pop_macro("new")
			}
			else if (m_nSize > nNewSize)
			{
				// destroy the old elements
				for( int i = 0; i < m_nSize-nNewSize; i++ )
					(m_pData + nNewSize + i)->~TYPE();
			}
			m_nSize = nNewSize;
		}
		else
		{
			// otherwise, grow array
			if (nGrowBy == 0)
			{
				// heuristically determine growth when nGrowBy == 0
				//  (this avoids heap fragmentation in many situations)
				nGrowBy = m_nSize / 8;
				nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
			}
			int nNewMax;
			if (nNewSize < m_nMaxSize + nGrowBy)
				nNewMax = m_nMaxSize + nGrowBy;  // granularity
			else
				nNewMax = nNewSize;  // no slush

			ASSERT(nNewMax >= m_nMaxSize);  // no wrap around

			if(nNewMax  < m_nMaxSize) return;
				

			TYPE* pNewData = (TYPE*) new BYTE[(size_t)nNewMax * sizeof(TYPE)];

			// copy new data from old
			memcpy((void*)pNewData,(void*)m_pData, (size_t)m_nSize * sizeof(TYPE));
			//CopyElements(m_pData,m_pData,m_nSize);
			// construct remaining elements
			ASSERT(nNewSize > m_nSize);
			memset((void*)(pNewData + m_nSize), 0, (size_t)(nNewSize-m_nSize) * sizeof(TYPE));
			for( int i = 0; i < nNewSize-m_nSize; i++ )
#pragma push_macro("new")
#undef new
				::new( (void*)( pNewData + m_nSize + i ) ) TYPE;
#pragma pop_macro("new")

			// get rid of old stuff (note: no destructors called)
			delete[] (BYTE*)m_pData;
			m_pData = pNewData;
			m_nSize = nNewSize;
			m_nMaxSize = nNewMax;
		}
	}//end SetSize

	// Operations
	// Clean up
	void FreeExtra()
	{
		if (m_nSize != m_nMaxSize)
		{
			// shrink to desired size
			TYPE* pNewData = NULL;
			if (m_nSize != 0)
			{
				pNewData = (TYPE*) new BYTE[m_nSize * sizeof(TYPE)];
				// copy new data from old
				memcpy(pNewData,	m_pData, m_nSize * sizeof(TYPE));
			}

			// get rid of old stuff (note: no destructors called)
			delete[] (BYTE*)m_pData;
			m_pData = pNewData;
			m_nMaxSize = m_nSize;
		}
	}

	void RemoveAll(){ SetSize(0); };

	// Accessing elements
	const TYPE& GetAt(int nIndex) const
	{
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		if(nIndex >= 0 && nIndex < m_nSize)
			return m_pData[nIndex]; 
	}

	TYPE& GetAt(int nIndex)
	{ 
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		if(nIndex >= 0 && nIndex < m_nSize)
			return m_pData[nIndex]; 
		return _NullItem();
	}
	void SetAt(int nIndex, ARG_TYPE newElement)
	{ 
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		if(nIndex >= 0 && nIndex < m_nSize)
			m_pData[nIndex] = newElement; 
	}

	const TYPE& ElementAt(int nIndex) const
	{ 
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		if(nIndex >= 0 && nIndex < m_nSize)
			return m_pData[nIndex]; 
		return _NullItem();
	}

	TYPE& ElementAt(int nIndex)
	{ 
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		if(nIndex >= 0 && nIndex < m_nSize)
			return m_pData[nIndex]; 
		return _NullItem();
	}

	// Direct Access to the element data (may return NULL)
	const TYPE* GetData() const{ return (const TYPE*)m_pData; }
	TYPE* GetData(){ return (TYPE*)m_pData; }

	// Potentially growing the array
	void SetAtGrow(int nIndex, ARG_TYPE newElement)
	{
		if(nIndex < 0) return;

		if (nIndex >= m_nSize)
			SetSize(nIndex+1);
		m_pData[nIndex] = newElement;
	}

	int Add(const ARG_TYPE newElement)
	{
		int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex;
	}
	TYPE& Append()
	{
		SetSize(m_nSize+1);
		return m_pData[m_nSize-1];
	}
	

	int Append(const BaseArray& src)
	{

		if(this == &src)
			return 0;
		int nOldSize = m_nSize;
		SetSize(m_nSize + src.m_nSize);
		DataSet_CopyElements<TYPE>(m_pData + nOldSize, src.m_pData, src.m_nSize);
		return nOldSize;
	}

	void Copy(const BaseArray& src)
	{
		if(this != &src)
		{
			SetSize(src.m_nSize);
			DataSet_CopyElements<TYPE>(m_pData, src.m_pData, src.m_nSize);
		}
	}


	// overloaded operator helpers
	const TYPE& operator[](int nIndex) const{ return GetAt(nIndex); };
	TYPE& operator[](int nIndex){ return ElementAt(nIndex); };

	// Operations that move elements around
	void InsertAt(int nIndex, ARG_TYPE newElement, int nCount = 1)
	{
		if(nIndex < 0 || nCount <= 0) return;

		if (nIndex >= m_nSize)
		{
			// adding after the end of the array
			SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
		}
		else
		{
			// inserting in the middle of the array
			int nOldSize = m_nSize;
			SetSize(m_nSize + nCount, -1);  // grow it to new size
			// destroy intial data before copying over it
			for( int i = 0; i < nCount; i++ )
				(m_pData + nOldSize + i)->~TYPE();
			// shift old data up to fill gap
			memmove(m_pData + nIndex + nCount, m_pData + nIndex, (nOldSize-nIndex) * sizeof(TYPE));

			// re-init slots we copied from
			memset((void*)(m_pData + nIndex), 0, (size_t)nCount * sizeof(TYPE));
			for( int i = 0; i < nCount; i++ )
#pragma push_macro("new")
#undef new
				::new( (void*)( m_pData + nIndex + i ) ) TYPE;
#pragma pop_macro("new")
		}

		// insert new value in the gap
		ASSERT(nIndex + nCount <= m_nSize);
		while (nCount--)
			m_pData[nIndex++] = newElement;
	}

	void RemoveAt(int nIndex, int nCount = 1)
	{
		ASSERT(nIndex >= 0);
		ASSERT(nCount >= 0);
		int nUpperBound = nIndex + nCount;
		ASSERT(nUpperBound <= m_nSize && nUpperBound >= nIndex && nUpperBound >= nCount);

		if(nIndex < 0 || nCount < 0 || (nUpperBound > m_nSize) || (nUpperBound < nIndex) || (nUpperBound < nCount))
			return;

		// just remove a range
		int nMoveCount = m_nSize - (nUpperBound);
		for( int i = 0; i < nCount; i++ )
			(m_pData + nIndex + i)->~TYPE();
		if (nMoveCount)
		{
			memmove(m_pData + nIndex,m_pData + nUpperBound, (size_t)nMoveCount * sizeof(TYPE));
		}
		m_nSize -= nCount;
	}

	void InsertAt(int nStartIndex, BaseArray* pNewArray)
	{
		ASSERT(pNewArray != NULL);
		ASSERT(nStartIndex >= 0);

		if(pNewArray == NULL || nStartIndex < 0) return;
		if (pNewArray->GetSize() > 0)
		{
			InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
			for (int i = 0; i < pNewArray->GetSize(); i++)
				SetAt(nStartIndex + i, pNewArray->GetAt(i));
		}
	}
	int FindItem(void* pVal,LPCompareItemVal pCompare)
	{
		for (int i=0;i<m_nSize;i++)
		{
			if(pCompare(&GetAt(i),pVal)==0)
			{
				return i;
			}
		}
		return -1;
	}

	// Implementation
protected:
	TYPE* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
};
//
struct MemPlex     // warning variable length structure
{
	MemPlex* pNext;
	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static MemPlex* Create(MemPlex*& pHead, UINT nMax, UINT cbElement)
	{
		MemPlex* p=(MemPlex*)new char[sizeof(MemPlex)+nMax*cbElement];
		p->pNext=pHead;
		pHead=p;
		return p;
	}
	// like 'calloc' but no zero fill
	// may throw memory exceptions

	void FreeDataChain()       // free this one and links
	{
		MemPlex* p=this;
		while (p!=0)
		{
			char* pBytes=(char*)p;
			MemPlex* pn=p->pNext;
			delete pBytes;
			p=pn;
		}
	}
};

#ifdef __ENV_WIN
#else

template<class TYPE, class ARG_TYPE>
bool CompareElements(const TYPE* pElement1, const ARG_TYPE* pElement2)
{
	return *pElement1 == *pElement2;
}

//struct ldiv_t
//{
//	long quot;
//	long rem;
//	ldiv_t(long numer=0,long denom=1)
//	{
//		quot=numer/denom;rem=numer%denom;
//	}
//};

//#define ldiv(numer,denom) ldiv_t(numer,denom)

template<class ARG_KEY>
inline UINT DataSet_HashKey(ARG_KEY key)
{
	// (algorithm copied from STL hash in xfunctional)
	ldiv_t HashVal;
	//((long)(ARG_KEY)key, 127773);
	long number=(long)(ARG_KEY)key;
	HashVal.quot=number/127773;
	HashVal.rem = 16807 * HashVal.rem - 2836 * HashVal.quot;
	if (HashVal.rem < 0)
		HashVal.rem += 2147483647;
	return ((UINT)HashVal.rem);
}

template<> inline UINT DataSet_HashKey<INT64>(INT64 key)
{
	// (algorithm copied from STL hash in xfunctional)
	return (DataSet_HashKey<DWORD>((DWORD)(key & 0xffffffff)) ^ DataSet_HashKey<DWORD>((DWORD)(key >> 32)));
}

#endif

template<typename TYPE, typename ARG_TYPE = const TYPE&>
class BaseList
{
protected:
	struct BLNode
	{
		BLNode* pNext;
		BLNode* pPrev;
		TYPE data;
	};
public:
// Construction
	/* explicit */ BaseList(int nBlockSize = 10);

// Attributes (head and tail)
	// count of elements
	int GetCount() const;
	int GetSize() const;
	bool IsEmpty() const;

	// peek at head or tail
	ARG_TYPE GetHead();
	const ARG_TYPE GetHead() const;
	ARG_TYPE GetTail();
	const ARG_TYPE GetTail() const;

// Operations
	// get head or tail (and remove it) - don't call on empty list !
	TYPE RemoveHead();
	TYPE RemoveTail();

	// add before head or after tail
	POSPTR AddHead(ARG_TYPE newElement);
	POSPTR AddTail(ARG_TYPE newElement);

	// add another list of elements before head or after tail
	void AddHead(BaseList* pNewList);
	void AddTail(BaseList* pNewList);

	// remove all elements
	void RemoveAll();

	// iteration
	POSPTR GetHeadPosition() const;
	POSPTR GetTailPosition() const;
	ARG_TYPE GetNext(POSPTR& rPosition); // return *Position++
	const ARG_TYPE GetNext(POSPTR& rPosition) const; // return *Position++
	ARG_TYPE GetPrev(POSPTR& rPosition); // return *Position--
	const ARG_TYPE GetPrev(POSPTR& rPosition) const; // return *Position--

	// getting/modifying an element at a given position
	ARG_TYPE GetAt(POSPTR position);
	const ARG_TYPE GetAt(POSPTR position) const;
	void SetAt(POSPTR pos, ARG_TYPE newElement);
	void RemoveAt(POSPTR position);

	// inserting before or after a given position
	POSPTR InsertBefore(POSPTR position, ARG_TYPE newElement);
	POSPTR InsertAfter(POSPTR position, ARG_TYPE newElement);

	// helper functions (note: O(n) speed)
	POSPTR Find(ARG_TYPE searchValue, POSPTR startAfter = NULL) const;
		// defaults to starting at the HEAD, return NULL if not found
	POSPTR FindIndex(int nIndex) const;
		// get the 'nIndex'th element (may return NULL)

// Implementation
protected:
	BLNode* m_pNodeHead;
	BLNode* m_pNodeTail;
	int m_nCount;
	BLNode* m_pNodeFree;
	struct MemPlex* m_pBlocks;
	int m_nBlockSize;

	BLNode* NewNode(BLNode*, BLNode*);
	void FreeNode(BLNode*);

public:
	~BaseList();
};



/////////////////////////////////////////////////////////////////////////////
// CList<TYPE, ARG_TYPE> inline functions

template<class TYPE, class ARG_TYPE>
inline int BaseList<TYPE, ARG_TYPE>::GetCount() const
	{ return m_nCount; }
template<class TYPE, class ARG_TYPE>
inline int BaseList<TYPE, ARG_TYPE>::GetSize() const
	{ return m_nCount; }
template<class TYPE, class ARG_TYPE>
inline bool BaseList<TYPE, ARG_TYPE>::IsEmpty() const
	{ return m_nCount == 0; }
template<class TYPE, class ARG_TYPE>
inline ARG_TYPE BaseList<TYPE, ARG_TYPE>::GetHead()
	{ ASSERT(m_pNodeHead != NULL);
		return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
inline const ARG_TYPE BaseList<TYPE, ARG_TYPE>::GetHead() const
	{ ASSERT(m_pNodeHead != NULL);
		return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
inline ARG_TYPE BaseList<TYPE, ARG_TYPE>::GetTail()
	{ ASSERT(m_pNodeTail != NULL);
		return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
inline const ARG_TYPE BaseList<TYPE, ARG_TYPE>::GetTail() const
	{ ASSERT(m_pNodeTail != NULL);
		return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
inline POSPTR BaseList<TYPE, ARG_TYPE>::GetHeadPosition() const
	{ return (POSPTR) m_pNodeHead; }
template<class TYPE, class ARG_TYPE>
inline POSPTR BaseList<TYPE, ARG_TYPE>::GetTailPosition() const
	{ return (POSPTR) m_pNodeTail; }
template<class TYPE, class ARG_TYPE>
inline ARG_TYPE BaseList<TYPE, ARG_TYPE>::GetNext(POSPTR& rPosition) // return *Position++
	{ BLNode* pNode = (BLNode*) rPosition;
		rPosition = (POSPTR) pNode->pNext;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline const ARG_TYPE BaseList<TYPE, ARG_TYPE>::GetNext(POSPTR& rPosition) const // return *Position++
	{ BLNode* pNode = (BLNode*) rPosition;
		rPosition = (POSPTR) pNode->pNext;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline ARG_TYPE BaseList<TYPE, ARG_TYPE>::GetPrev(POSPTR& rPosition) // return *Position--
	{ BLNode* pNode = (BLNode*) rPosition;
		
		rPosition = (POSPTR) pNode->pPrev;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline const ARG_TYPE BaseList<TYPE, ARG_TYPE>::GetPrev(POSPTR& rPosition) const // return *Position--
	{ BLNode* pNode = (BLNode*) rPosition;
		
		rPosition = (POSPTR) pNode->pPrev;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline ARG_TYPE BaseList<TYPE, ARG_TYPE>::GetAt(POSPTR position)
	{ BLNode* pNode = (BLNode*) position;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline const ARG_TYPE BaseList<TYPE, ARG_TYPE>::GetAt(POSPTR position) const
	{ BLNode* pNode = (BLNode*) position;
		
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline void BaseList<TYPE, ARG_TYPE>::SetAt(POSPTR pos, ARG_TYPE newElement)
	{ BLNode* pNode = (BLNode*) pos;
		pNode->data = newElement; }

template<class TYPE, class ARG_TYPE>
BaseList<TYPE, ARG_TYPE>::BaseList(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class TYPE, class ARG_TYPE>
void BaseList<TYPE, ARG_TYPE>::RemoveAll()
{
	ASSERT(this);

	// destroy elements
	BLNode* pNode;
	for (pNode = m_pNodeHead; pNode != NULL; pNode = pNode->pNext)
		pNode->data.~TYPE();

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class TYPE, class ARG_TYPE>
BaseList<TYPE, ARG_TYPE>::~BaseList()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

/////////////////////////////////////////////////////////////////////////////
// Node helpers
//
// Implementation note: BLNode's are stored in CPlex blocks and
//  chained together. Free blocks are maintained in a singly linked list
//  using the 'pNext' member of BLNode with 'm_pNodeFree' as the head.
//  Used blocks are maintained in a doubly linked list using both 'pNext'
//  and 'pPrev' as links and 'm_pNodeHead' and 'm_pNodeTail'
//   as the head/tail.
//
// We never free a CPlex block unless the List is destroyed or RemoveAll()
//  is used - so the total number of CPlex blocks may grow large depending
//  on the maximum past size of the list.
//

template<class TYPE, class ARG_TYPE>
typename BaseList<TYPE, ARG_TYPE>::BLNode*
BaseList<TYPE, ARG_TYPE>::NewNode(BLNode* pPrev, BLNode* pNext)
{
	if (m_pNodeFree == NULL)
	{
		// add another block
		MemPlex* pNewBlock = MemPlex::Create(m_pBlocks, m_nBlockSize,
				 sizeof(BLNode));

		// chain them into free list
		BLNode* pNode = (BLNode*) pNewBlock->data();
		// free in reverse order to make it easier to debug
		pNode += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pNode--)
		{
			pNode->pNext = m_pNodeFree;
			m_pNodeFree = pNode;
		}
	}
	ASSERT(m_pNodeFree != NULL);  // we must have something

	BaseList::BLNode* pNode = m_pNodeFree;
	m_pNodeFree = m_pNodeFree->pNext;
	pNode->pPrev = pPrev;
	pNode->pNext = pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow

#pragma push_macro("new")
#undef new
	::new( (void*)( &pNode->data ) ) TYPE;
#pragma pop_macro("new")
	return pNode;
}

template<class TYPE, class ARG_TYPE>
void BaseList<TYPE, ARG_TYPE>::FreeNode(BLNode* pNode)
{
	pNode->data.~TYPE();
	pNode->pNext = m_pNodeFree;
	m_pNodeFree = pNode;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class TYPE, class ARG_TYPE>
POSPTR BaseList<TYPE, ARG_TYPE>::AddHead(ARG_TYPE newElement)
{
	ASSERT(this);

	BLNode* pNewNode = NewNode(NULL, m_pNodeHead);
	pNewNode->data = newElement;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = pNewNode;
	else
		m_pNodeTail = pNewNode;
	m_pNodeHead = pNewNode;
	return (POSPTR) pNewNode;
}

template<class TYPE, class ARG_TYPE>
POSPTR BaseList<TYPE, ARG_TYPE>::AddTail(ARG_TYPE newElement)
{
	ASSERT(this);

	BLNode* pNewNode = NewNode(m_pNodeTail, NULL);
	pNewNode->data = newElement;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = pNewNode;
	else
		m_pNodeHead = pNewNode;
	m_pNodeTail = pNewNode;
	return (POSPTR) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void BaseList<TYPE, ARG_TYPE>::AddHead(BaseList* pNewList)
{
	ASSERT(this);

	ASSERT(pNewList != NULL);
	ASSERT(pNewList);

	// add a list of same elements to head (maintain order)
	POSPTR pos = pNewList->GetTailPosition();
	while (pos != NULL)
		AddHead(pNewList->GetPrev(pos));
}

template<class TYPE, class ARG_TYPE>
void BaseList<TYPE, ARG_TYPE>::AddTail(BaseList* pNewList)
{
	ASSERT(this);
	ASSERT(pNewList != NULL);
	ASSERT(pNewList);

	// add a list of same elements
	POSPTR pos = pNewList->GetHeadPosition();
	while (pos != NULL)
		AddTail(pNewList->GetNext(pos));
}

template<class TYPE, class ARG_TYPE>
TYPE BaseList<TYPE, ARG_TYPE>::RemoveHead()
{
	ASSERT(this);
	ASSERT(m_pNodeHead != NULL);  // don't call on empty list !!!
	ASSERT(AfxIsValidAddress(m_pNodeHead, sizeof(BLNode)));

	BLNode* pOldNode = m_pNodeHead;
	TYPE returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = NULL;
	else
		m_pNodeTail = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
TYPE BaseList<TYPE, ARG_TYPE>::RemoveTail()
{
	ASSERT(this);
	ASSERT(m_pNodeTail != NULL);  // don't call on empty list !!!

	BLNode* pOldNode = m_pNodeTail;
	TYPE returnValue = pOldNode->data;

	m_pNodeTail = pOldNode->pPrev;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = NULL;
	else
		m_pNodeHead = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
POSPTR BaseList<TYPE, ARG_TYPE>::InsertBefore(POSPTR position, ARG_TYPE newElement)
{
	ASSERT(this);

	if (position == NULL)
		return AddHead(newElement); // insert before nothing -> head of the list

	// Insert it before position
	BLNode* pOldNode = (BLNode*) position;
	BLNode* pNewNode = NewNode(pOldNode->pPrev, pOldNode);
	pNewNode->data = newElement;

	if (pOldNode->pPrev != NULL)
	{
		pOldNode->pPrev->pNext = pNewNode;
	}
	else
	{
		ASSERT(pOldNode == m_pNodeHead);
		m_pNodeHead = pNewNode;
	}
	pOldNode->pPrev = pNewNode;
	return (POSPTR) pNewNode;
}

template<class TYPE, class ARG_TYPE>
POSPTR BaseList<TYPE, ARG_TYPE>::InsertAfter(POSPTR position, ARG_TYPE newElement)
{
	ASSERT(this);

	if (position == NULL)
		return AddTail(newElement); // insert after nothing -> tail of the list

	// Insert it before position
	BLNode* pOldNode = (BLNode*) position;
	BLNode* pNewNode = NewNode(pOldNode, pOldNode->pNext);
	pNewNode->data = newElement;

	if (pOldNode->pNext != NULL)
	{
		pOldNode->pNext->pPrev = pNewNode;
	}
	else
	{
		ASSERT(pOldNode == m_pNodeTail);
		m_pNodeTail = pNewNode;
	}
	pOldNode->pNext = pNewNode;
	return (POSPTR) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void BaseList<TYPE, ARG_TYPE>::RemoveAt(POSPTR position)
{
	ASSERT(this);

	BLNode* pOldNode = (BLNode*) position;

	// remove pOldNode from list
	if (pOldNode == m_pNodeHead)
	{
		m_pNodeHead = pOldNode->pNext;
	}
	else
	{
		pOldNode->pPrev->pNext = pOldNode->pNext;
	}
	if (pOldNode == m_pNodeTail)
	{
		m_pNodeTail = pOldNode->pPrev;
	}
	else
	{
		pOldNode->pNext->pPrev = pOldNode->pPrev;
	}
	FreeNode(pOldNode);
}

template<class TYPE, class ARG_TYPE>
POSPTR BaseList<TYPE, ARG_TYPE>::FindIndex(int nIndex) const
{
	ASSERT(this);

	if (nIndex >= m_nCount || nIndex < 0)
		return NULL;  // went too far

	BLNode* pNode = m_pNodeHead;
	while (nIndex--)
	{
		
		pNode = pNode->pNext;
	}
	return (POSPTR) pNode;
}

template<class TYPE, class ARG_TYPE>
POSPTR BaseList<TYPE, ARG_TYPE>::Find(ARG_TYPE searchValue, POSPTR startAfter) const
{
	ASSERT(this);

	BLNode* pNode = (BLNode*) startAfter;
	if (pNode == NULL)
	{
		pNode = m_pNodeHead;  // start at head
	}
	else
	{
		
		pNode = pNode->pNext;  // start after the one specified
	}

	for (; pNode != NULL; pNode = pNode->pNext)
		if (CompareElements<TYPE>(&pNode->data, &searchValue))
			return (POSPTR)pNode;
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class BaseMap 
{
public:
	// MapPair
	struct MapPair
	{
		const KEY key;
		VALUE value;
	protected:
		MapPair( ARG_KEY keyval ) : key( keyval )	{}
	};

protected:
	// Association
	class MapAssoc : public MapPair
	{
		friend class BaseMap<KEY,ARG_KEY,VALUE,ARG_VALUE>;
		MapAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
	public:
		MapAssoc( ARG_KEY key ) : MapPair( key ) {}
	};

public:
// Construction
	/* explicit */ BaseMap(int nBlockSize = 10);

// Attributes
	// number of elements
	int GetCount() const;
	int GetSize() const;
	bool IsEmpty() const;

	// Lookup
	bool Lookup(ARG_KEY key, VALUE& rValue) const;
	const MapPair *PLookup(ARG_KEY key) const;
	MapPair *PLookup(ARG_KEY key);

// Operations
	// Lookup and add if not there
	VALUE& operator[](ARG_KEY key);

	// add a new (key, value) pair
	void SetAt(ARG_KEY key, ARG_VALUE newValue);

	// removing existing (key, ?) pair
	bool RemoveKey(ARG_KEY key);
	void RemoveAll();

	// iterating all (key, value) pairs
	POSPTR GetStartPosition() const;

	const MapPair *PGetFirstAssoc() const;
	MapPair *PGetFirstAssoc();

	void GetNextAssoc(POSPTR& rNextPosition, KEY& rKey, VALUE& rValue) const;

	const MapPair *PGetNextAssoc(const MapPair *pAssocRet) const;
	MapPair *PGetNextAssoc(const MapPair *pAssocRet);

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, bool bAllocNow = TRUE);

// Implementation
protected:
	MapAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	int m_nCount;
	MapAssoc* m_pFreeList;
	struct MemPlex* m_pBlocks;
	int m_nBlockSize;

	MapAssoc* NewAssoc(ARG_KEY key);
	void FreeAssoc(MapAssoc*);
	MapAssoc* GetAssocAt(ARG_KEY, UINT&, UINT&) const;

public:
	~BaseMap();
};

/////////////////////////////////////////////////////////////////////////////
// BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE> inline functions

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline int BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetCount() const
	{ return m_nCount; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline int BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetSize() const
	{ return m_nCount; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline bool BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::IsEmpty() const
	{ return m_nCount == 0; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline void BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::SetAt(ARG_KEY key, ARG_VALUE newValue)
	{ (*this)[key] = newValue; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline POSPTR BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : BEFORE_START_POSPTR; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
const typename BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::MapPair* BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetFirstAssoc() const
{ 
	ASSERT(this);
	if(m_nCount == 0) return NULL;

	AFXASSUME(m_pHashTable != NULL);  // never call on empty map

	MapAssoc* pAssocRet = (MapAssoc*)BEFORE_START_POSPTR;

	// find the first association
	for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
		if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
			break;
	ASSERT(pAssocRet != NULL);  // must find something

	return pAssocRet;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::MapPair* BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetFirstAssoc()
{ 
	ASSERT(this);
	if(m_nCount == 0) return NULL;

	AFXASSUME(m_pHashTable != NULL);  // never call on empty map

	MapAssoc* pAssocRet = (MapAssoc*)BEFORE_START_POSPTR;

	// find the first association
	for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
		if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
			break;
	ASSERT(pAssocRet != NULL);  // must find something

	return pAssocRet;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline UINT BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetHashTableSize() const
	{ return m_nHashTableSize; }

/////////////////////////////////////////////////////////////////////////////
// BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE> out-of-line functions

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::BaseMap(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::InitHashTable(
	UINT nHashSize, bool bAllocNow)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
	ASSERT(this);
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new MapAssoc* [nHashSize];
		ASSERT(m_pHashTable != NULL);
		memset(m_pHashTable, 0, sizeof(MapAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveAll()
{
	ASSERT(this);

	if (m_pHashTable != NULL)
	{
		// destroy elements (values and keys)
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			MapAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			  pAssoc = pAssoc->pNext)
			{
				pAssoc->MapAssoc::~MapAssoc();
				//DestructElements<VALUE>(&pAssoc->value, 1);
				//DestructElements<KEY>((KEY*)&pAssoc->key, 1);
			}
		}
		
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}
	
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::~BaseMap()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::MapAssoc*
BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::NewAssoc(ARG_KEY key)
{
	if (m_pFreeList == NULL)
	{
		// add another block
		MemPlex* newBlock = MemPlex::Create(m_pBlocks, m_nBlockSize, sizeof(BaseMap::MapAssoc));
		// chain them into free list
		BaseMap::MapAssoc* pAssoc = (BaseMap::MapAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	ASSERT(m_pFreeList != NULL);  // we must have something

	BaseMap::MapAssoc* pAssoc = m_pFreeList;

	// zero the memory
	BaseMap::MapAssoc* pTemp = pAssoc->pNext;
	memset( pAssoc, 0, sizeof(BaseMap::MapAssoc) );
	pAssoc->pNext = pTemp;

	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow
#pragma push_macro("new")
#undef new
	::new(pAssoc) BaseMap::MapAssoc(key);
#pragma pop_macro("new")
//	ConstructElements<KEY>(&pAssoc->key, 1);
//	ConstructElements<VALUE>(&pAssoc->value, 1);   // special construct values
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::FreeAssoc(MapAssoc* pAssoc)
{
	pAssoc->MapAssoc::~MapAssoc();
//	DestructElements<VALUE>(&pAssoc->value, 1);
//	DestructElements<KEY>(&pAssoc->key, 1);
	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::MapAssoc*
BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetAssocAt(ARG_KEY key, UINT& nHashBucket, UINT& nHashValue) const
// find association (or return NULL)
{
	nHashValue = DataSet_HashKey<ARG_KEY>(key);
	nHashBucket = nHashValue % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	MapAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHashBucket]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (pAssoc->nHashValue == nHashValue && CompareElements(&pAssoc->key, &key))
			return pAssoc;
	}
	return NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
bool BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Lookup(ARG_KEY key, VALUE& rValue) const
{
	ASSERT(this);

	UINT nHashBucket, nHashValue;
	MapAssoc* pAssoc = GetAssocAt(key, nHashBucket, nHashValue);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
const typename BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::MapPair* BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PLookup(ARG_KEY key) const
{
	ASSERT(this);

	UINT nHashBucket, nHashValue;
	MapAssoc* pAssoc = GetAssocAt(key, nHashBucket, nHashValue);
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::MapPair* BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PLookup(ARG_KEY key)
{
	ASSERT(this);

	UINT nHashBucket, nHashValue;
	MapAssoc* pAssoc = GetAssocAt(key, nHashBucket, nHashValue);
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
VALUE& BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::operator[](ARG_KEY key)
{
	ASSERT(this);

	UINT nHashBucket, nHashValue;
	MapAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHashBucket, nHashValue)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		ASSERT(m_pHashTable);
		// it doesn't exist, add a new Association
		pAssoc = NewAssoc(key);
		pAssoc->nHashValue = nHashValue;
		//'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHashBucket];
		m_pHashTable[nHashBucket] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
bool BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveKey(ARG_KEY key)
// remove key - return TRUE if removed
{
	ASSERT(this);

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	UINT nHashValue;
	MapAssoc** ppAssocPrev;
	nHashValue = DataSet_HashKey<ARG_KEY>(key);
	ppAssocPrev = &m_pHashTable[nHashValue%m_nHashTableSize];

	MapAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if ((pAssoc->nHashValue == nHashValue) && CompareElements(&pAssoc->key, &key))
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetNextAssoc(POSPTR& rNextPosition,
	KEY& rKey, VALUE& rValue) const
{
	ASSERT(this);
	ASSERT(m_pHashTable != NULL);  // never call on empty map

	MapAssoc* pAssocRet = (MapAssoc*)rNextPosition;
	ASSERT(pAssocRet != NULL);

	if (pAssocRet == (MapAssoc*) BEFORE_START_POSPTR)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
		{
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
			{
				break;
			}
		}
		ASSERT(pAssocRet != NULL);  // must find something
	}

	// find next association
	MapAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = (pAssocRet->nHashValue % m_nHashTableSize) + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (POSPTR) pAssocNext;

	// fill in return data
	rKey = pAssocRet->key;
	rValue = pAssocRet->value;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
const typename BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::MapPair*
BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetNextAssoc(const typename BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::MapPair* pPairRet) const
{
	ASSERT(this);

	MapAssoc* pAssocRet = (MapAssoc*)pPairRet;

	ASSERT(m_pHashTable != NULL);  // never call on empty map
	ASSERT(pAssocRet != NULL);
	
	if(m_pHashTable == NULL || pAssocRet == NULL)
		return NULL;
		
	ASSERT(pAssocRet != (MapAssoc*)BEFORE_START_POSPTR);

	// find next association
	MapAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = (pAssocRet->nHashValue % m_nHashTableSize) + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	return pAssocNext;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::MapPair*
BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetNextAssoc(const typename BaseMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::MapPair* pPairRet)
{
	ASSERT(this);

	MapAssoc* pAssocRet = (MapAssoc*)pPairRet;

	ASSERT(m_pHashTable != NULL);  // never call on empty map
	ASSERT(pAssocRet != NULL);
	
	if(m_pHashTable == NULL || pAssocRet == NULL)
		return NULL;
		
	ASSERT(pAssocRet != (MapAssoc*)BEFORE_START_POSPTR);

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(MapAssoc)));
	MapAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = (pAssocRet->nHashValue % m_nHashTableSize) + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	return pAssocNext;
}


/////////////////////////////////////////////////////////////////////////////
// CTypedPtrArray<BASE_CLASS, TYPE>

template<class BASE_CLASS, class TYPE>
class TypedPtrArray : public BASE_CLASS
{
public:
	// Accessing elements
	TYPE GetAt(int nIndex) const
		{ return (TYPE)BASE_CLASS::GetAt(nIndex); }
	TYPE& ElementAt(int nIndex)
		{ return (TYPE&)BASE_CLASS::ElementAt(nIndex); }
	void SetAt(int nIndex, TYPE ptr)
		{ BASE_CLASS::SetAt(nIndex, ptr); }

	// Potentially growing the array
	void SetAtGrow(int nIndex, TYPE newElement)
		{ BASE_CLASS::SetAtGrow(nIndex, newElement); }
	int Add(TYPE newElement)
		{ return BASE_CLASS::Add(newElement); }
	int Append(const TypedPtrArray<BASE_CLASS, TYPE>& src)
		{ return BASE_CLASS::Append(src); }
	void Copy(const TypedPtrArray<BASE_CLASS, TYPE>& src)
		{ BASE_CLASS::Copy(src); }

	// Operations that move elements around
	void InsertAt(int nIndex, TYPE newElement, int nCount = 1)
		{ BASE_CLASS::InsertAt(nIndex, newElement, nCount); }
	void InsertAt(int nStartIndex, TypedPtrArray<BASE_CLASS, TYPE>* pNewArray)
		{ BASE_CLASS::InsertAt(nStartIndex, pNewArray); }

	// overloaded operator helpers
	TYPE operator[](int nIndex) const
		{ return (TYPE)BASE_CLASS::operator[](nIndex); }
	TYPE& operator[](int nIndex)
		{ return (TYPE&)BASE_CLASS::operator[](nIndex); }
};

/////////////////////////////////////////////////////////////////////////////
// CTypedPtrList<BASE_CLASS, TYPE>

template<class BASE_CLASS, class TYPE>
class _TypedPtrBaseList : public BASE_CLASS
{
public:
// Construction
	_TypedPtrBaseList(int nBlockSize = 10)
		: BASE_CLASS(nBlockSize) { }

	// peek at head or tail
	TYPE& GetHead()
		{ return (TYPE&)BASE_CLASS::GetHead(); }
	TYPE GetHead() const
		{ return (TYPE)BASE_CLASS::GetHead(); }
	TYPE& GetTail()
		{ return (TYPE&)BASE_CLASS::GetTail(); }
	TYPE GetTail() const
		{ return (TYPE)BASE_CLASS::GetTail(); }

	// get head or tail (and remove it) - don't call on empty list!
	TYPE RemoveHead()
		{ return (TYPE)BASE_CLASS::RemoveHead(); }
	TYPE RemoveTail()
		{ return (TYPE)BASE_CLASS::RemoveTail(); }

	// iteration
	TYPE& GetNext(POSPTR& rPosition)
		{ return (TYPE&)BASE_CLASS::GetNext(rPosition); }
	TYPE GetNext(POSPTR& rPosition) const
		{ return (TYPE)BASE_CLASS::GetNext(rPosition); }
	TYPE& GetPrev(POSPTR& rPosition)
		{ return (TYPE&)BASE_CLASS::GetPrev(rPosition); }
	TYPE GetPrev(POSPTR& rPosition) const
		{ return (TYPE)BASE_CLASS::GetPrev(rPosition); }

	// getting/modifying an element at a given position
	TYPE& GetAt(POSPTR position)
		{ return (TYPE&)BASE_CLASS::GetAt(position); }
	TYPE GetAt(POSPTR position) const
		{ return (TYPE)BASE_CLASS::GetAt(position); }
	void SetAt(POSPTR pos, TYPE newElement)
		{ BASE_CLASS::SetAt(pos, newElement); }

	// inserting before or after a given position
	POSPTR InsertBefore(POSPTR position, TYPE newElement)
		{ return BASE_CLASS::InsertBefore(position, newElement); }
	POSPTR InsertAfter(POSPTR position, TYPE newElement)
		{ return BASE_CLASS::InsertAfter(position, newElement); }

	// transfer before or after a given position
	// Transfer semantics ensure no leakage by deleting the element in the case of an exception
	POSPTR TransferInsertBefore(POSPTR position, TYPE newElement)
	{
		try
		{
			return BASE_CLASS::InsertBefore(position, newElement); 
		}
		catch(...)
		{
			delete newElement;
			throw;
		}
	}

	POSPTR TransferInsertAfter(POSPTR position, TYPE newElement)
	{
		try
		{
			return BASE_CLASS::InsertAfter(position, newElement); 
		}
		catch(...)
		{
			delete newElement;
			throw;
		}
	}
};

template<class BASE_CLASS, class TYPE>
struct TypedPtrBaseList : public _TypedPtrBaseList<BASE_CLASS, TYPE>
{
	typedef TYPE _TItem;
public:
// Construction
	TypedPtrBaseList(int nBlockSize = 10)
		: _TypedPtrBaseList<BASE_CLASS, TYPE>(nBlockSize) { }

	// add before head or after tail
	POSPTR AddHead(TYPE newElement)
		{ return BASE_CLASS::AddHead(newElement); }
	POSPTR AddTail(TYPE newElement)
		{ return BASE_CLASS::AddTail(newElement); }

	// transfer add before head or tail
	POSPTR TransferAddHead(TYPE newElement)
	{ 
		try
		{
			return BASE_CLASS::AddHead(newElement); 
		}
		catch(...)
		{
			delete newElement;
			throw;
		}
	}
	POSPTR TransferAddTail(TYPE newElement)
	{
		try
		{
			return BASE_CLASS::AddTail(newElement); 
		}
		catch(...)
		{
			delete newElement;
			throw;
		}
	}

	// add another list of elements before head or after tail
	void AddHead(TypedPtrBaseList<BASE_CLASS, TYPE>* pNewList)
		{ BASE_CLASS::AddHead(pNewList); }
	void AddTail(TypedPtrBaseList<BASE_CLASS, TYPE>* pNewList)
		{ BASE_CLASS::AddTail(pNewList); }
};

/////////////////////////////////////////////////////////////////////////////
// CTypedPtrMap<BASE_CLASS, KEY, VALUE>
#if 0
template<class BASE_CLASS, class KEY, class VALUE>
class CTypedPtrMap : public BASE_CLASS
{
public:

// Construction
	CTypedPtrMap(int nBlockSize = 10)
		: BASE_CLASS(nBlockSize) { }

	// Lookup
	bool Lookup(typename BASE_CLASS::BASE_ARG_KEY key, VALUE& rValue) const
		{ return BASE_CLASS::Lookup(key, (BASE_CLASS::BASE_VALUE&)rValue); }

	// Lookup and add if not there
	VALUE& operator[](typename BASE_CLASS::BASE_ARG_KEY key)
		{ return (VALUE&)BASE_CLASS::operator[](key); }

	// add a new key (key, value) pair
	void SetAt(KEY key, VALUE newValue)
		{ BASE_CLASS::SetAt(key, newValue); }

	// removing existing (key, ?) pair
	bool RemoveKey(KEY key)
		{ return BASE_CLASS::RemoveKey(key); }

	// iteration
	void GetNextAssoc(POSITION& rPosition, KEY& rKey, VALUE& rValue) const
		{ BASE_CLASS::GetNextAssoc(rPosition, (BASE_CLASS::BASE_KEY&)rKey,
			(BASE_CLASS::BASE_VALUE&)rValue); }
};

#endif

template<typename TI>
struct TypedPtrBaseArray:public BaseArray<TI*>
{
	typedef TI _TItem;
};


template<typename TPARENT,typename TCHILD>
struct IDPtr
{
	WORD idChild;
	WORD idParent;
	IDPtr(DWORD idPtr=-1)
	{
		*LPDWORD(this)=idPtr;
	}
	IDPtr(void* p)
	{
		long pl=(long)p;
		*LPDWORD(this)=(DWORD)pl;
	}
	virtual TPARENT* GetParent(){return 0;}
	virtual TCHILD* GetChild(){return 0;}

	operator DWORD(){return *LPDWORD(this);}
	operator void*(){return (void*)(*LPDWORD(this));}
	TCHILD* operator->(){return GetChild();}

};

template<typename T>
struct UniAutoReleasePtr
{
	T* _ptr;
	UniAutoReleasePtr(T* p=0)
	{
		_ptr=p;
	}
	UniAutoReleasePtr& operator=(UniAutoReleasePtr& up)
	{
		_ptr=up._ptr;up._ptr=0;
		return *this;
	}
	UniAutoReleasePtr& operator=(T* p)
	{
		_ptr=p;
		return *this;
	}
	~UniAutoReleasePtr()
	{
		//_ptr->Release();
		delete _ptr;
		_ptr=0;
	}
	T* operator->(){return _ptr;}
	operator T*(){return _ptr;}
};

template<typename T>
struct  AutoReleaseArray:public BaseArray<UniAutoReleasePtr<T>,T*>{};

#endif // __IBASECLASS__H__
