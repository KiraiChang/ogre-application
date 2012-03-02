#include "kinect_application.h"
#include "../kinect/resource.h"

KinectApplication::KinectApplication(void):m_pNuiSensor(NULL)
{
	ZeroMemory(m_szAppTitle, sizeof(m_szAppTitle));
    LoadString(m_hInstance, IDS_APPTITLE, m_szAppTitle, _countof(m_szAppTitle));
}

KinectApplication::~KinectApplication(void)
{
}

const std::string KinectApplication::getApplicationName(void)const
{
	return "KinectOgreApplication";
}

//about kinect operator function
HRESULT KinectApplication::Nui_Init( )
{
	HRESULT  hr;
    RECT     rc;
    bool     result;

    if ( !m_pNuiSensor )
    {
        HRESULT hr = NuiCreateSensorByIndex(0, &m_pNuiSensor);

        if ( FAILED(hr) )
        {
            return hr;
        }

        SysFreeString(m_instanceId);

        m_instanceId = m_pNuiSensor->NuiDeviceConnectionId();
    }

	DWORD nuiFlags = NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON |  NUI_INITIALIZE_FLAG_USES_COLOR;
    hr = m_pNuiSensor->NuiInitialize( nuiFlags );
    if ( E_NUI_SKELETAL_ENGINE_BUSY == hr )
    {
        nuiFlags = NUI_INITIALIZE_FLAG_USES_DEPTH |  NUI_INITIALIZE_FLAG_USES_COLOR;
        hr = m_pNuiSensor->NuiInitialize( nuiFlags) ;
    }
  
    //if ( FAILED( hr ) )
    //{
    //    if ( E_NUI_DEVICE_IN_USE == hr )
    //    {
    //        MessageBoxResource( IDS_ERROR_IN_USE, MB_OK | MB_ICONHAND );
    //    }
    //    else
    //    {
    //        MessageBoxResource( IDS_ERROR_NUIINIT, MB_OK | MB_ICONHAND );
    //    }
    //    return hr;
    //}

    //if ( HasSkeletalEngine( m_pNuiSensor ) )
    //{
    //    hr = m_pNuiSensor->NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, 0 );
    //    if( FAILED( hr ) )
    //    {
    //        MessageBoxResource( IDS_ERROR_SKELETONTRACKING, MB_OK | MB_ICONHAND );
    //        return hr;
    //    }
    //}

    //hr = m_pNuiSensor->NuiImageStreamOpen(
    //    NUI_IMAGE_TYPE_COLOR,
    //    NUI_IMAGE_RESOLUTION_640x480,
    //    0,
    //    2,
    //    m_hNextColorFrameEvent,
    //    &m_pVideoStreamHandle );

    //if ( FAILED( hr ) )
    //{
    //    MessageBoxResource( IDS_ERROR_VIDEOSTREAM, MB_OK | MB_ICONHAND );
    //    return hr;
    //}

    //hr = m_pNuiSensor->NuiImageStreamOpen(
    //    HasSkeletalEngine(m_pNuiSensor) ? NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX : NUI_IMAGE_TYPE_DEPTH,
    //    NUI_IMAGE_RESOLUTION_320x240,
    //    0,
    //    2,
    //    m_hNextDepthFrameEvent,
    //    &m_pDepthStreamHandle );

    //if ( FAILED( hr ) )
    //{
    //    MessageBoxResource(IDS_ERROR_DEPTHSTREAM, MB_OK | MB_ICONHAND);
    //    return hr;
    //}

    //// Start the Nui processing thread
    //m_hEvNuiProcessStop = CreateEvent( NULL, FALSE, FALSE, NULL );
    //m_hThNuiProcess = CreateThread( NULL, 0, Nui_ProcessThread, this, 0, NULL );

    return hr;
}

HRESULT KinectApplication::Nui_Init( OLECHAR * instanceName )
{

}

void KinectApplication::Nui_UnInit( )
{

}

void KinectApplication::Nui_GotDepthAlert( )
{

}

void KinectApplication::Nui_GotColorAlert( )
{

}

void KinectApplication::Nui_GotSkeletonAlert( )
{

}

void KinectApplication::Nui_Zero()
{

}

int KinectApplication::MessageBoxResource( UINT nID, UINT nType )
{
    static TCHAR szRes[512];

    LoadString( m_hInstance, nID, szRes, _countof(szRes) );
    return MessageBox( NULL, szRes, m_szAppTitle, nType );
}