//StiControlBar.cxx
//M.L. Miller (Yale Software)
//04/01

#include <iostream>

#include "StChain.h"
#include "TControlBar.h"
#include "StiMaker.h"

//Sti
#include "Sti/StiDetector.h"
#include "Sti/StiDetectorContainer.h"

//StiGui
#include "StiGui/StiRootDrawableDetector.h"
#include "StiGui/StiDisplayManager.h"

//StiMaker
#include "StiControlBar.h"

int StiControlBar::mevent = 0;
StChain* StiControlBar::mchain = 0;

ClassImp(StiControlBar)

StiControlBar::StiControlBar()
{
    cout <<"StiControlBar::StiControlBar()"<<endl;
    mbar = makeControlBar();
}

StiControlBar::~StiControlBar()
{
    cout <<"StiControlBar::~StiControlBar()"<<endl;
}

void StiControlBar::resetStiGuiForEvent()
{
    //cout <<"StiControlBar::resetStiGuiForEvent()"<<endl;
    StiControlBar::setCurrentDetectorToDefault();
    StiMaker::instance()->reset();
    StiDisplayManager::instance()->draw();
    StiDisplayManager::instance()->update();
    //cout <<"\t Leaving StiControlBar::resetStiGuiForEvent()"<<endl;
}

void StiControlBar::doNextStiGuiAction()
{
    //cout <<"StiControlBar::doNextStGuiAction()"<<endl;
    StiMaker::instance()->doNextAction();
    //cout <<"\t Leaving StiControlBar::doNextStGuiAction()"<<endl;
}

void StiControlBar::stepToNextEvent()
{
    mchain->Make();
    ++mevent;
}

void StiControlBar::finish()
{
    mchain->Finish();
    
    //delete this;
    //this=0;
}

void StiControlBar::setVisible()
{
    StiDisplayManager::instance()->setVisible();
    StiDisplayManager::instance()->draw();
    StiDisplayManager::instance()->update();
}

void StiControlBar::setInvisible()
{
    StiDisplayManager::instance()->setInvisible();
    StiDisplayManager::instance()->draw();
    StiDisplayManager::instance()->update();
}

void StiControlBar::setSvtVisible()
{
    StiDisplayManager::instance()->setSvtVisible();
    StiDisplayManager::instance()->draw();
    StiDisplayManager::instance()->update();
}

void StiControlBar::setSvtInvisible()
{
    StiDisplayManager::instance()->setSvtInvisible();
    StiDisplayManager::instance()->draw();
    StiDisplayManager::instance()->update();
}

void StiControlBar::setTpcVisible()
{
    StiDisplayManager::instance()->setTpcVisible();
    StiDisplayManager::instance()->draw();
    StiDisplayManager::instance()->update();
}

void StiControlBar::setTpcInvisible()
{
    StiDisplayManager::instance()->setTpcInvisible();
    StiDisplayManager::instance()->draw();
    StiDisplayManager::instance()->update();
}

void StiControlBar::printDisplayManager()
{
    StiDisplayManager::instance()->print();
}

void StiControlBar::printDetector()
{
    StiDetectorContainer::instance()->print();
}


void StiControlBar::setLayer()
{
    //cout <<"Function Not Currently Implemented"<<endl;
    StiControlBar::setCurrentDetectorToDefault();
    cout <<"\nEnter position: (double)"<<endl;
    double position;
    cin >>position;
    cout <<"Setting to  position:\t"<<position<<endl;
    StiDetectorContainer& rdet = *(StiDetectorContainer::instance());
    rdet.setToDetector(position);
    StiDetector* layer = *rdet;
    if (!layer) {
	cout <<"Error in setSectorAndPadrow"<<endl;
	return;
    }
    cout <<"Detector Set To: "<<layer->getName()<<endl;
    StiControlBar::showCurrentDetector();
}

void StiControlBar::setLayerAndAngle()
{
    //cout <<"Function Not Currently Implemented"<<endl;
    StiControlBar::setCurrentDetectorToDefault();
    cout <<"\nEnter position: (double)"<<endl;
    double position;
    cin >>position;

    cout <<"\nEnter angle: (double)"<<endl;
    double angle;
    cin >> angle;
    cout <<"Setting to  position:\t"<<position<<"\tangle:\t"<<angle<<endl;

    StiDetectorContainer& rdet = *(StiDetectorContainer::instance());
    rdet.setToDetector(position, angle);
    StiDetector* layer = *rdet;
    if (!layer) {
	cout <<"Error in setSectorAndPadrow"<<endl;
	return;
    }
    cout <<"Detector Set To: "<<layer->getName()<<endl;
    StiControlBar::showCurrentDetector();
}

void StiControlBar::moveOut()
{
    //cout <<"Function Not Currently Implemented"<<endl;
    //cout <<"StiControlBar::moveOut()"<<endl;
    StiControlBar::setCurrentDetectorToDefault();
    StiDetectorContainer& rdet = *(StiDetectorContainer::instance());
    rdet.moveOut();
    StiControlBar::showCurrentDetector();
    //cout <<"\t Leaving StiControlBar::moveOut()"<<endl;
}

