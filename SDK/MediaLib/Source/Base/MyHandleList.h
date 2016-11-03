// MyHandleList.h: interface for the MyHandleList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYHANDLELIST_H__A6CA956D_F43F_4B12_B9E2_454ED4485F01__INCLUDED_)
#define AFX_MYHANDLELIST_H__A6CA956D_F43F_4B12_B9E2_454ED4485F01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sync.h"
#include "SwBaseWin.h"

template <class Type>
class MyHandleList
{
public:
	bool AddHeadHandle(Type tHandle);
	bool InsertBefore(Type tHandlePos, Type tHandleNew);
	bool InsertAfter(Type tHandlePos, Type tHandleNew);
	bool AddTailHandle(Type tHandle);
	bool RemoveHandle(Type tHandle);
	bool DeleteHandle(Type tHandle);
	Type GetHeadHandle();
	Type GetNextHandle(Type tHandle);
	Type GetTailHandle();
	Type GetPrevHandle(Type tHandle);
	Type PickHeadHandle();
	Type PickTailHandle();
	Type GetHandleAt(int index);
	int GetIndex(Type tHandle);
	int GetCount();
	bool AsureHandleValid(Type tHandle);
	void ClearHandleList();
	void DeleteHandleList();

public:
	MyHandleList(bool bNeedFreeMemory=true, DWORD dwGhostLife=0/*ms*/);
	virtual ~MyHandleList();

protected:
    struct MHLNode {
        MHLNode* pNext;
        MHLNode* pPrev;
        void* data;
        bool forsaked;
        DWORD ticForsaked;
        MHLNode() {
            pNext = NULL;
            pPrev = NULL;
            data = NULL;
            forsaked = false;
            DWORD ticForsaked = 0;
        }
    };
    MHLNode* m_pHead;
    MHLNode* m_pTail;
    int m_nCount;
    int m_nForsakedCount;
    DWORD m_dwGhostLife;
    DWORD m_dwTicLastClearOutdatedForsakedNodes;
    MHLNode* NewNode(Type data, MHLNode* pPrev, MHLNode* pNext) {
        if (!data) {
            return NULL;
        }
        MHLNode* pNode = new MHLNode;
        if (!pNode)
            return NULL;
        pNode->data = (void *)data;
        pNode->pNext = pNext;
        pNode->pPrev = pPrev;
        if (pPrev)
            pPrev->pNext = pNode;
        else
            m_pHead = pNode;
        if (pNext)
            pNext->pPrev = pNode;
        else
            m_pTail = pNode;
        m_nCount++;
        return pNode;
    }
    bool FreeNode(MHLNode* pNode, bool bForsakeOnly=true) {
        if (!pNode)
            return false;
        if (m_dwGhostLife == 0) {
            bForsakeOnly = false;
        }
        if (bForsakeOnly) {
            if (!pNode->forsaked) {
                pNode->forsaked = true;
                pNode->ticForsaked = GetTickCount();
                m_nCount--;
                m_nForsakedCount++;
            }
            return true;
        }
        if (pNode->pPrev)
            pNode->pPrev->pNext = pNode->pNext;
        else
            m_pHead = pNode->pNext;
        if (pNode->pNext)
            pNode->pNext->pPrev = pNode->pPrev;
        else
            m_pTail = pNode->pPrev;
        if (!pNode->forsaked) {
            m_nCount--;
        }
        else {
            m_nForsakedCount--;
        }
        delete pNode;
        return true;
    }
    void ClearOutdatedForsakedNodes() {
        {
            CriticalSectionLock l(m_cs);

            DWORD tic = GetTickCount();
            if (tic-m_dwTicLastClearOutdatedForsakedNodes < 1000) {
                return;
            }
            m_dwTicLastClearOutdatedForsakedNodes = tic;

            MHLNode* pNode = m_pHead;
            for (; (m_nForsakedCount>0&&pNode); pNode=pNode->pNext) {
                MHLNode* pNodeDeal = pNode;
                if (pNodeDeal->forsaked && tic-pNodeDeal->ticForsaked>m_dwGhostLife) {
                    FreeNode(pNodeDeal, false);
                }
            }
        }
    }
	CriticalSection m_cs;

public:
	bool m_bNeedFreeMemory;
};

