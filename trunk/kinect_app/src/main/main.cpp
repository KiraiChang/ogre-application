#include "..\ogre_source\kinect_application.h"
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
    {
        // Create application object
        KinectApplication app;

		app.getKinectDevice()->m_hInstance = hInst;

        try {
            app.go();
        }
		catch( Ogre::Exception& e ) 
		{
            MessageBoxA( NULL, e.what(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
        }
		catch( std::exception &e )
		{
            MessageBoxA( NULL, e.what(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		}

        return 0;
    }

#ifdef __cplusplus
}
#endif