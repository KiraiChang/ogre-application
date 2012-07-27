#ifndef STABLE_FLUIDS_MESH_H_
#define STABLE_FLUIDS_MESH_H_

#include "water_interface.h"
#include <iostream>
#include <ogre.h>

class StableFluidsGrid;

class StableFluidsMesh : public WaterInterface
{
private:
	StableFluidsGrid*						m_pGrid;
	Ogre::MeshPtr							mesh ;
	Ogre::SubMesh *							subMesh ; 
	float *									vertexBuffers[3] ; // we need 3 vertex buffers
	int										currentBuffNumber ;
	int										complexity ;
	int										planeSize;
	std::string								meshName ;
	int										numFaces ;
	int										numVertices ;
	Ogre::Vector3*							vNormals ;

	Ogre::HardwareVertexBufferSharedPtr		posVertexBuffer ;
	Ogre::HardwareVertexBufferSharedPtr		normVertexBuffer ;
	Ogre::HardwareVertexBufferSharedPtr		texcoordsVertexBuffer ;
	Ogre::HardwareIndexBufferSharedPtr		indexBuffer ;

	float									lastTimeStamp ;
	float									lastAnimationTimeStamp;
	float									lastFrameTime ;

private:
	void									calculateFakeNormals();
	void									calculateNormals();
public:
											StableFluidsMesh(const std::string& meshName, float planeSize, int complexity) ;
    
    virtual									~StableFluidsMesh ();
	void									push(float x, float y, float depth, bool absolute=false) ;								
	/** updates mesh */
	void									updateMesh(float timeSinceLastFrame) ;
};

#endif //STABLE_FLUIDS_MESH_H_