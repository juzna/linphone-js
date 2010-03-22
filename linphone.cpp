/**********************************************************\

  Auto-generated linphone.cpp

  This file contains the auto-generated main plugin object
  implementation for the linphone project

\**********************************************************/

#include "NpapiTypes.h"
#include "linphoneAPI.h"

#include "linphone.h"

void linphone::StaticInitialize()
{
    // Place one-time initialization stuff here; note that there isn't an absolute guarantee that
    // this will only execute once per process, just a guarantee that it won't execute again until
    // after StaticDeinitialize is called
    printf("juzna: first init\n");
}

void linphone::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here
    printf("juzna: last init\n");
}


linphone::linphone()
{
}

linphone::~linphone()
{
}

FB::JSAPI* linphone::createJSAPI()
{
    // m_host is the BrowserHostWrapper
    return new linphoneAPI(m_host);
}

bool linphone::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool linphone::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool linphone::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
bool linphone::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *)
{
    // The window is attached; act appropriately
    return false;
}

bool linphone::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *)
{
    // The window is about to be detached; act appropriately
    return false;
}
