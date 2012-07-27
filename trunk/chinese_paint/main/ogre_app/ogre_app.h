#ifndef _OGRE_APP_H_
#define _OGRE_APP_H_

#include <SampleBrowser.h>

class OgreApp : public OgreBites::SampleBrowser
{
private:
	void redirectIOToConsole(void);
public:
	OgreApp(void);
};


#endif _OGRE_APP_H_