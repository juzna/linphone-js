/**********************************************************\

  Auto-generated FactoryMain.cpp

  This file contains the auto-generated factory methods
  for the linphone project

\**********************************************************/

#include "FactoryDefinitions.h"
#include "linphone.h"

FB::PluginCore *_getMainPlugin()
{
    return new linphone();
}

void GlobalPluginInitialize()
{
    linphone::StaticInitialize();
}

void GlobalPluginDeinitialize()
{
    linphone::StaticDeinitialize();
}
