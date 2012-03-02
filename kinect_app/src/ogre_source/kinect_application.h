#ifndef _KINECT_APPLICATION_H_
#define _KINECT_APPLICATION_H_

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include <NuiApi.h>
#endif

#include "ogre_application.h"

class KinectApplication : public OgreApplication
{
private:
	// Current kinect
    INuiSensor *            m_pNuiSensor;
    BSTR                    m_instanceId;
public:
	HINSTANCE               m_hInstance;
	TCHAR                   m_szAppTitle[256];    // Application title

public:
	KinectApplication(void);
	virtual ~KinectApplication(void);
	virtual const std::string getApplicationName(void)const;

	//about kinect operator function
	HRESULT                 Nui_Init( );
    HRESULT                 Nui_Init( OLECHAR * instanceName );
    void                    Nui_UnInit( );
    void                    Nui_GotDepthAlert( );
    void                    Nui_GotColorAlert( );
    void                    Nui_GotSkeletonAlert( );

    void                    Nui_Zero();

	int MessageBoxResource(UINT nID, UINT nType);
};

#endif _KINECT_APPLICATION_H_