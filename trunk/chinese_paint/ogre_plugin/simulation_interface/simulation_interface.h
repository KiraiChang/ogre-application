#ifndef _SIMULATION_INTERFACE_H_
#define _SIMULATION_INTERFACE_H_

namespace OgreBites
{
	class Slider;
	class SdkTrayManager;
	class SelectMenu;
	class CheckBox;
}

class SimulationInterface
{
private:

public:
	virtual void init() = 0;
	virtual void release() = 0;
	virtual void setupControls(OgreBites::SdkTrayManager* sdkTray) = 0;
	virtual void sliderMoved(OgreBites::Slider* slider) = 0;
	virtual void checkBoxToggled(OgreBites::CheckBox* checkBox) = 0;
	virtual void itemSelected(OgreBites::SelectMenu* menu) = 0;
	virtual void update(float timePass) = 0;
};

#endif //_SIMULATION_INTERFACE_H_