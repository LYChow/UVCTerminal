//
//  DiapatchWrapper.h
//  IOSTest
//
//  Created by zhangyujun on 16/1/2.
//  Copyright © 2016年 sinowave. All rights reserved.
//

#ifndef DiapatchWrapper_h
#define DiapatchWrapper_h
#include <dispatch/dispatch.h>
//#include "debug.h"

struct DQ
{
	dispatch_queue_t _q;
	DQ(dispatch_queue_t q=0)
	{
		if(q==0) FromMain();
		else _q=q;
		
	}
	DQ(long Id,unsigned long Flags=0)
	{
		FromGlobal(Id,Flags);
	}
	DQ(const  char* label,dispatch_queue_attr_t attr=0)
	{
		Create(label,attr);
	}
	~DQ()
	{
		_q=0;
	}
	operator dispatch_queue_t(){return _q;}
	dispatch_queue_t Deatch()
	{
		dispatch_queue_t q=_q;
		_q=0;
		return q;
	}
	
	DQ& operator=(dispatch_queue_t q){_q=q;return *this;}
	
	DQ& FromCurrent()
	{
		_q=dispatch_get_current_queue();
		return *this;
	}
	DQ& FromGlobal(long Id,unsigned long Flags)
	{
		_q=dispatch_get_global_queue(Id,Flags);
		return *this;
		
	}
	DQ& FromMain()
	{
		_q=dispatch_get_main_queue();
		return *this;
	}
	
	dispatch_queue_t Create(const char* label,dispatch_queue_attr_t attr)
	{
		return _q=dispatch_queue_create(label,attr);
		
	}
	const char* GetLable()
	{
		return dispatch_queue_get_label(_q);
	}
	
	void AsyncCall( dispatch_block_t block)
	{
		dispatch_async(_q,block);
	}
	void AsyncCallF( void* context,dispatch_function_t block)
	{
		dispatch_async_f(_q,context,block);
	}
	
	void SyncCall( dispatch_block_t block)
	{
		dispatch_async(_q,block);
	}
	void SyncCallF( void* context,dispatch_function_t block)
	{
		dispatch_async_f(_q,context,block);
	}
	
	void Apply(size_t iterations, void (^block)(size_t))
	{
		dispatch_apply(iterations,_q,block);
	}
	
	void ApplyF(size_t iterations,void* context, void (*block)(void*,size_t))
	{
		dispatch_apply_f(iterations,_q,context,block);
	}
	
	
	
	
	//	dispatch_queue_attr_t
	//	dispatch_queue_attr_make_with_qos_class(dispatch_queue_attr_t attr,
	//											dispatch_qos_class_t qos_class, int relative_priority);
	
	
	dispatch_qos_class_t GetQosClass(int* relative_priority_ptr)
	{
		return dispatch_queue_get_qos_class(_q,relative_priority_ptr);
	}
	void SetAsTarget(dispatch_object_t object)
	{
		dispatch_set_target_queue(object,_q);
	}
	
	void After(dispatch_time_t when,dispatch_block_t block)
	{
		dispatch_after(when,_q,block);
	}
	
	void AfterF(dispatch_time_t when,void *context,dispatch_function_t work)
	{
		dispatch_after_f(when,_q,context,work);
	}
	
	void BarrierAsync(dispatch_block_t block)
	{
		dispatch_barrier_async(_q,block);
	}
	void BarrierAsyncF(void* context,dispatch_function_t block)
	{
		dispatch_barrier_async_f(_q,context,block);
	}
	
	void BarrierSync(dispatch_block_t block)
	{
		dispatch_barrier_sync(_q,block);
	}
	void BarrierSyncF(void* context,dispatch_function_t block)
	{
		dispatch_barrier_sync_f(_q,context,block);
	}
	
	void SetSpecific(const void *key,void *context, dispatch_function_t destructor)
	{
		dispatch_queue_set_specific(_q,key,context,destructor);
	}
	
	void* GetSpecific(const void *key)
	{
		return dispatch_queue_get_specific(_q,key);
	}
	
	
	static void* _GetSpecific(const void* key)
	{
		return dispatch_get_specific(key);
	}
	static void _RunMain()
	{
		dispatch_main();
	};
	
	static DQ Main()
	{
		return dispatch_get_main_queue();
	}
};

struct DS
{
	dispatch_source_t _ds;
	DS(dispatch_queue_t queue,void* context=0,dispatch_source_type_t type=DISPATCH_SOURCE_TYPE_TIMER,uintptr_t handle=0,unsigned long mask=0)
	{
		printf("--DS create\n");
		_ds=dispatch_source_create(type,handle,mask,queue);
		dispatch_set_context(_ds,context);
	}
	~DS()
	{
		printf("--DS release\n");
		//dispatch_release(_ds);
		_ds=0;
	}
	bool Create(dispatch_queue_t queue,void* context=0,
				dispatch_source_type_t type=DISPATCH_SOURCE_TYPE_TIMER,
				uintptr_t handle=0,unsigned long mask=0)
	{
		printf("--DS create\n");
		_ds=dispatch_source_create(type,handle,mask,queue);
		dispatch_set_context(_ds,context);
		return _ds!=0;
	}
	void SetEventHandler(dispatch_block_t handler)
	{
		dispatch_source_set_event_handler(_ds,handler);
	}
	void SetEventHandlerF(dispatch_function_t handler)
	{
		dispatch_source_set_event_handler_f(_ds,handler);
	}
	void SetCancleHandler(dispatch_block_t handler)
	{
		dispatch_source_set_cancel_handler(_ds,handler);
	}
	void SetCancelHandlerF(dispatch_function_t handler)
	{
		dispatch_source_set_cancel_handler_f(_ds,handler);
	}
	void Resume()
	{
		dispatch_resume(_ds);
	}
	void Cancel()
	{
		dispatch_source_cancel(_ds);
	}
	long IsCanceled()
	{
		return dispatch_source_testcancel(_ds);
	}
	void SetTimer( uint64_t interval,dispatch_time_t start=DISPATCH_TIME_NOW,uint64_t leeway=0)
	{
		//second*=NSEC_PER_SEC;
		dispatch_source_set_timer(_ds,start,interval,leeway);
	}
	void Release()
	{
		_ds=0;
	}
	
	
	
};



#endif /* DiapatchWrapper_h */
