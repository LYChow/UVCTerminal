#ifndef __PCHPUBDEF__H__
#define __PCHPUBDEF__H__

typedef int _BOOL;
typedef class std::string String;

#define INT_P(p) (*((int*)p))
#define PINT_P(p) ((int*)p)
#define _MMBOFFSET(mmb) (LPBYTE(&mmb)-LPBYTE(this))
#define _PTRMMBOFFSET(p,mmb) (LPBYTE(&p->mmb)-LPBYTE(p))

#define ZeroVar(var) ZeroMemory(&(var),sizeof(var))

#define ZeroPtrFrom(p,mmb) ZeroMemory(&p->mmb,sizeof(*p)-_PTRMMBOFFSET(p,mmb));
#define ZeroThisFrom(mmb) ZeroPtrFrom(this,mmb)


#define BitMask(bit) (1<<bit)
#define BitSet(val,bit) val|=BitMask(bit)
#define BitClear(val,bit) val&=~BitMask(bit)
#define IsBitSet(val,bit) ((val&BitMask(bit))!=0)


#define _BEGIN_NAMESPACE(ns) namespace ns{

#define _BEGIN_NAMESPACE_SWApi _BEGIN_NAMESPACE(SWApi)

#define _END_NAMESPACE };




#endif
