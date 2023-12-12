// Author: Egor A. Pristavka

#pragma once

/** Interface for any class that will use delegates. */
class ICallbackInterface {
protected:
	virtual ~ICallbackInterface() = default;

	/** Should be used to bind delegates which exist during lifetime of the object, save delegate handles. Should be called when the object is initialized. */
	virtual void RegisterCallbacks() = 0;

	/** Should be used to unbind all the delegates which exist at the object and which can be resolved after object has been deleted. */
	virtual void UnregisterCallbacks() = 0;
};
