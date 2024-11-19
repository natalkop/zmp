#ifndef SCENE_HH
#define SCENE_HH

#include "AbstractScene.hh"

class Scene: public AbstractScene {
	public:
		Scene();
		virtual AbstractMobileObj*  FindMobileObj(const char *sName) override;
		virtual void AddMobileObj(AbstractMobileObj *pMobObj) override;

};

#endif
