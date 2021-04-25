//// Disabling App Nap stuff
//
// Didn't work (in Terminal):
//  defaults write cz.havefun.HaveClip NSAppSleepDisabled -bool YES
//  defaults write cz.havefun.HaveClip LSAppNapIsDisabled -bool YES
//
//
// Links (App Nap):
//
// - Pure solution in C (don't need Objective-C compiler)
//   https://stackoverflow.com/questions/22784886/what-can-make-nanosleep-drift-with-exactly-10-sec-on-mac-os-x-10-9/22785076#22785076
//
// - Keep-alive timers on sockets that weren't firing every 5 seconds with App Nap
//   https://github.com/robbiehanson/CocoaAsyncSocket/issues/276#issuecomment-59842364
//
// - `beginActivityWithOptions:reason:` documentation
//   https://developer.apple.com/documentation/foundation/nsprocessinfo/1415995-beginactivitywithoptions
//
// - What App Nap does & how it works
//   https://developer.apple.com/library/archive/documentation/Performance/Conceptual/power_efficiency_guidelines_osx/AppNap.html
//
// - QTimer and App Nap (add `LIBS += -framework Foundation` and `CONFIG += objective_c` to ".pro" file)
//   https://forum.qt.io/post/213085
//   - solution (AppNapSuspender class) doesn't work since it doesn't use ARC nor manual retain/release -> activity is "autorelease"ed after 3-4 minutes
//     ARC solution: https://stackoverflow.com/questions/19847293/disable-app-nap-in-macos-10-9-mavericks-application  #20100906
//
// - It would be nice if Qt Support disabling App Nap (Utility class to wrap AppNap activities)
//   https://bugreports.qt.io/browse/QTBUG-43861?focusedCommentId=296871#comment-296871
//   https://codereview.qt-project.org/c/qt/qtmacextras/+/106306
//   (see `ActivityOption` in "qmacactivity.h" and `NSActivityOptions` enum in "qmacactivity.mm")
//   - Automatic & Sudden Termination
//     https://developer.apple.com/library/archive/documentation/General/Conceptual/MOSXAppProgrammingGuide/CoreAppDesign/CoreAppDesign.html#//apple_ref/doc/uid/TP40010543-CH3-SW22
//
// - About plist key (Terminal commands above)
//   https://github.com/performancecopilot/pcp/issues/20
//   https://github.com/bitcoin/bitcoin/issues/11896
//   https://git.gnu.io/gnu/emacs/commit/04a7977f700fc46cf29d5a41bc7dcffef71044c6
//
// - `LSUIElement` key of Info.plist - disables App Nap by default (this behavior may change in the future)
//   https://stackoverflow.com/questions/19577541/disabling-timer-coalescing-in-osx-for-a-given-process/19578413#19578413
//   - to set dynamically from code, not from Info.plist
//     https://forum.qt.io/post/252840
//     https://developer.apple.com/documentation/appkit/nsapplicationactivationpolicy/nsapplicationactivationpolicyaccessory?language=objc
//   - `dispatch_source*`: *_create, *_set_event_handler, *_set_timer, dispatch_resume
//     https://stackoverflow.com/a/19588636
//   - WWDC 2013: Improving Power Efficiency with App Nap
//     https://developer.apple.com/videos/play/wwdc2013/209/
//   - use `NSSupportsAppNap` key to opt-in to App Nap
//     https://forum.xojo.com/t/mavericks-is-putting-my-app-to-sleep/15643/20
//
// - Some more examples
//   - manual retain/release with ARC, `id` vs `id<NSObject>` vs `NSObject*`
//     https://github.com/krab/bitcoin/blob/1e0f3c44992fb82e6bf36c2ef9277b0759c17c4c/src/qt/macos_appnap.mm#L20-L53
//     from https://github.com/bitcoin/bitcoin/pull/12783
//   - [retain] https://github.com/blundering/iTerm2-snippets/commit/b9d98c44e281c9be9ee080a7c32726e562f2fe8a
//
//
// Memory Management in Objective-C:
// - autorelease/retain/release
//   https://stackoverflow.com/questions/7076247/retain-release-of-returned-objects
// - Qt doesn't use Automated Reference Counting (ARC)
//   https://wiki.qt.io/Apple_Platforms_Coding_Conventions#ARC_.28Automated_Reference_Counting.29


#import <Foundation/NSProcessInfo.h>
#include "AppNapPreventingActivity.h"

void AppNapPreventingActivity::begin()
{
	activity = [[[NSProcessInfo processInfo] beginActivityWithOptions:
		NSActivityUserInitiatedAllowingIdleSystemSleep & ~NSActivitySuddenTerminationDisabled
		reason: @"Clipboard tracking initiated."]
	retain];
}

void AppNapPreventingActivity::end()
{
	[[NSProcessInfo processInfo] endActivity: activity];
	[activity release];
}
