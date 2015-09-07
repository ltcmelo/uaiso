/*
 * widget.d
 *
 * This module implements the interface for a drawable feature on a GUI window.
 *
 * Author: Dave Wilkinson
 *
 */

module widg;

import gui.application;
import gui.window;

import core.definitions;

import interfaces.container;

import graphics.view;
import graphics.graphics;

import core.main;
import core.string;
import core.color;
import core.event;

// Description: This class implements and abstracts a control, which is a special container that can be drawn and added to a Window.  The control receives many events for different tasks, and allows reusable components within the static version of an application.
class Widget : Responder {

	// Description: This constructor will create the widget at the location
	//	indicated by x and y and the size indicated by width and height.
	// x: The x coordinate for the widget.
	// y: The y coordinate for the widget.
	// width: The width of the widget.
	// height: The height of the widget.
	this(int x, int y, int width, int height) {
		_subX = x;
		_x = x;

		_subY = y;
		_y = y;

		_r = _x + width;
		_b = _y + height;

		_width = width;
		_height = height;
	}

	// Deconstructor //

	~this() {
		detach();
	}

	// Events //

	// Description: Called when the control is added to a Window.
	void onAdd() {
	}

	// Description: Called when the control is removed from its parent window.
	void onRemove() {
	}

	// Description: Called when the control should be redrawn.
	// g: The graphics object that has already been locked.  Use this to draw primitives to the window.
	void onDraw(ref Graphics g) {
	}

	// Description: Called when the primary mouse button (usually the left button) is pressed.
	// mouseProps: A convenient reference to the window's Mouse structure, describing the mouse state.
	// Returns: The user should return true when the control should be redrawn.
	bool onPrimaryMouseDown(ref Mouse mouseProps) {
		return false;
	}

	// Description: Called when the primary mouse button (usually the left button) is released.
	// mouseProps: A convenient reference to the window's Mouse structure, describing the mouse state.
	// Returns: The user should return true when the control should be redrawn.
	bool onPrimaryMouseUp(ref Mouse mouseProps) {
		return false;
	}

	// Description: Called when the secondary mouse button (usually the right button) is pressed.
	// mouseProps: A convenient reference to the window's Mouse structure, describing the mouse state.
	// Returns: The user should return true when the control should be redrawn.
	bool onSecondaryMouseDown(ref Mouse mouseProps) {
		return false;
	}

	// Description: Called when the secondary mouse button (usually the right button) is released.
	// mouseProps: A convenient reference to the window's Mouse structure, describing the mouse state.
	// Returns: The user should return true when the control should be redrawn.
	bool onSecondaryMouseUp(ref Mouse mouseProps) {
		return false;
	}

	// Description: Called when the tertiary mouse button (usually the middle button) is pressed.
	// mouseProps: A convenient reference to the window's Mouse structure, describing the mouse state.
	// Returns: The user should return true when the control should be redrawn.
	bool onTertiaryMouseDown(ref Mouse mouseProps) {
		return false;
	}

	// Description: Called when the tertiary mouse button (usually the middle button) is released.
	// mouseProps: A convenient reference to the window's Mouse structure, describing the mouse state.
	// Returns: The user should return true when the control should be redrawn.
	bool onTertiaryMouseUp(ref Mouse mouseProps) {
		return false;
	}

	// Description: Called when some other atypical mouse button is pressed.
	// mouseProps: A convenient reference to the window's Mouse structure, describing the mouse state.
	// button: An index of the alternate button.
	// Returns: The user should return true when the control should be redrawn.
	bool onOtherMouseDown(ref Mouse mouseProps, uint button) {
		return false;
	}

	// Description: Called when some other atypical mouse button is released.
	// mouseProps: A convenient reference to the window's Mouse structure, describing the mouse state.
	// button: An index of the alternate button.
	// Returns: The user should return true when the control should be redrawn.
	bool onOtherMouseUp(ref Mouse mouseProps, uint button) {
		return false;
	}

	// Description: Called when the mouse cursor enters within the bounds of the control.
	// Returns: The user should return true when the control should be redrawn.
	bool onMouseEnter() {
		return false;
	}

