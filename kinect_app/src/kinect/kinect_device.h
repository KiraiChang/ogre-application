#ifndef _KINECT_DEVICE_H_
#define _KINECT_DEVICE_H_

#include <windows.h>
#include <NuiApi.h>

class KinectDevice
{
private:
    INuiSensor *            m_pNuiSensor;
    BSTR                    m_instanceId;
public:
	HINSTANCE               m_hInstance;

public:
							KinectDevice(void);
	virtual					~KinectDevice(void);

	//about kinect operator function
	HRESULT                 Nui_Init(void);
    HRESULT                 Nui_Init( OLECHAR * instanceName );
    void                    Nui_UnInit(void);
    void                    Nui_Zero(void);

	int						MessageBoxResource(UINT nID, UINT nType);

	void					getSkeletonFrame(NUI_SKELETON_FRAME &frame);
};

#endif //_KINECT_DEVICE_H_