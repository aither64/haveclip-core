#ifndef APPNAPPREVENTINGACTIVITY_H
#define APPNAPPREVENTINGACTIVITY_H

#ifndef __OBJC__ //or `_OBJC_NSOBJECT_H_`
//#include <QtGlobal>

#ifdef Q_FORWARD_DECLARE_OBJC_CLASS
Q_FORWARD_DECLARE_OBJC_CLASS(NSObject); // not to conflict with declaration in QDEBUG_H
#else
class NSObject; //or `typedef struct objc_object NSObject;` - see Q_FORWARD_DECLARE_OBJC_CLASS
#endif

#endif


class AppNapPreventingActivity
{
	NSObject *activity;

	//NOTE: if access outside of ClipboardManager::clipboardTracking() is needed then:
	//       - remove `friend ...`
	//       - uncomment `public:`
	//       - pass `reason` string to `begin(const QString &reason){... reason:reason.toNSString() ...}`
	//       - add additional checks (activity != nil)
	//       - call end() from destructor
	friend class ClipboardManager;
//public:
	explicit AppNapPreventingActivity() = default;
	//Q_DISABLE_COPY(AppNapPreventingActivity); // delete or not delete...
	//void *operator new(std::size_t) = delete; // delete or not delete...

	void begin();
	void end();
};

#endif // APPNAPPREVENTINGACTIVITY_H
