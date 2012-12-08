#include "ogre_moxi.h"

#define MOXI_MATERIAL_NAME "ChinesePaint/Moxi"
#define BLOCK_MATERIAL_NAME "ChinesePaint/Moxi_Block"
#define TARGET_WIDTH 512
#define TARGET_HEIGHT 512
/*-----------------------------------------------------------------*/
/*--------------------------PROTECTED_FUN--------------------------*/
/*-----------------------------------------------------------------*/
void OgreMoxi::createTexture(void)
{
		/*
	texture VelDenMap 	target_width target_height PF_FLOAT16_RGBA  //[u, v, wf, seep]
	texture BlockMap 	target_width target_height PF_FLOAT16_RGBA  //[blk, f0, lwf, ws]
	texture Dist1Map 	target_width target_height PF_FLOAT16_RGBA  //f[N, E, W, S]
	texture Dist2Map 	target_width target_height PF_FLOAT16_RGBA  //f[NE, SE, NW, SW]
	texture FlowInkMap 	target_width target_height PF_FLOAT16_RGBA  //if[P1, P2, P3, glue]
	texture SurfInkMap 	target_width target_height PF_FLOAT16_RGBA  //is[P1, P2, P3, glue]
	texture FixInkMap 	target_width target_height PF_FLOAT16_RGBA  //ix[P1, P2, P3, fblk]
	texture StainMap 	target_width target_height PF_FLOAT16_RGBA  //[wpen, wpin, edge, -]
	texture SinkInkMap 	target_width target_height PF_FLOAT16_RGBA  //[P1, P2, P3, glue]
	texture DetInkMap 	target_width target_height PF_FLOAT16_RGBA  //[P1, P2, P3, wetcut]
	*/
	std::string textureName;
	Ogre::HardwarePixelBufferSharedPtr buffer;
	//textureName = "VelDenMap";
	//m_mapTexture[textureName] = Ogre::TextureManager::getSingleton().createManual(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	//	Ogre::TEX_TYPE_2D, TARGET_WIDTH, TARGET_HEIGHT, 0, Ogre::PF_R8G8B8A8, Ogre::TU_DYNAMIC_WRITE_ONLY);
	//textureName = "BlockMap";
	//m_mapTexture[textureName] = Ogre::TextureManager::getSingleton().createManual(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	//	Ogre::TEX_TYPE_2D, TARGET_WIDTH, TARGET_HEIGHT, 0, Ogre::PF_R8G8B8A8, Ogre::TU_DYNAMIC_WRITE_ONLY);
	//textureName = "Dist1Map";
	//m_mapTexture[textureName] = Ogre::TextureManager::getSingleton().createManual(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	//	Ogre::TEX_TYPE_2D, TARGET_WIDTH, TARGET_HEIGHT, 0, Ogre::PF_R8G8B8A8, Ogre::TU_DYNAMIC_WRITE_ONLY);
	//textureName = "Dist2Map";
	//m_mapTexture[textureName] = Ogre::TextureManager::getSingleton().createManual(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	//	Ogre::TEX_TYPE_2D, TARGET_WIDTH, TARGET_HEIGHT, 0, Ogre::PF_R8G8B8A8, Ogre::TU_DYNAMIC_WRITE_ONLY);
	textureName = "FlowInkMap";
	m_mapTexture[textureName] = Ogre::TextureManager::getSingleton().createManual(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, TARGET_WIDTH, TARGET_HEIGHT, 0, Ogre::PF_R8G8B8A8, Ogre::TU_DYNAMIC_WRITE_ONLY);
	//textureName = "SurfInkMap";
	//m_mapTexture[textureName] = Ogre::TextureManager::getSingleton().createManual(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	//	Ogre::TEX_TYPE_2D, TARGET_WIDTH, TARGET_HEIGHT, 0, Ogre::PF_R8G8B8A8, Ogre::TU_DYNAMIC_WRITE_ONLY);
	//textureName = "FixInkMap";
	//m_mapTexture[textureName] = Ogre::TextureManager::getSingleton().createManual(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	//	Ogre::TEX_TYPE_2D, TARGET_WIDTH, TARGET_HEIGHT, 0, Ogre::PF_R8G8B8A8, Ogre::TU_DYNAMIC_WRITE_ONLY);
	//textureName = "StainMap";
	//m_mapTexture[textureName] = Ogre::TextureManager::getSingleton().createManual(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	//	Ogre::TEX_TYPE_2D, TARGET_WIDTH, TARGET_HEIGHT, 0, Ogre::PF_R8G8B8A8, Ogre::TU_DYNAMIC_WRITE_ONLY);
	//textureName = "SinkInkMap";
	//m_mapTexture[textureName] = Ogre::TextureManager::getSingleton().createManual(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	//	Ogre::TEX_TYPE_2D, TARGET_WIDTH, TARGET_HEIGHT, 0, Ogre::PF_R8G8B8A8, Ogre::TU_DYNAMIC_WRITE_ONLY);
	//textureName = "DetInkMap";
	//m_mapTexture[textureName] = Ogre::TextureManager::getSingleton().createManual(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	//	Ogre::TEX_TYPE_2D, TARGET_WIDTH, TARGET_HEIGHT, 0, Ogre::PF_R8G8B8A8, Ogre::TU_DYNAMIC_WRITE_ONLY);

	M_TEXTURE::iterator ite;
	Ogre::TexturePtr ptr;
	for(ite = m_mapTexture.begin(); ite != m_mapTexture.end();++ite)
	{
		buffer = ite->second->getBuffer();  // save off the texture buffer
		// initialise the texture to have full luminance
		buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
		//memset(buffer->getCurrentLock().data, 1.0, buffer->getSizeInBytes());
		for(int j = 0; j < buffer->getHeight();j++)
		{
			for(int i = 0; i < buffer->getWidth(); i++)
			{
				int index = i * 4 + j * buffer->getWidth() * 4;
				((Ogre::uint8 *)buffer->getCurrentLock().data)[index] = 0xff;
				((Ogre::uint8 *)buffer->getCurrentLock().data)[index+1] = 0xff;
				((Ogre::uint8 *)buffer->getCurrentLock().data)[index+2] = 0xff;
				((Ogre::uint8 *)buffer->getCurrentLock().data)[index+3] = 0xff;
			}
		}
		buffer->unlock();
	}
}
/*-----------------------------------------------------------------*/
/*----------------------------PUBLIC_FUN---------------------------*/
/*-----------------------------------------------------------------*/
OgreMoxi::OgreMoxi(Ogre::Viewport *viewport)
{
	createTexture();
	Ogre::CompositorManager::getSingleton().addCompositor(viewport, MOXI_MATERIAL_NAME);
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, MOXI_MATERIAL_NAME, true);
	//init();
}

OgreMoxi::~OgreMoxi(void)
{

}

void OgreMoxi::updateTexture(const std::string &name, float u, float v, int r, int g, int b, int a)
{
	assert(m_mapTexture.count(name) != 0);
	int x = u * m_mapTexture[name]->getSrcWidth();
	int y = v * m_mapTexture[name]->getSrcHeight();
	int index = y * m_mapTexture[name]->getSrcWidth() * 4 + x * 4;
	Ogre::HardwarePixelBufferSharedPtr buffer = m_mapTexture[name]->getBuffer();

	buffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);

	Ogre::uint8* data = (Ogre::uint8*)buffer->getCurrentLock().data;
	data[index] = r;
	data[index+1] = g;
	data[index+2] = b;
	data[index+3] = a;

	buffer->unlock();
}
/*-----------------------------------------------------------------*/
/*-------------------------------END-------------------------------*/
/*-----------------------------------------------------------------*/