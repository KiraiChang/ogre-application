#include "particle_simulation.h"

#define PARTICLE_MOVE_SPEED 25
#define PARTICLE_LIVE_TIME 10
#define PARTICLE_SIZE_X 0.5
#define PARTICLE_SIZE_Y 0.5
#define SCALE 1
#define IX(i,j,k) ((i)+(k+2)*(j))

void WaveParticle::update(Ogre::ParticleSystem *ps, float timePass, float *vU, float *vV, int gridNumber)
{
	Ogre::Particle* particle = NULL;
	unsigned int index;
	float u0, u1, v0, v1, u, v;
	int x0, x1, y0, y1;
	Ogre::ParticleIterator ite = ps->_getIterator();
	while(!ite.end())
	{
		particle = ite.getNext();
		Ogre::Vector3 pos = particle->position;
		x0 = (int)(pos.x-0.5); y0 = (int)(pos.z-0.5);
		x1 = x0 + 1; y1 = y0 + 1;
		u1 = (pos.x-0.5)-x0; u0 = 1-u1; v1 = (pos.z-0.5)-y0; v0 = 1-v1;
		u = u0*(v0*vU[IX(x0,y0,gridNumber)]+v1*vV[IX(x0,y1,gridNumber)])+
			u1*(v0*vU[IX(x1,y0,gridNumber)]+v1*vV[IX(x1,y1,gridNumber)]);

		v = u0*(v0*vU[IX(x0,y0,gridNumber)]+v1*vV[IX(x0,y1,gridNumber)])+
			u1*(v0*vU[IX(x1,y0,gridNumber)]+v1*vV[IX(x1,y1,gridNumber)]);

		pos.x += u * timePass * PARTICLE_MOVE_SPEED; pos.z += v * timePass * PARTICLE_MOVE_SPEED;
		particle->position = pos;
	}
}

void WaveParticle::process(V_POINT &vPoint, Ogre::Vector2 &center)
{
	Ogre::Vector2 point;
	center.x = 0;
	center.y = 0;
	V_POINT vResult;
	Ogre::Vector2 scale;
	size_t size = vPoint.size();
	for(int i = 0; i < size; i++)
	{
		center += vPoint[i];
	}
	center /= size;
	for(int i = 0; i < size; i++)
	{
		point = vPoint[i];
		point = (point-center) * SCALE + center;
		vPoint[i] = point;
	}
}

void WaveParticle::initParticle(Ogre::ParticleSystem *ps, V_POINT vPoint)
{
	Ogre::Particle* particle = NULL;
	Ogre::Vector2 p1, p2, dist;
	size_t size = vPoint.size();
	for(int i = 0; i < size; i++)
	{
		p1 = vPoint[i];
		p2 = vPoint[(i+1)%size];

		particle = ps->createParticle();
		if(particle != NULL)
		{
			particle->position.x = p1.x;
			particle->position.y = 0;
			particle->position.z = p1.y;
			particle->timeToLive = PARTICLE_LIVE_TIME;
			particle->setDimensions (PARTICLE_SIZE_X, PARTICLE_SIZE_Y);
		}

		while(p2.distance(p1) > 1.0)
		{
			dist = (p2 - p1).normalisedCopy();
			p1 += dist;

			particle = ps->createParticle();
			if(particle != NULL)
			{
				particle->position.x = p1.x;
				particle->position.y = 0;
				particle->position.z = p1.y;
				particle->timeToLive = PARTICLE_LIVE_TIME;
				particle->setDimensions (PARTICLE_SIZE_X, PARTICLE_SIZE_Y);
			}
		}
		particle = ps->createParticle();
		if(particle != NULL)
		{
			particle->position.x = p2.x;
			particle->position.y = 0;
			particle->position.z = p2.y;
			particle->timeToLive = PARTICLE_LIVE_TIME;
			particle->setDimensions (PARTICLE_SIZE_X, PARTICLE_SIZE_Y);
		}
	}
}

