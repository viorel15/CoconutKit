//
//  HLSStackController.h
//  CoconutKit
//
//  Created by Samuel Défago on 22.07.11.
//  Copyright 2011 Hortis. All rights reserved.
//

#import "HLSContainerStack.h"
#import "HLSViewController.h"

// Forward declarations
@protocol HLSStackControllerDelegate;

/**
 * We often need to manage a stack of view controllers. Usually, we use a navigation controller, but there is no way
 * to use other transition animations as the built-in ones. Sometimes, we also want to show view controllers
 * modally, but often the usual presentModalViewController:animated: method of UIViewController is too limited (modal
 * sheets on the iPad have pre-defined sizes, and when displaying full screen the view below disappears, which prevents
 * from displaying transparent modal windows).
 *
 * To circumvent those problems, HLSStackController provides a generic way to deal with a view controller stack. It can
 * be applied a richer set of transition animations. HLSStackController is not meant to be subclassed.
 *
 * This view controller container guarantees correct view lifecycle and rotation event propagation to the view controllers
 * it manages. Note that when a view controller gets pushed onto the stack, the view controller below will get the
 * viewWillDisappear: and viewDidDisappear: events, even if it stays visible through transparency (the same holds for
 * the viewWillAppear: and viewDidAppear: events when the view controller on top gets popped).
 * This decision was made because it would have been extremely difficult and costly to look at all view controller's 
 * views in the stack to find those which are really visible (this would have required to find the intersections of all 
 * view and subview rectangles, cumulating alphas to find which parts of the view stack are visible and which aren't;
 * clearly not worth it).
 *
 * When a view controller's view is inserted into a stack controller, its view frame is automatically adjusted to match 
 * the container view bounds, as for usual UIKit containers (UITabBarController, UINavigationController). Be sure that
 * the view controller's view size and autoresizing behaviors are correctly set.
 *
 * HLSStackController uses the smoother 1-step rotation available from iOS3. You cannot use the 2-step rotation for view 
 * controllers you pushed in it (it will be ignored, see UIViewController documentation). The 2-step rotation is deprecated 
 * starting with iOS 5, you should not use it anymore anyway.
 *
 * Since a stack controller can manage many view controller's views, and since in general only the first few top ones
 * need to be visible, it would be a waste of resources to keep all views loaded at any time. At creation time, the
 * maximal number of loaded view controllers ("capacity") can be provided. By default, the capacity is set to 2, 
 * which means that the container guarantees that at most the two top view controller's views are loaded. The 
 * controller simply unloads the view controller's views below in the stack so save memory. Usually, the default value
 * should fulfill most needs, but if you require more transparency levels or if you want to minimize load / unload
 * operations, you can increase this value. Standard capacity values are provided at the beginning of this file.
 *
 * You can also use stack controllers with storyboards (a feature available since iOS 5):
 *   - drop a view controller onto the storyboard, and set its class to HLSStackController. You can customize the
 *     view controller capacity by setting an NSNumber user-defined runtime attribute called 'capacity'
 *   - drop another view controller onto the storyboard, and set it as root view controller of the stack by
 *     binding the stack controller with it using an HLSStackPushSegue called 'hls_root'. The transition style which
 *     gets applied is always HLSTransitionStyleNone and cannot be customized
 *   - if you want to push another view controller, drop a view controller onto the storyboard, and connect the 
 *     root view controller with it using another HLSStackPushSegue (with any non-reserved identifier). If you 
 *     need to customize transition settings (style and duration), you must implement the -prepareForSegue:sender: 
 *     method in your source view controller (the root view controller in this example)
 *   - if you want to pop a view controller, bind it to any other view controller (in general the one towards
 *     which the transition will occur, or itself) using an HLSStackPopSegue
 * For further information, refer to the documentation of HLSStackPushSegue and HLSStackPopSegue.
 *
 * TODO: This class currently does not support view controllers implementing the HLSOrientationCloner protocol
 *
 * Designated initializer: initWithRootViewController:capacity:
 */
@interface HLSStackController : HLSViewController <HLSContainerStackDelegate> {
@private
    HLSContainerStack *m_containerStack;
    NSUInteger m_capacity;
    id<HLSStackControllerDelegate> m_delegate;
}

/**
 * Create a new stack controller with the specified view controller as root. This view controller cannot be animated when 
 * installed, and can neither be replaced, nor removed. The capacity can be freely set.
 */
