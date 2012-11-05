#ifndef _MAPPING_ALGORITHM_
#define _MAPPING_ALGORITHM_

#include <ogre.h>
#include <vector>
typedef std::vector<Ogre::Vector2> V_POINT;

class MappingAlgorithm
{
public:
	virtual V_POINT process(size_t verticesCount, Ogre::Vector3 *vertices, float *grid, size_t gridNumber) = 0;
};

class Normal : MappingAlgorithm
{
public:
	virtual V_POINT process(size_t verticesCount, Ogre::Vector3 *vertices, float *grid, size_t gridNumber);
};

class ConvexHull : MappingAlgorithm
{
public:
	virtual V_POINT process(size_t verticesCount, Ogre::Vector3 *vertices, float *grid, size_t gridNumber);
};

#endif //_MAPPING_ALGORITHM_