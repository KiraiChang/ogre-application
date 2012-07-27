#ifndef _VTFW_MESH_H_
#define _VTFW_MESH_H_
#include "water_interface.h"
#include <iostream>
#include <ogre.h>

class VTFWMesh : public WaterInterface
{
private:
	Ogre::MeshPtr											mesh ;
	Ogre::SubMesh *											subMesh ; 
	std::string												meshName ;
	Ogre::HardwareVertexBufferSharedPtr						posVertexBuffer ;
	Ogre::HardwareIndexBufferSharedPtr						indexBuffer ;

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