- (id)initWithRootViewController:(UIViewController *)rootViewController capacity:(NSUInteger)capacity;

/**
 * Create a new stack controller with the specified view controller as root. This view controller cannot be animated when 
 * installed, and can neither be replaced, nor removed. The default capacity is used.
 */
- (id)initWithRootViewController:(UIViewController *)rootViewController;

/**
 * Push a view controller onto the stack without animation.
 * This method can also be called before the stack controller is displayed
 */
- (void)pushViewController:(UIViewController *)viewController animated:(BOOL)animated;

/**
 * Push a view controller onto the stack using one of the built-in transition styles. The transition duration is set by 
 * the animation itself
 * This method can also be called before the stack controller is displayed (the animation does not get played, but this
 * defines the pop animation which will get played when the view controller is later removed)
 */
- (void)pushViewController:(UIViewController *)viewController 
       withTransitionClass:(Class)transitionClass
                  animated:(BOOL)animated;

/**
 * Same as pushViewController:withTransitionStyle:, but the transition duration can be overridden (the duration will be 
 * evenly distributed on the animation steps composing the animation so that the animation rhythm stays the same). Use 
 * the reserved kAnimationTransitionDefaultDuration value as duration to get the default transition duration (same 
 * result as the method above)
 * This method can also be called before the stack controller is displayed (the animation does not get played, but this
 * defines the pop animation which will get played when the view controller is later removed)
 */
- (void)pushViewController:(UIViewController *)viewController
       withTransitionClass:(Class)transitionClass
                  duration:(NSTimeInterval)duration
                  animated:(BOOL)animated;

/**
 * Remove the top view controller from the stack, reversing the transition animation which was used when it was pushed.
 * The root view controller cannot be popped
 */
- (void)popViewControllerAnimated:(BOOL)animated;

/**
 * Pop all view controllers to get back to a given view controller. The current top view controller will transition
 * to the specified view controller using the reverse animation with which it was pushed onto the stack. If the
 * view controller to pop to does not belong to the stack or is nil, this method does nothing
 */
- (void)popToViewController:(UIViewController *)viewController animated:(BOOL)animated;

/**
 * Pop all view controllers to get back to the root view controller. The current top view controller will transition
 * to the root view controller using the reverse animation with which it was pushed onto the stack
 */
- (void)popToRootViewControllerAnimated:(BOOL)animated;

/**
 * Return the view controller at the bottom
 */
- (UIViewController *)rootViewController;

/**
 * Return the view controller currently on top
 */
- (UIViewController *)topViewController;

/**
 * The view controllers in the stack. The first one is the root view controller, the last one the top one
 */
- (NSArray *)viewControllers;

@property (nonatomic, assign) id<HLSStackControllerDelegate> delegate;

@end

@protocol HLSStackControllerDelegate <NSObject>

@optional

- (void)stackController:(HLSStackController *)stackController
 willPushViewController:(UIViewController *)pushedViewController
    coverViewController:(UIViewController *)coveredViewController
               animated:(BOOL)animated;

- (void)stackController:(HLSStackController *)stackController
 willShowViewController:(UIViewController *)viewController 
               animated:(BOOL)animated;

- (void)stackController:(HLSStackController *)stackController
  didShowViewController:(UIViewController *)viewController
               animated:(BOOL)animated;

- (void)stackController:(HLSStackController *)stackController
  didPushViewController:(UIViewController *)pushedViewController
    coverViewController:(UIViewController *)coveredViewController
               animated:(BOOL)animated;

- (void)stackController:(HLSStackController *)stackController
  willPopViewController:(UIViewController *)poppedViewController
   revealViewController:(UIViewController *)revealedViewController
               animated:(BOOL)animated;

- (void)stackController:(HLSStackController *)stackController
 willHideViewController:(UIViewController *)viewController
               animated:(BOOL)animated;

- (void)stackController:(HLSStackController *)stackController
  didHideViewController:(UIViewController *)viewController
               animated:(BOOL)animated;

- (void)stackController:(HLSStackController *)stackController
   didPopViewController:(UIViewController *)poppedViewController
   revealViewController:(UIViewController *)revealedViewController
               animated:(BOOL)animated;

@end

@interface UIViewController (HLSStackController)

/**
 * Return the stack controller the view controller is inserted in, or nil if none.
 */
@property (nonatomic, readonly, assign) HLSStackController *stackController;

@end