void StiControlBar::moveIn()
{
    //cout <<"Function Not Currently Implemented"<<endl;
    //cout <<"StiControlBar::moveIn()"<<endl;
    StiControlBar::setCurrentDetectorToDefault();
    StiDetectorContainer& rdet = *(StiDetectorContainer::instance());
    rdet.moveIn();
    StiControlBar::showCurrentDetector();
    //cout <<"\t Leaving StiControlBar::moveIn()"<<endl;
}
            
void StiControlBar::movePlusPhi()
{
    //cout <<"Function Not Currently Implemented"<<endl;
    StiControlBar::setCurrentDetectorToDefault();
    StiDetectorContainer& rdet = *(StiDetectorContainer::instance());
    rdet.movePlusPhi();
    StiControlBar::showCurrentDetector();
}

void StiControlBar::moveMinusPhi()
{
    //cout <<"Function Not Currently Implemented"<<endl;
    StiControlBar::setCurrentDetectorToDefault();
    StiDetectorContainer& rdet = *(StiDetectorContainer::instance());
    rdet.moveMinusPhi();
    StiControlBar::showCurrentDetector();
}

void StiControlBar::showCurrentDetector()
{
    //cout <<"Function Not Currently Implemented"<<endl;
    StiDetectorContainer& rdet = *(StiDetectorContainer::instance());
    StiRootDrawableDetector* layer = dynamic_cast<StiRootDrawableDetector*>(*rdet);
    
    if (!layer) {
	cout <<"Error! StiControlBar::showCurrentDetector(): Failed to get drawable detector"<<endl;
	return;
    }
    layer->setVisibility(true);
    layer->setColor(2);
    
    cout<<*layer<<endl;
    StiDisplayManager::instance()->draw();
    StiDisplayManager::instance()->update();
    
    return;
}

void StiControlBar::setCurrentDetectorToDefault()
{
    //cout <<"Function Not Currently Implemented"<<endl;
    //cout <<"StiControlBar::setCurrentDetectorToDefault()"<<endl;
    StiDetectorContainer& rdet = *(StiDetectorContainer::instance());
    StiRootDrawableDetector* layer = dynamic_cast<StiRootDrawableDetector*>(*rdet);
    if (!layer) {
	cout <<"Error! StiControlBar::setCurrentDetectorToDefault(): Failed to get drawable detector"<<endl;
	return;
    }
    layer->setColor(1);
    //cout <<"\t Leaving StiControlBar::setCurrentDetectorToDefault()"<<endl;
}

TControlBar* StiControlBar::makeControlBar()
{
    TControlBar* bar = new TControlBar("vertical","Sti Control Panel");
    
    //Add Buttons:
    bar->AddButton("Dump Display Manager","StiControlBar::printDisplayManager()",
		   "Show contents of the Display Manager");
    bar->AddButton("Dump Detector","StiControlBar::printDetector()","Show contents of Detector Container");
    bar->AddSeparator();
    
    bar->AddButton("All Visible","StiControlBar::setVisible()","Set All Drawables to Visible State");
    bar->AddButton("All Invisible","StiControlBar::setInvisible()","Set All Drawables to Invisible State");
    
    bar->AddButton("Svt Visible","StiControlBar::setSvtVisible()","Set Svt Drawables to Visible State");    
    bar->AddButton("Svt Invisible","StiControlBar::setSvtInvisible()","Set Svt Drawables to Invisible State");
    
    bar->AddButton("Tpc Visible","StiControlBar::setTpcVisible()","Set Tpc Drawables to Visible State");    
    bar->AddButton("Tpc Invisible","StiControlBar::setTpcInvisible()","Set Tpc Drawables to Invisible State");
    
    bar->AddButton("Reset", "StiControlBar::resetStiGuiForEvent()","Reset Sti For Next Event");
    bar->AddButton("Step",  "StiControlBar::doNextStiGuiAction()","Step Through Next Action");
    bar->AddButton("Event Step","StiControlBar::stepToNextEvent()","Step Through to Next Event");

    //Detector navigation
    bar->AddButton("Show Current Detector","StiControlBar::showCurrentDetector()","Highlight Current Detector");
    
    bar->AddButton("Move Out","StiControlBar::moveOut()","Step Radially to Next Padrow Out");
    bar->AddButton("Move In","StiControlBar::moveIn()","Step Radially to Next Padrow In");
    
    bar->AddButton("Move Around (Plus)","StiControlBar::movePlusPhi()","Step positive in global phi");
    bar->AddButton("Move Around (Minus)","StiControlBar::moveMinusPhi()","Step negative in global phi");

    bar->AddButton("Set Layer","StiControlBar::setLayer()","Set to given layer");
    bar->AddButton("Set Layer and Angle","StiControlBar::setLayerAndAngle()",
		   "Set to given layer, angle");

    //Chain management
    bar->AddSeparator();
    bar->AddButton("Finish","StiControlBar::finish()","Call StChain::Finish()");

    bar->Show();
    return bar;
};
