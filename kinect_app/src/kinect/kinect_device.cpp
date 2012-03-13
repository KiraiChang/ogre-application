#include "resource.h"
#include "kinect_device.h"

KinectDevice::KinectDevice(void):m_pNuiSensor(NULL), m_instanceId(NULL)
{
}

KinectDevice::~KinectDevice(void)
{

}

//about kinect operator function
HRESULT KinectDevice::Nui_Init( )
{
	HRESULT  hr;
    //RECT     rc;
    //bool     result;

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
  
    if ( FAILED( hr ) )
    {
        if ( E_NUI_DEVICE_IN_USE == hr )
        {
            MessageBoxResource( IDS_ERROR_IN_USE, MB_OK | MB_ICONHAND );
        }
        else
        {
            MessageBoxResource( IDS_ERROR_NUIINIT, MB_OK | MB_ICONHAND );
        }
        return hr;
    }

    if ( HasSkeletalEngine( m_pNuiSensor ) )
    {
        hr = m_pNuiSensor->NuiSkeletonTrackingEnable( NULL, 0 );
        if( FAILED( hr ) )
        {
            MessageBoxResource( IDS_ERROR_SKELETONTRACKING, MB_OK | MB_ICONHAND );
            return hr;
        }
    }

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

HRESULT KinectDevice::Nui_Init( OLECHAR * instanceName )
{
	// Generic creation failure
	if ( NULL == instanceName )
	{
		MessageBoxResource( IDS_ERROR_NUICREATE, MB_OK | MB_ICONHAND );
		return E_FAIL;
	}

	HRESULT hr = NuiCreateSensorById( instanceName, &m_pNuiSensor );

	// Generic creation failure
	if ( FAILED(hr) )
	{
		MessageBoxResource( IDS_ERROR_NUICREATE, MB_OK | MB_ICONHAND );
		return hr;
	}

	SysFreeString(m_instanceId);

	m_instanceId = m_pNuiSensor->NuiDeviceConnectionId();

	return Nui_Init();
}

void KinectDevice::Nui_UnInit( )
{
    //SelectObject( m_SkeletonDC, m_SkeletonOldObj );
    //DeleteDC( m_SkeletonDC );
    //DeleteObject( m_SkeletonBMP );

    //if ( NULL != m_Pen[0] )
    //{
    //    for ( int i = 0; i < NUI_SKELETON_COUNT; i++ )
    //    {
    //        DeleteObject( m_Pen[i] );
    //    }
    //    ZeroMemory( m_Pen, sizeof(m_Pen) );
    //}

    //if ( NULL != m_hFontSkeletonId )
    //{
    //    DeleteObject( m_hFontSkeletonId );
    //    m_hFontSkeletonId = NULL;
    //}

    // Stop the Nui processing thread
    //if ( NULL != m_hEvNuiProcessStop )
    //{
    //    // Signal the thread
    //    SetEvent(m_hEvNuiProcessStop);

    //    // Wait for thread to stop
    //    if ( NULL != m_hThNuiProcess )
    //    {
    //        WaitForSingleObject( m_hThNuiProcess, INFINITE );
    //        CloseHandle( m_hThNuiProcess );
    //    }
    //    CloseHandle( m_hEvNuiProcessStop );
    //}

    //if ( m_hNextSkeletonEvent && ( m_hNextSkeletonEvent != INVALID_HANDLE_VALUE ) )
    //{
    //    CloseHandle( m_hNextSkeletonEvent );
    //    m_hNextSkeletonEvent = NULL;
    //}
    //if ( m_hNextDepthFrameEvent && ( m_hNextDepthFrameEvent != INVALID_HANDLE_VALUE ) )
    //{
    //    CloseHandle( m_hNextDepthFrameEvent );
    //    m_hNextDepthFrameEvent = NULL;
    //}
    //if ( m_hNextColorFrameEvent && ( m_hNextColorFrameEvent != INVALID_HANDLE_VALUE ) )
    //{
    //    CloseHandle( m_hNextColorFrameEvent );
    //    m_hNextColorFrameEvent = NULL;
    //}

	if ( m_pNuiSensor )
    {
        m_pNuiSensor->NuiShutdown( );
    }

    if ( m_pNuiSensor )
    {
        m_pNuiSensor->Release();
        m_pNuiSensor = NULL;
    }
}

void KinectDevice::Nui_Zero()
{
	if (m_pNuiSensor)
	{
		m_pNuiSensor->Release();
		m_pNuiSensor = NULL;
	}
	//m_hNextDepthFrameEvent = NULL;
	//m_hNextColorFrameEvent = NULL;
	//m_hNextSkeletonEvent = NULL;
	//m_pDepthStreamHandle = NULL;
	//m_pVideoStreamHandle = NULL;
	//m_hThNuiProcess = NULL;
	//m_hEvNuiProcessStop = NULL;
	//ZeroMemory(m_Pen,sizeof(m_Pen));
	//m_SkeletonDC = NULL;
	//m_SkeletonBMP = NULL;
	//m_SkeletonOldObj = NULL;
	//m_PensTotal = 6;
	//ZeroMemory(m_Points,sizeof(m_Points));
	//m_LastSkeletonFoundTime = 0;
	//m_bScreenBlanked = false;
	//m_DepthFramesTotal = 0;
	//m_LastDepthFPStime = 0;
	//m_LastDepthFramesTotal = 0;
	//ZeroMemory(m_SkeletonIds,sizeof(m_SkeletonIds));
	//ZeroMemory(m_TrackedSkeletonIds,sizeof(m_SkeletonIds));
}

int KinectDevice::MessageBoxResource( UINT nID, UINT nType )
{
    static TCHAR szRes[512];

    LoadString( m_hInstance, nID, szRes, _countof(szRes) );
    return MessageBox( NULL, szRes, "KinectDeviceError", nType );
}

//return true have get the skeletonFrame
bool KinectDevice::getSkeletonFrame(NUI_SKELETON_FRAME &frame)
{
    bool bFoundSkeleton = false;

    if ( SUCCEEDED(m_pNuiSensor->NuiSkeletonGetNextFrame( 0, &frame )) )
    {
        for ( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
        {
            if( frame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED ||
                (frame.SkeletonData[i].eTrackingState == NUI_SKELETON_POSITION_ONLY))
            {
                bFoundSkeleton = true;
            }
        }
    }

	    // no skeletons!
    if( !bFoundSkeleton )
    {
        return false;
    }

    // smooth out the skeleton data
    HRESULT hr = m_pNuiSensor->NuiTransformSmooth(&frame, NULL);
    if ( FAILED(hr) )
    {
        return false;
    }

	return bFoundSkeleton;
}