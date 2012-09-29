#ifndef _SOLID_MESH_
#define _SOLID_MESH_

#include <ogre.h>
#define MAX_MAP_COUNT 3
class SolidMesh
{
private:
	Ogre::SceneManager *					m_pSceneMgr;
	Ogre::Entity *							m_pEnt;
	Ogre::SceneNode *						m_pNode;
	Ogre::AnimationState*					m_pAniState;
	size_t									m_sVertexCount;
	size_t									m_sIndexCount;
	int										m_iCurrentMap ;
	Ogre::Vector3*							m_vVertices[MAX_MAP_COUNT];
	unsigned long*							m_vIndices;//[MAX_MAP_COUNT];

public:
											SolidMesh(const std::string &solidName, 
												const std::string &meshName, 
												Ogre::SceneManager *mgr);
	virtual									~SolidMesh(void);
	virtual void							init(const std::string &solidName, 
												const std::string &meshName);
	virtual void							release(void);
	virtual void							update(float timePass);
	void									setAnimation(const std::string &aniName, bool enable = true, bool loop = true);
	size_t									getVertexCount(void);
	size_t									getIndexCount(void);
	Ogre::Vector3*							getVertices(void);
	Ogre::Vector3*							getPrevVertices(void);
	unsigned long*							getIndices(void);
	//unsigned long*							getPrevIndices(void);
	Ogre::SceneNode *						getNode(void);
	
};

#endif //_SOLID_MESH_
