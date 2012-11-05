#ifndef _PARTICLE_SIMULATION_
#define _PARTICLE_SIMULATION_
#include <Ogre.h>
#include <vector>

typedef std::vector<Ogre::Vector2> V_POINT;

class ParticleSimulation
{
public:
	virtual void update(Ogre::ParticleSystem *ps, float timePass, float *vU, float *vV, int gridNumber) = 0;
	virtual void initParticle(Ogre::ParticleSystem *ps, V_POINT vPoint) = 0;
};

class WaveParticle : ParticleSimulation
{
public:
	virtual void update(Ogre::ParticleSystem *ps, float timePass, float *vU, float *vV, int gridNumber);
	virtual void initParticle(Ogre::ParticleSystem *ps, V_POINT vPoint);
};
#endif //_PARTICLE_SIMULATION_
