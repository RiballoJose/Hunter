#include "PlayState.h"
#include "PauseState.h"
#include "MenuState.h"
template<> PlayState* Ogre::Singleton<PlayState>::msSingleton = 0;

using namespace Ogre::OverlayElementCommands;

void
PlayState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();
  _sceneMgr = _root->getSceneManager("SceneManager");
  _sceneMgr->addRenderQueueListener(GameManager::getSingletonPtr()->getOverlaySystem());
  _overlayManager = Ogre::OverlayManager::getSingletonPtr();
  
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _light = _sceneMgr->createLight("Light");

  _pTrackManager = TrackManager::getSingletonPtr();
  _pSoundFXManager = SoundFXManager::getSingletonPtr();

  createScene();
  createOverlay();
  
  _mainTrack->play();

  _exitGame = false;
}
void 
PlayState::createOverlay()
{
  _ovJuego = _overlayManager->getByName("Juego");
  _ovPunt = _overlayManager->getOverlayElement("Puntuacion");
  _ovVida = _overlayManager->getOverlayElement("Vida");
  _ovScore = _overlayManager->getOverlayElement("Puntos");
  _ovVida->setCaption("Vidas");
  _ovPunt->setCaption("Puntos");
  _ovJuego->show();
}
void
PlayState::createScene()
{
  Ogre::Entity* ent = NULL;
  Ogre::SceneNode* nodo = NULL;
  std::stringstream bloq, material;
  bloq.str("");

  _mainTrack = _pTrackManager->load("Background.ogg");
  _simpleEffect = _pSoundFXManager->load("Efecto.ogg");

  
  _perspective = 0;
  _camera->setPosition(Ogre::Vector3(0, 42, 7));
  _camera->lookAt(Ogre::Vector3(0, -50, 0));

  _sceneMgr->setSkyDome(true, "Sky", 5, 8);
  
  nodo = _sceneMgr->getRootSceneNode()->createChildSceneNode("Escenario", Ogre::Vector3(-0.5,0.0,3.0));
  ent = _sceneMgr->createEntity("Base.mesh");
  nodo->attachObject(ent);

 
  _sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
  _sceneMgr->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5) );
  _sceneMgr->setAmbientLight(Ogre::ColourValue(0.9, 0.9, 0.9));
  _sceneMgr->setShadowTextureCount(2);
  _sceneMgr->setShadowTextureSize(512);
  
  _light->setType(Ogre::Light::LT_SPOTLIGHT);
  _light->setDirection(Ogre::Vector3(0,-1,0));
  _light->setSpotlightInnerAngle(Ogre::Degree(25.0f));
  _light->setSpotlightOuterAngle(Ogre::Degree(200.0f));
  _light->setPosition(0, 150, 0);
  _light->setSpecularColour(0.8, 0.8, 0.8);
  _light->setDiffuseColour(0.8, 0.8, 0.8);
  _light->setSpotlightFalloff(5.0f);
  _light->setCastShadows(true);
}

void
PlayState::exit ()
{
  _ovJuego->hide(); 
  _mainTrack->stop();
  _sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();
}

void
PlayState::pause()
{
}

void
PlayState::resume()
{
  if(_exitGame){changeState(MenuState::getSingletonPtr());}
}

bool
PlayState::frameStarted
(const Ogre::FrameEvent& evt)
{
  _deltaT = evt.timeSinceLastFrame;
  _score = 0;
  _ovScore->setCaption(Ogre::StringConverter::toString(_score));
  if(!_exitGame){
  }
  else{return false;}
  return true;
}


void
PlayState::destroyAllAttachedMovableObjects(Ogre::SceneNode* node)
{
   if(!node) return;

   Ogre::SceneNode::ObjectIterator itObject = node->getAttachedObjectIterator();

   while (itObject.hasMoreElements()){
      node->getCreator()->destroyMovableObject(itObject.getNext());
   }

   Ogre::SceneNode::ChildNodeIterator itChild = node->getChildIterator();

   while (itChild.hasMoreElements()){
      Ogre::SceneNode* pChildNode = static_cast<Ogre::SceneNode*>(itChild.getNext());
      destroyAllAttachedMovableObjects(pChildNode);
   }
}


bool
PlayState::frameEnded
(const Ogre::FrameEvent& evt)
{
  if (_exitGame){
    //std::cout << _score << std::endl;
    EndState::getSingletonPtr()->addScore(_score);
    pushState(EndState::getSingletonPtr());
    //return false;
  }
  return true;
}

void
PlayState::keyPressed
(const OIS::KeyEvent &e)
{
  switch(e.key){
  case OIS::KC_SPACE:
    pushState(PauseState::getSingletonPtr());
    break;
  case OIS::KC_ESCAPE://overlay?
    break;
  case OIS::KC_P:
    break;
  case OIS::KC_C:
      _perspective = (_perspective+1) % 2;
      switch(_perspective){
      case 0:
	//Vista aerea
	_camera->setPosition(Ogre::Vector3(0, 42, 7));
	_camera->lookAt(Ogre::Vector3(0, -50, 0));
	break;
      case 1:
	//vista 3D
	_camera->setPosition(Ogre::Vector3(0, 32, 37));
	_camera->lookAt(Ogre::Vector3(0, 0, 0));
	break;
      }
      break;
/*  case OIS::KC_RIGHT:
    _simpleEffect->play();
    _pacman->lookAt(Ogre::Vector3(0,0,-999), _pacman->TS_WORLD);
    _prevDir = _currentDir;
    _currentDir = 1;
    _nextDir = 1;
    break;
  case OIS::KC_LEFT:
    _simpleEffect->play();
    _pacman->lookAt(Ogre::Vector3(0,0,999), _pacman->TS_WORLD);
    _prevDir = _currentDir;
    _currentDir = 3;
    _nextDir = 3;
    break;
  case OIS::KC_UP:
    _simpleEffect->play();
    _pacman->lookAt(Ogre::Vector3(-999,0,0), _pacman->TS_WORLD);
    _prevDir = _currentDir;
    _currentDir = 4;
    _nextDir = 4;
    break;
  case OIS::KC_DOWN:
    _simpleEffect->play();
    _pacman->lookAt(Ogre::Vector3(999,0,0), _pacman->TS_WORLD);
    _prevDir = _currentDir;
    _currentDir = 2;
    _nextDir = 2;
    break;
 */
  default:
    break;
  }
}
void
PlayState::keyReleased
(const OIS::KeyEvent &e)
{
  switch(e.key){
  case OIS::KC_ESCAPE:
    _exitGame = true;
    break;
  default:
    break;
  }
}

PlayState*
PlayState::getSingletonPtr ()
{
return msSingleton;
}

PlayState&
PlayState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}