template <class Type>
MyHandleList<Type>::MyHandleList(bool bNeedFreeMemory/*=true*/, DWORD dwGhostLife/*=5000*//*ms*/)
{
	{
		CriticalSectionLock l(m_cs);
        m_pHead = NULL;
        m_pTail = NULL;
        m_nCount = 0;
        m_nForsakedCount = 0;
        m_dwGhostLife = dwGhostLife;
        m_dwTicLastClearOutdatedForsakedNodes = 0;
	}
	m_bNeedFreeMemory = bNeedFreeMemory;
}

template <class Type>
MyHandleList<Type>::~MyHandleList()
{
    m_dwGhostLife = 0; // to delete node immediately

	if (m_bNeedFreeMemory)
		DeleteHandleList();
	else
		ClearHandleList();
}

template <class Type>
bool MyHandleList<Type>::AddHeadHandle(Type tHandle)
{
	{
		CriticalSectionLock l(m_cs);

        ClearOutdatedForsakedNodes();

        if (!NewNode(tHandle, NULL, m_pHead))
            return false;
	}
	return true;
}

template <class Type>
bool MyHandleList<Type>::InsertBefore(Type tHandlePos, Type tHandleNew)
{
	if (!tHandlePos)
		return AddHeadHandle(tHandleNew);

    ClearOutdatedForsakedNodes();

	bool br = false;

	{
		CriticalSectionLock l(m_cs);

        MHLNode* pNode = m_pHead;
        for (; pNode; pNode=pNode->pNext) {
            if ((Type)pNode->data == tHandlePos) {
                if (NewNode(tHandleNew, pNode->pPrev, pNode)) {
                    br = true;
                }
                break;
            }
        }
	}

	return br;
}

template <class Type>
bool MyHandleList<Type>::InsertAfter(Type tHandlePos, Type tHandleNew)
{
	if (!tHandlePos)
		return AddTailHandle(tHandleNew);

    ClearOutdatedForsakedNodes();

	bool br = false;

	{
		CriticalSectionLock l(m_cs);

        MHLNode* pNode = m_pHead;
        for (; pNode; pNode=pNode->pNext) {
            if ((Type)pNode->data == tHandlePos) {
                if (!NewNode(tHandleNew, pNode, pNode->pNext))
                    break;
                br = true;
                break;
            }
        }
	}

	return br;
}

template <class Type>
bool MyHandleList<Type>::AddTailHandle(Type tHandle)
{
    ClearOutdatedForsakedNodes();

    {
		CriticalSectionLock l(m_cs);

        if (!NewNode(tHandle, m_pTail, NULL))
            return false;
	}

	return true;
}

template <class Type>
bool MyHandleList<Type>::RemoveHandle(Type tHandle)
{
	bool br = false;

    ClearOutdatedForsakedNodes();

	{
		CriticalSectionLock l(m_cs);

        MHLNode* pNode = m_pHead;
        for (; pNode; pNode=pNode->pNext) {
            if ((Type)pNode->data == tHandle) {
                if (!FreeNode(pNode)) {
                    break;
                }
                br = true;
                break;
            }
        }
	}

	return br;
}

template <class Type>
bool MyHandleList<Type>::DeleteHandle(Type tHandle)
{
	Type pHandle = tHandle;

	bool br = RemoveHandle(tHandle);
	if (br)
	{
        if (tHandle) {
            delete pHandle;
        }
	}

	return br;
}

template <class Type>
Type MyHandleList<Type>::GetHeadHandle()
{
	Type tr = NULL;

    ClearOutdatedForsakedNodes();

	{
		CriticalSectionLock l(m_cs);

        MHLNode* pNode = m_pHead;
        for (; pNode; pNode=pNode->pNext) {
            if (!pNode->forsaked) {
                tr = (Type)pNode->data;
                break;
            }
        }
    }

	return tr;
}

template <class Type>
Type MyHandleList<Type>::GetNextHandle(Type tHandle)
{
	Type tr = NULL;

	{
		CriticalSectionLock l(m_cs);

        MHLNode* pNode = m_pHead;
        for (; pNode; pNode=pNode->pNext) {
            if ((Type)pNode->data == tHandle) {
                for (pNode=pNode->pNext; pNode; pNode=pNode->pNext) {
                    if (!pNode->forsaked) {
                        tr = (Type)pNode->data;
                        break;
                    }
                }
                break;
            }
        }
	}

    ClearOutdatedForsakedNodes();

	return tr;
}