	// Description: Called when the mouse cursor leaves the bounds of the control.
	// Returns: The user should return true when the control should be redrawn.
	bool onMouseLeave() {
		return false;
	}

	// Description: Called when the mouse moves within the bounds of the control.
	// mouseProps: A convenient reference to the window's Mouse structure, describing the mouse state.
	// Returns: The user should return true when the control should be redrawn.
	bool onMouseMove(ref Mouse mouseProps) {
		return false;
	}

	// Description: Called when the control receives focus.
	// bWasWindow: When true, the window received focus as well.
	// Returns: The user should return true when the control should be redrawn.
	bool onGotFocus(bool bWasWindow) {
		return false;
	}

	// Description: Called when the control loses focus.
	// bWasWindow: When true, the window lost focus as well.
	// Returns: The user should return true when the control should be redrawn.
	bool onLostFocus(bool bWasWindow) {
		return false;
	}

	// Description: Called when the control is focused during a key press.
	// key: The description of the key.
	// Returns: The user should return true when the control should be redrawn.
	bool onKeyDown(Key key) {
		return false;
	}

	// Description: Called when the control is focused during a key release.
	// keyCode: The description of the key.
	// Returns: The user should return true when the control should be redrawn.
	bool onKeyUp(Key key) {
		return false;
	}

	// Description: Called when the control is focused during a key press that results in a printable character.
	// keyChar: The character in standard UTF-32.
	// Returns: The user should return true when the control should be redrawn.
	bool onKeyChar(dchar keyChar) {
		return false;
	}

	// Methods

	// Description: Will return a boolean indictating whether or not the window given is the parent window.
	// window: The window to determine if it is the parent.
	// Returns: Will return true if the owning window is the same as the one passed by the parameter.
	bool isOfWindow(ref Window window) {
		if (_window is window) {
			return true;
		}

		return false;
	}

	// Description: Will return a boolean indictating whether or not the container given is the parent container.
	// container: The container to determine if it is the parent.
	// Returns: Will return true if the owning container is the same as the one passed by the parameter.
	bool isOfContainer(AbstractContainer container) {
		if (_container is container) {
			return true;
		}

		return false;
	}

	// Description: Will return a reference to the parent window that owns this control.
	// Returns: The reference to the current window object that owns this control instance.
	Window parent() {
		return _window;
	}

	// Description: Will remove this control to whatever it has been added to.
	void detach() {
		if (_window is null) { return; }
		if (_nextControl is null) { return; }

		_window.removeControl(this);
	}

	// Description: Will return a boolean value describing whether the point is within the region occupied by the control.
	// Returns: Will return true when the point is within the region.
	bool containsPoint(int x, int y) {
		if (_x < x && _y < y &&
			_b > y && _r > x) {
			return true;
		}

		return false;
	}

	// Description: Will return the flag that determines whether or not the control is visible (ie. drawn).
	// Returns: If true, the control is currently visible.  If false, the control is currently hidden.
	bool visible() {
		return _visible;
	}

	// Description: Will set the flag to mark the control either visible or hidden.
	// bVisible: Passing true would mark this control to be drawn, passing false will mark this control as hidden and not drawn.
	void visible(bool bVisible) {
		_visible = bVisible;
	}

	// Widget type information

	bool isContainer() {
		return false;
	}

	bool hovered() {
		return _hovered;
	}

	bool focused() {
		return _focused;
	}

	void enabled(bool bEnable) {
		_enabled = bEnable;
	}

	bool enabled() {
		return _enabled;
	}

	uint width() {
		return _width;
	}

	uint height() {
		return _height;
	}

	int left() {
		return _subX;
	}

	int top() {
		return _subY;
	}

	int right() {
		return _r;
	}

	int bottom() {
		return _b;
	}

	void resize(uint width, uint height) {
		_width = width;
		_height = height;

		_r = _x + width;
		_b = _y + height;
	}

	void move(int x, int y) {
		if (_container !is null) {
			_x = x + _container.baseLeft;
			_y = y + _container.baseTop;
		}
		else {
			_x = x;
			_y = y;
		}

		_subX = x;
		_subY = y;

		_r = _x + _width;
		_b = _y + _height;
	}

protected:

