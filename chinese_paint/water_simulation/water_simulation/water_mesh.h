#ifndef _WATER_MESH_H_
#define _WATER_MESH_H_

#include "water_interface.h"
#include <iostream>
#include <ogre.h>

enum SIMULATION_MODE
{
	eOGREMode=0,
	eVTFWMode
};
class WaterMesh : public WaterInterface
{
private:
	Ogre::MeshPtr mesh ;
	Ogre::SubMesh *subMesh ; 
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

	float lastTimeStamp ;
	float lastAnimationTimeStamp;
	float lastFrameTime ;
	
	void calculateFakeNormals();
	void calculateNormals();

	void ogreSimulation(float timepass);
	void vtfwSimulation(float timepass);
public:
	WaterMesh(const std::string& meshName, float planeSize, int complexity) ;
    
    virtual ~WaterMesh ();


	/** "pushes" a mesh at position [x,y]. Note, that x,y are float, hence 
	*	4 vertices are actually pushed
	*	@note 
	*		This should be replaced by push with 'radius' parameter to simulate
	*  		big objects falling into water
	*		x和y不可以大於complexity
	*/
	void push(float x, float y, float depth, bool absolute=false) ;

	/** gets height at given x and y, takes average value of the closes nodes */
	float getHeight(float x, float y);

	/** updates mesh */
	void updateMesh(float timeSinceLastFrame) ;
	
	float PARAM_C ; // ripple speed 
	float PARAM_D ; // distance
	float PARAM_U ; // viscosity
	float PARAM_T ; // time
	bool useFakeNormals ;
	SIMULATION_MODE m_eSimulationMode;
};

#endif //_WATER_MESH_H_