template <class Type>
Type MyHandleList<Type>::GetTailHandle()
{
	Type tr = NULL;

    ClearOutdatedForsakedNodes();

	{
		CriticalSectionLock l(m_cs);

        MHLNode* pNode = m_pTail;
        for (; pNode; pNode=pNode->pPrev) {
            if (!pNode->forsaked) {
                tr = (Type)pNode->data;
                break;
            }
        }
    }

	return tr;
}

template <class Type>
Type MyHandleList<Type>::GetPrevHandle(Type tHandle)
{
	Type tr = NULL;

	{
		CriticalSectionLock l(m_cs);

        MHLNode* pNode = m_pHead;
        for (; pNode; pNode=pNode->pNext) {
            if ((Type)pNode->data == tHandle) {
                for (pNode=pNode->pPrev; pNode; pNode=pNode->pPrev) {
                    if (!pNode->forsaked) {
                        tr = (Type)pNode->data;
                        break;
                    }
                }
                break;
            }
        }
	}

    ClearOutdatedForsakedNodes();

	return tr;
}

template <class Type>
Type MyHandleList<Type>::PickHeadHandle()
{
	Type tr = NULL;

	{
		CriticalSectionLock l(m_cs);

        tr = GetHeadHandle();
        RemoveHandle(tr);
	}

	return tr;
}

template <class Type>
Type MyHandleList<Type>::PickTailHandle()
{
	Type tr = NULL;

	{
		CriticalSectionLock l(m_cs);

        tr = GetTailHandle();
        RemoveHandle(tr);
	}

	return tr;
}

template <class Type>
Type MyHandleList<Type>::GetHandleAt(int index)
{
	Type tr = NULL;

    ClearOutdatedForsakedNodes();

	{
		CriticalSectionLock l(m_cs);

		if (index < 0 || index >= m_nCount) {
			tr = NULL;
		}
        else {
            MHLNode* pNode = m_pHead;
            while (pNode && pNode->forsaked) {
                pNode = pNode->pNext;
            }
            for (int i=0; i<index; i++) {
                pNode = pNode->pNext;
                while (pNode && pNode->forsaked) {
                    pNode = pNode->pNext;
                }
                if (!pNode) {
                    break;
                }
            }
            if (pNode) {
                tr = (Type)pNode->data;
            }
        }
	}

	return tr;
}

template <class Type>
int MyHandleList<Type>::GetIndex(Type tHandle)
{
	int index = -1;
	int cnt = 0;

    ClearOutdatedForsakedNodes();

	{
		CriticalSectionLock l(m_cs);

        for (MHLNode* pNode = m_pHead; pNode; pNode=pNode->pNext) {
            if (!pNode->forsaked) {
                if ((Type)pNode->data == tHandle) {
                    index = cnt;
                    break;
                }
                cnt++;
            }
        }
	}

	return index;
}

template <class Type>
int MyHandleList<Type>::GetCount()
{
	int ir = 0;

    ClearOutdatedForsakedNodes();

	{
		CriticalSectionLock l(m_cs);

        ir = m_nCount;
    }

	return ir;
}


template <class Type>
bool MyHandleList<Type>::AsureHandleValid(Type tHandle)
{
	bool br = false;

    ClearOutdatedForsakedNodes();

	{
		CriticalSectionLock l(m_cs);

        for (MHLNode* pNode = m_pHead; pNode; pNode=pNode->pNext) {
            if (!pNode->forsaked && (Type)pNode->data==tHandle) {
                br = true;
                break;
            }
        }
	}

	return br;
}

template <class Type>
void MyHandleList<Type>::ClearHandleList()
{
	CriticalSectionLock l(m_cs);

    MHLNode* pNode = m_pHead, *pNodeDeal = NULL;
    while (pNode) {
        pNodeDeal = pNode;
        pNode = pNode->pNext;
        FreeNode(pNodeDeal);
    }
}

template <class Type>
void MyHandleList<Type>::DeleteHandleList()
{
	Type pHandle = (Type)PickHeadHandle();
    while (pHandle) {
        delete pHandle;
		pHandle = (Type)PickHeadHandle();
    }
}

#endif // !defined(AFX_MYHANDLELIST_H__A6CA956D_F43F_4B12_B9E2_454ED4485F01__INCLUDED_)
