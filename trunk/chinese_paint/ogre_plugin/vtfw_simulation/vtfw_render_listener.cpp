#include "vtfw_render_listener.h"
#include <ogre.h>

const Ogre::uint8 UINT8_VTFW_QUEUE_ID = Ogre::RENDER_QUEUE_9; 

VTFWRenderListener::VTFWRenderListener(const Ogre::String &textureName, Ogre::Camera *pCamera, int texSize):
						m_pCamera(pCamera), 
						m_pOldRenderTarget(NULL)
{
	// create height texture
	m_sPtrRenderTarget = Ogre::TextureManager::getSingleton().createManual(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, texSize, texSize, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET);
}

VTFWRenderListener::~VTFWRenderListener(void)
{
}

void VTFWRenderListener::preRenderQueues (void)
{
	if(m_sPtrRenderTarget.isNull())
	{
		return ;
	}

	m_pOldRenderTarget = m_pCamera->getViewport()->getTarget();
	Ogre::RenderSystem* pRenderSystem = Ogre::Root::getSingleton().getRenderSystem();
	Ogre::RenderTarget* pTarget = m_sPtrRenderTarget->getBuffer()->getRenderTarget();
	pTarget->setAutoUpdated(false);
	pTarget->setActive(true);
	Ogre::Root::getSingleton().getRenderSystem()->_setRenderTarget(pTarget);
	Ogre::Root::getSingleton().getRenderSystem()->clearFrameBuffer(Ogre::FBT_COLOUR/*|Ogre::FBT_DEPTH|Ogre::FBT_STENCIL*/,Ogre::ColourValue::Black);
	//Ogre::Root::getSingleton().getRenderSystem()->unbindGpuProgram()

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("ChinesePaint/WaterHLSL");
	mat->load();
	Ogre::Root::getSingleton().getRenderSystem()->bindGpuProgram(mat->getTechnique(0)->getPass(0)->getVertexProgram()->_getBindingDelegate());
	Ogre::Root::getSingleton().getRenderSystem()->bindGpuProgram(mat->getTechnique(0)->getPass(0)->getFragmentProgram()->_getBindingDelegate());
}

void VTFWRenderListener::postRenderQueues (void)
{
	if(m_pOldRenderTarget!=NULL/*&&m_bRenderTexture*/)
	{
		m_sPtrRenderTarget->getBuffer()->getRenderTarget()->writeContentsToFile("height.png");
		Ogre::RenderSystem* pRenderSystem = Ogre::Root::getSingleton().getRenderSystem();
		pRenderSystem->_setRenderTarget(m_pOldRenderTarget);
		m_pOldRenderTarget = NULL;

	}
}

//void VTFWRenderListener::renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, 
//	bool& skipThisInvocation)
//{
//	if(m_sPtrRenderTarget.isNull())
//	{
//		return ;
//	}
//
//	if(queueGroupId==UINT8_VTFW_QUEUE_ID/*&&m_pRenderTarget.isNull()==false*//*&&m_bRenderTexture==true*/)
//	{
//		m_pOldRenderTarget = m_pCamera->getViewport()->getTarget();
//		Ogre::RenderSystem* pRenderSystem = Ogre::Root::getSingleton().getRenderSystem();
//		Ogre::RenderTarget* pTarget = m_sPtrRenderTarget->getBuffer()->getRenderTarget();
//		pTarget->setAutoUpdated(false);
//		pTarget->setActive(true);
//		Ogre::Root::getSingleton().getRenderSystem()->_setRenderTarget(pTarget);
//		Ogre::Root::getSingleton().getRenderSystem()->clearFrameBuffer(Ogre::FBT_COLOUR/*|Ogre::FBT_DEPTH|Ogre::FBT_STENCIL*/,Ogre::ColourValue::Black);
//		//Ogre::Root::getSingleton().getRenderSystem()->unbindGpuProgram()
//
//		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("ChinesePaint/WaterHLSL");
//		mat->load();
//		Ogre::Root::getSingleton().getRenderSystem()->bindGpuProgram(mat->getTechnique(0)->getPass(0)->getVertexProgram()->_getBindingDelegate());
//		Ogre::Root::getSingleton().getRenderSystem()->bindGpuProgram(mat->getTechnique(0)->getPass(0)->getFragmentProgram()->_getBindingDelegate());
//
//
//		skipThisInvocation = false;
//	}
//}

//void VTFWRenderListener::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, 
//	bool& repeatThisInvocation)
//{
//	if(queueGroupId==UINT8_VTFW_QUEUE_ID)
//	{
//		if(m_pOldRenderTarget!=NULL/*&&m_bRenderTexture*/)
//		{
//			m_sPtrRenderTarget->getBuffer()->getRenderTarget()->writeContentsToFile("height.png");
//			Ogre::RenderSystem* pRenderSystem = Ogre::Root::getSingleton().getRenderSystem();
//			pRenderSystem->_setRenderTarget(m_pOldRenderTarget);
//			m_pOldRenderTarget = NULL;
//
//		}
//		repeatThisInvocation = false;
//	}
//}

Ogre::TexturePtr VTFWRenderListener::getPixelBuffer()
{
	return m_sPtrRenderTarget;
}

//const Ogre::uint8 VTFWRenderListener::getQueueID()
//{
//	return UINT8_VTFW_QUEUE_ID;
//}