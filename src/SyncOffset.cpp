#include "SyncOffset.hpp"

SyncOffset::SyncOffset(void)
{
	type = SyncOffsetType::None;
	linearDelay.tv_sec = 0;
	linearDelay.tv_usec = 0;
}

SyncOffset::SyncOffset(const SyncOffset & so)
{
	*this = so;
}

SyncOffset &    SyncOffset::operator=(SyncOffset const & so)
{
	if (this != &so)
	{
		type = so.type;
		linearDelay = so.linearDelay;
		order = so.order;
		customDelayCallback = so.customDelayCallback;
	}
	return *this;
}	

SyncOffset		SyncOffset::CreateLinearSyncOffset(const int delaySecs, const int delayMillis, const SyncOffsetOrder order)
{
	SyncOffset	o;

	o.type = SyncOffsetType::Linear;
	o.order = order;
	o.linearDelay.tv_sec = delaySecs;
	o.linearDelay.tv_usec = delayMillis * 1000;
	return o;
}

SyncOffset		SyncOffset::CreateCustomSyncOffset(CustomSyncOffsetCallback callback, const SyncOffsetOrder order)
{
	SyncOffset	o;

	o.type = SyncOffsetType::Custom;
	o.order = order;
	gettimeofday(&o.linearDelay, NULL);
	o.customDelayCallback = callback;
	return o;
}

SyncOffset		SyncOffset::CreateNoneSyncOffset(void)
{
	SyncOffset	o;

	o.type = SyncOffsetType::None;
	return o;
}

SyncOffset::~SyncOffset(void) {}
