#ifndef _VTFW_MESH_H_
#define _VTFW_MESH_H_
#include "../simulation_interface/water_interface.h"
#include <iostream>
#include <ogre.h>

class VTFWMesh : public WaterInterface
{
private:
	Ogre::MeshPtr mesh ;
	Ogre::SubMesh *subMesh ; 
	Ogre::HardwarePixelBufferSharedPtr mHeightBuf;
	Ogre::HardwarePixelBufferSharedPtr mPreviousHeightBuf;
	float *vertexBuffers[3] ; // we need 3 vertex buffers
	int currentBuffNumber ;
	int complexity ;
	std::string meshName ;
	int numFaces ;
	int numVertices ;
	Ogre::Vector3* vNormals ;

	Ogre::HardwareVertexBufferSharedPtr posVertexBuffer ;
	Ogre::HardwareVertexBufferSharedPtr normVertexBuffer ;
	Ogre::HardwareVertexBufferSharedPtr texcoordsVertexBuffer ;
	Ogre::HardwareIndexBufferSharedPtr indexBuffer ;
	Ogre::PixelBox*											m_pPixelBox;

	float													lastTimeStamp ;
	float													lastAnimationTimeStamp;
	float													lastFrameTime ;
public:
															VTFWMesh(const std::string& meshName, float planeSize, int complexity) ;
    virtual													~VTFWMesh ();
	virtual void											updateMesh(float timeSinceLastFrame);
	virtual void											push(float x, float y, float depth, bool absolute=false);
	std::string												getType(void) {return "VTFW_MESH";}
};

#endif //_VTFW_MESH_H_
