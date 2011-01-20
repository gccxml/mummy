#ifndef _SENDERDATAEVENT_H
#define _SENDERDATAEVENT_H

#ifdef _WIN32
#define SDE_CC __stdcall
#else
#define SDE_CC
#endif

//----------------------------------------------------------------------------
template <typename SenderT, typename DataT>
class EventHandler
{
public:
	virtual void notify(SenderT sender, DataT data) = 0;

	virtual ~EventHandler()
	  {
	  }
};

//----------------------------------------------------------------------------
template <typename ListenerT, typename SenderT, typename DataT>
class InstanceEventHandler : public EventHandler<SenderT, DataT>
{
	typedef void (ListenerT::*PtrMember)(SenderT, DataT);
	ListenerT* m_object;
	PtrMember m_member;

public:
	InstanceEventHandler(ListenerT* object, PtrMember member)
		: m_object(object), m_member(member)
	{
  }

	void notify(SenderT sender, DataT data)
	{
		(m_object->*m_member)(sender, data);
	}
};

//----------------------------------------------------------------------------
template <typename SenderT, typename DataT>
class CallbackEventHandler : public EventHandler<SenderT, DataT>
{
	typedef void (SDE_CC *CallbackT)(SenderT, DataT);
	CallbackT Callback;

public:
	CallbackEventHandler(CallbackT callback)
		: Callback(callback)
	{
  }

	void notify(SenderT sender, DataT data)
	{
		(*Callback)(sender, data);
	}
};

//----------------------------------------------------------------------------
typedef unsigned int EventHandlerId;
#include <map>

class AbstractVehicle;

//----------------------------------------------------------------------------
template <typename SenderT=AbstractVehicle*, typename DataT=unsigned int>
class SenderDataEvent
{
	typedef std::map<EventHandlerId, EventHandler<SenderT, DataT> *> HandlersMap;
	HandlersMap Handlers;
	EventHandlerId NextHandlerId;

public:
	SenderDataEvent()
		: NextHandlerId(1) {}

	template <typename ListenerT>
	EventHandlerId attach(ListenerT* object, void (ListenerT::*member)(SenderT, DataT))
	{
		Handlers[NextHandlerId] = new InstanceEventHandler<ListenerT, SenderT, DataT>(object, member);
		NextHandlerId++;
		return NextHandlerId-1;
	}

	EventHandlerId attach(void (SDE_CC *callback)(SenderT, DataT))
	{
		Handlers[NextHandlerId] = new CallbackEventHandler<SenderT, DataT>(callback);
		NextHandlerId++;
		return NextHandlerId-1;
	}

  bool detach(EventHandlerId id)
	{
		typename HandlersMap::iterator it = Handlers.find(id);

		if (it == Handlers.end())
			return false;

		delete it->second;
		Handlers.erase(it);
		return true;
	}

	void Invoke(SenderT sender, DataT data)
	{
		typename HandlersMap::iterator it = Handlers.begin();
		for(; it != Handlers.end(); it++)
		{
			it->second->notify(sender, data);
		}
	}
};

#endif