	// - will post an event to this control
	//void PostEvent(Int32 theEvent, Int32 p1, Int32 p2);

	void requestCapture() {
		if (_window !is null) {
//			WindowCaptureMouse(_window, this);
		}
	}

	void requestRelease() {
		if (_window !is null) {
//			WindowReleaseMouse(_window, this);
		}
	}

private:

	package Window _window = null;

	package AbstractContainer _container = null;

	package View _view = null;

	package bool _visible = true;

	package bool _enabled = true;
	package bool _focused = false;
	package bool _hovered = false;

	package bool _needs_redraw = false;

	int _x = -1; //x (for window)
	int _y = -1; //y
	int _r = -1; //right
	int _b = -1; //bottom
	int _subX = -1; // x within container
	int _subY = -1; // y within container

	int _width = -1;   //width
	int _height = -1;  //height

	package void removeControl() {
		onRemove();

		_view = null;
		_window = null;

		_nextControl = null;
		_prevControl = null;
	}

	// control list
	package Widget _nextControl;
	package Widget _prevControl;
}

// Description: This control will provide a simple control container.
class Container : Widget, AbstractContainer
{
	// Description: This will create a button with the specified dimensions and text.
	this(int x, int y, int width, int height) {
		super(x,y,width,height);
	}

	override void onAdd() {
	}

	override void onDraw(ref Graphics g) {
		g.clipSave();

		g.clipRect(_x,_y,_r,_b);

		Widget c = _firstControl;

		if (c !is null) {
			do {
				c =	c._prevControl;

				c.onDraw(g);
			} while (c !is _firstControl)
		}

		g.clipRestore();
	}

	override bool isContainer() {
		return true;
	}

	void addControl(Widget control) {
		// do not add a control that is already part of another window
		if (control.parent !is null) { return; }

		// add to the control linked list
		if (_firstControl is null && _lastControl is null) {
			// first control

			_firstControl = control;
			_lastControl = control;

			control._nextControl = control;
			control._prevControl = control;
		}
		else {
			// next control
			control._nextControl = _firstControl;
			control._prevControl = _lastControl;

			_firstControl._prevControl = control;
			_lastControl._nextControl = control;

			_firstControl = control;
		}

		// increase the number of controls
		_numControls++;

		// call a function initializing a control on the control's end
		Window wnd = this.parent;

		control._window = wnd;
		control._view = _view;
		control._container = this;

		control.onAdd();

		control.move(control.left, control.top);
	}

	void removeControl(Widget control) {
		if (control.isOfContainer(this)) {
			if (_firstControl is null && _lastControl is null) {
				// it is the last control
				_firstControl = null;
				_lastControl = null;
			}
			else {
				// is it not the last control

				if (_firstControl is control) {
					_firstControl = _firstControl._nextControl;
				}

				if (_lastControl is control) {
					_lastControl = _lastControl._prevControl;
				}

				control.removeControl();
			}

			_numControls--;
		}
	}

	Widget controlAtPoint(int x, int y) {
		Widget ctrl = _firstControl;

		if (ctrl !is null) {
			do {
				if (ctrl.containsPoint(x,y) && ctrl.visible) {
					if (ctrl.isContainer()) {
						Widget innerCtrl = (cast(AbstractContainer)ctrl).controlAtPoint(x,y);
						if (innerCtrl !is null) { return innerCtrl; }
					}
					else {
						return ctrl;
					}
				}
				ctrl = ctrl._nextControl;
			} while (ctrl !is _firstControl)
		}

		return null;
	}

	override void move(int x, int y) {
		super.move(x,y);

		Widget ctrl = _firstControl;

		if (ctrl !is null) {
			do {
				ctrl.move(ctrl.left, ctrl.top);

				ctrl = ctrl._nextControl;
			} while (ctrl !is _firstControl)
		}
	}

	int baseLeft() {
		return _x;
	}

	int baseTop() {
		return _y;
	}

private:

	// head and tail of the control linked list
	package Widget _firstControl = null;	//head
	package Widget _lastControl = null;	//tail
	package int _numControls = 0;

	package Widget _captured_control = null;
	package Widget _last_control = null;
	package Widget _focused_control = null;
}
