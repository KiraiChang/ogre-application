#ifndef _OGRE_MESH_TOOL_
#define _OGRE_MESH_TOOL_

#include <ogre.h>

void getMeshInformation(Ogre::Entity* entity,
                        size_t &vertex_count,
                        Ogre::Vector3* &vertices,
                        size_t &index_count,
                        unsigned long* &indices,
                        const Ogre::Vector3 &position,
                        const Ogre::Quaternion &orient,
                        const Ogre::Vector3 &scale);

#endif //_OGRE_MESH_TOOL_