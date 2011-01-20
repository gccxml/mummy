#ifndef _ABSTRACTVEHICLEEVENT_H
#define _ABSTRACTVEHICLEEVENT_H

#include "VehiclesConfig.h"

#include "SenderDataEvent.h"

template <typename DataT>
class AbstractVehicleEvent : public SenderDataEvent<AbstractVehicle*, DataT>
{
};

#endif // _ABSTRACTVEHICLEEVENT_H
