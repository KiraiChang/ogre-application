#ifndef _VTFW_MESH_H_
#define _VTFW_MESH_H_
#include "../simulation_interface/water_interface.h"
#include <iostream>
#include <ogre.h>
#include <map>
typedef std::map< std::string, Ogre::TexturePtr> MAP_OGRE_TEXTURE;
class VTFWMesh : public WaterInterface,
					public Ogre::CompositorInstance::Listener,
					public Ogre::RenderTargetListener,
					public Ogre::RenderQueue::RenderableListener
{
private:
	Ogre::MeshPtr											mesh ;
	Ogre::SubMesh *											subMesh ; 
	//MAP_OGRE_TEXTURE										mTexture;
	Ogre::HardwarePixelBufferSharedPtr						mHeightBuf;
	Ogre::HardwarePixelBufferSharedPtr						mPreviousHeightBuf;
	float *													vertexBuffers[3] ; // we need 3 vertex buffers
	int														currentBuffNumber ;
	int														complexity ;
	std::string												meshName ;
	int														numFaces ;
	int														numVertices ;
	Ogre::Vector3*											vNormals ;

	Ogre::HardwareVertexBufferSharedPtr						posVertexBuffer ;
	Ogre::HardwareVertexBufferSharedPtr						normVertexBuffer ;
	Ogre::HardwareVertexBufferSharedPtr						texcoordsVertexBuffer ;
	Ogre::HardwareIndexBufferSharedPtr						indexBuffer ;
	Ogre::PixelBox*											m_pPixelBox;
	//Ogre::PixelBox*											m_pPrePixelBox;
	Ogre::Image::Box										m_imageBox;
	Ogre::RenderWindow*										m_pWindow;

	float													lastTimeStamp ;
	float													lastAnimationTimeStamp;
	float													lastFrameTime ;
public:
															VTFWMesh(const std::string& meshName, float planeSize, int complexity, Ogre::RenderWindow *win) ;
    virtual													~VTFWMesh ();
	virtual void											updateMesh(float timeSinceLastFrame);
	virtual void											push(float x, float y, float depth, bool absolute=false);
	std::string												getType(void) {return "VTFW_MESH";}
	//Ogre::TexturePtr										getTexture(const std::string &name);
private:
	//virtual void											preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);
	//virtual void											postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);
	// Implementation of Ogre::CompositorInstance::Listener
	virtual void											notifyMaterialSetup(Ogre::uint32 passId, Ogre::MaterialPtr& material);

	// Implementation of Ogre::RenderTargetListener
	virtual void											preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);
	virtual void											postViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);

	// Implementation of Ogre::RenderQueue::RenderableListener
	virtual bool											renderableQueued(Ogre::Renderable* rend, Ogre::uint8 groupID, 
																Ogre::ushort priority, Ogre::Technique** ppTech, Ogre::RenderQueue* pQueue);
};

#endif //_VTFW_MESH_H_
