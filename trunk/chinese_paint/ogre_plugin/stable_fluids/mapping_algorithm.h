#ifndef _MAPPING_ALGORITHM_
#define _MAPPING_ALGORITHM_

namespace Ogre
{
	class Vector3;
	class Vector2;
}

class MappingAlgorithm
{
public:
	virtual void process(size_t verticesCount, Ogre::Vector3 *vertices, float *grid, size_t gridNumber) = 0;
};

class Normal : MappingAlgorithm
{
public:
	virtual void process(size_t verticesCount, Ogre::Vector3 *vertices, float *grid, size_t gridNumber);
};

class ConvexHull : MappingAlgorithm
{
public:
	virtual void process(size_t verticesCount, Ogre::Vector3 *vertices, float *grid, size_t gridNumber);
};

#endif //_MAPPING_ALGORITHM_