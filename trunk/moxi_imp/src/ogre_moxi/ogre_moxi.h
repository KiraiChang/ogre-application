#ifndef _OGRE_MOXI_H_
#define _OGRE_MOXI_H_

#include <ogre.h>
#include <string>

typedef std::map<std::string, Ogre::TexturePtr> M_TEXTURE;

class OgreMoxi : public Ogre::RenderTargetListener
{
private:
	M_TEXTURE								m_mapTexture;
protected:
	virtual void							createTexture(void);
public:
											OgreMoxi(Ogre::Viewport *viewport);
	virtual									~OgreMoxi(void);
	virtual void							updateTexture(const std::string &name, float u, float v, int r, int g, int b, int a);

};
#endif //_OGRE_MOXI_H_