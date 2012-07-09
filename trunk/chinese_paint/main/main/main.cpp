
#include "OgrePlatform.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_SYMBIAN 
#	ifdef __GCCE__
#		include <staticlibinit_gcce.h>
#	endif

#	include <e32base.h> // for Symbian classes.
#	include <coemain.h> // for CCoeEnv.

#endif 

#include "..\ogre_app\ogre_app.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include "SampleBrowser_OSX.h"
#elif OGRE_PLATFORM == OGRE_PLATFORM_IPHONE
#include "SampleBrowser_iOS.h"
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
#elif OGRE_PLATFORM == OGRE_PLATFORM_SYMBIAN    
int mainWithTrap();
int main()
{
	int res = 0;
    __UHEAP_MARK; 
 
	// Create the control environment.
	CCoeEnv* environment = new (ELeave) CCoeEnv();    

	TRAPD( err, environment->ConstructL( ETrue, 0 ) );

	if( err != KErrNone )
	{
		printf( "Unable to create a CCoeEnv!\n" );
		getchar();            
	}
    
    TRAP( err, res = mainWithTrap());

    // Close the stdout & stdin, else printf / getchar causes a memory leak.
    fclose( stdout );
    fclose( stdin );
    
	// Cleanup
	CCoeEnv::Static()->DestroyEnvironment();
	delete CCoeEnv::Static();
       
    __UHEAP_MARKEND;
    
    return res;
}    

int mainWithTrap()

#else

int main(int argc, char *argv[])
#endif
{
#if OGRE_PLATFORM == OGRE_PLATFORM_IPHONE
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	int retVal = UIApplicationMain(argc, argv, @"UIApplication", @"AppDelegate");
	[pool release];
	return retVal;
#elif (OGRE_PLATFORM == OGRE_PLATFORM_APPLE) && __LP64__
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    mAppDelegate = [[AppDelegate alloc] init];
    [[NSApplication sharedApplication] setDelegate:mAppDelegate];
	int retVal = NSApplicationMain(argc, (const char **) argv);

	[pool release];

	return retVal;
#else

	try
	{
		OgreApp oa;
		oa.go();
	}
	catch (Ogre::Exception& e)
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_ICONERROR | MB_TASKMODAL);
#else
		std::cerr << "An exception has occurred: " << e.getFullDescription().c_str() << std::endl;
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_SYMBIAN
        getchar();
#endif

	}

#endif
	return 0;
}