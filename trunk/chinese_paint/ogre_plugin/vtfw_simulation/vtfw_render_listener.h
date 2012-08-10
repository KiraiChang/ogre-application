#ifndef _VTFW_RENDER_LISTENER_H_
#define _VTFW_RENDER_LISTENER_H_

#include <OgreRenderQueueListener.h>
#include <ogretexture.h>

class VTFWRenderListener : public Ogre::RenderQueueListener
{
private:
	Ogre::RenderTarget *						m_pOldRenderTarget;
	Ogre::TexturePtr							m_sPtrRenderTarget;
	Ogre::Camera *								m_pCamera;
public:
												VTFWRenderListener(const Ogre::String &textureName, Ogre::Camera *pCamera, int texSize);
												~VTFWRenderListener(void);
	void									 	preRenderQueues (void);
	void 										postRenderQueues (void);
	//void										renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, 
	//												bool& skipThisInvocation);

	//void										renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, 
	//												bool& repeatThisInvocation);

	Ogre::TexturePtr							getPixelBuffer();
	//const Ogre::uint8							getQueueID();
};

#endif //_VTFW_RENDER_LISTENER_H_