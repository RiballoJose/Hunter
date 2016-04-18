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

  _numBall = 0;
  _fuerza = MIN_FUERZA;

  createScene();
  createOverlay();
  createInitialWorld();
  
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
  _camera->setPosition(Ogre::Vector3(-10, 25, 80));
  _camera->lookAt(Ogre::Vector3(3, 0, 5));

  _sceneMgr->setSkyDome(true, "Sky", 5, 8);

   nodo = _sceneMgr->getRootSceneNode()->createChildSceneNode(
    "Duck", Ogre::Vector3(-2, 6, 20));
  ent = _sceneMgr->createEntity("Duck.mesh");
   nodo->attachObject(ent);

/* Cáñon */
  _nodBase = _sceneMgr->getRootSceneNode()->createChildSceneNode(
    "Base", Ogre::Vector3(-2, 0.001, 45));
  ent = _sceneMgr->createEntity("Cannion_Base.mesh");
  _nodBase->attachObject(ent);

  _nodCannion = _nodBase->createChildSceneNode("Cannion", 
    Ogre::Vector3(0, 1, 0));
  ent = _sceneMgr->createEntity("Cannion.mesh");
  _nodCannion->attachObject(ent);

  _nodShoot = _nodCannion->createChildSceneNode("Disparo",
    Ogre::Vector3(0, 1.7, -2));

  /* Fisica */
  _debugDrawer = new OgreBulletCollisions::DebugDrawer();
  _debugDrawer->setDrawWireframe(true);  
  nodo = _sceneMgr->getRootSceneNode()->createChildSceneNode(
    "debugNode", Ogre::Vector3::ZERO);
  nodo->attachObject(static_cast<Ogre::SimpleRenderable *>(_debugDrawer));

  Ogre::AxisAlignedBox worldBounds = Ogre::AxisAlignedBox (
    Ogre::Vector3 (-10000, -10000, -10000), 
    Ogre::Vector3 (10000,  10000,  10000));
  Ogre::Vector3 gravity = Ogre::Vector3(0, -9.8, 0);

  _world = new OgreBulletDynamics::DynamicsWorld(_sceneMgr,
     worldBounds, gravity);
  _world->setDebugDrawer (_debugDrawer);
  _world->setShowDebugShapes (false);  // Muestra los collision shapes

  std::cout << "1" << std::endl;

 
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

void PlayState::createInitialWorld() {
  

 /* Creacion de la entidad y del SceneNode */
  Ogre::Plane plane1(Ogre::Vector3::UNIT_Y, 0);
  Ogre::MeshManager::getSingleton().createPlane("p1",
  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane1,
  500, 500, 1, 1, true, 1, 20, 20, Ogre::Vector3::UNIT_Z);
  Ogre::SceneNode* node = _sceneMgr->createSceneNode("Ground");
  Ogre::Entity* groundEnt = _sceneMgr->createEntity("Base", "p1");
  groundEnt->setCastShadows(false);
  groundEnt->setMaterialName("Ground");
  node->attachObject(groundEnt);
  _sceneMgr->getRootSceneNode()->addChild(node);

  std::cout << "2" << std::endl;

  /* Creamos forma de colision para el plano */ 
  OgreBulletCollisions::CollisionShape *Shape;
  Shape = new OgreBulletCollisions::StaticPlaneCollisionShape
    (Ogre::Vector3::UNIT_Y, 0);
  OgreBulletDynamics::RigidBody *rigidBodyPlane = new 
    OgreBulletDynamics::RigidBody("rigidBodyPlane", _world);

  /* Creamos la forma estatica (forma, Restitucion, Friccion) */
  rigidBodyPlane->setStaticShape(Shape, 0.1, 0.8); 
  
  /* Anadimos los objetos Shape y RigidBody */
  _shapes.push_back(Shape);      _bodies.push_back(rigidBodyPlane);

  
  std::cout << "3" << std::endl;
}

void PlayState::shoot(){
  Ogre::Entity* entity=NULL;
  Ogre::SceneNode* node=NULL;
  Ogre::Vector3 pos = Ogre::Vector3::ZERO;
  
  pos = _nodShoot->_getDerivedPosition();

  entity = _sceneMgr->createEntity("ball" + Ogre::StringConverter::toString(_numBall), "ball.mesh");
  node = _sceneMgr->getRootSceneNode()->
    createChildSceneNode();
  node->attachObject(entity);

  OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = NULL; 
  OgreBulletCollisions::CollisionShape *bodyShape = NULL;
  OgreBulletDynamics::RigidBody *rigidBody = NULL;

  trimeshConverter = new 
    OgreBulletCollisions::StaticMeshToShapeConverter(entity);
  bodyShape = trimeshConverter->createConvex();

  rigidBody = new OgreBulletDynamics::RigidBody("rigidBody" + Ogre::StringConverter::toString(_numBall), _world);

  rigidBody->setShape(node, bodyShape,
         0.6 /* Restitucion */, 0.6 /* Friccion */,
         5.0 /* Masa */, pos /* Posicion inicial */,
         Ogre::Quaternion::IDENTITY /* Orientacion */);

  rigidBody->setLinearVelocity(_dir * _fuerza);

  _numBall++;

  _shapes.push_back(bodyShape);   _bodies.push_back(rigidBody);

  std::cout << "4" << std::endl;

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

  if(!_exitGame){
    /* Movimiento del cañon */
    if(_derecha && _nodBase->getOrientation().zAxis().x > (-0.95)){
      _nodBase->yaw(Ogre::Degree(-180 * _deltaT));
    } else if(_izquierda && _nodBase->getOrientation().zAxis().x < (0.95)){
      _nodBase->yaw(Ogre::Degree(180 * _deltaT));
    } else if(_arriba && _nodCannion->getOrientation().zAxis().y > (-0.5)){
      _nodCannion->pitch(Ogre::Degree(180 * _deltaT));
    } else if(_abajo && _nodCannion->getOrientation().zAxis().y < (0.15)){
      _nodCannion->pitch(Ogre::Degree((-180) * _deltaT));
    }
   }

    /* Calculo de la direccion del disparo */
    _dir = _nodBase->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
    _dir.y = (_nodCannion->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z).y;

    /* Incremento de la fuerza de disparo */
    if(_shoot && (_fuerza <= MAX_FUERZA)){
      _fuerza += INC_FUERZA;
    }

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
  _deltaT = evt.timeSinceLastFrame;
  _world->stepSimulation(_deltaT);
  
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
  case OIS::KC_P:
    pushState(PauseState::getSingletonPtr());
    break;
  case OIS::KC_ESCAPE://overlay?
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
case OIS::KC_RIGHT:
		_derecha = true;
		break;
	case OIS::KC_LEFT:
		_izquierda = true;
		break;
	case OIS::KC_UP:
		_arriba = true;
		break;
	case OIS::KC_DOWN:
		_abajo = true;
		break;
	case OIS::KC_SPACE:
		_shoot = true;
		break;
  default:
    break;
  }
}
void
PlayState::keyReleased
(const OIS::KeyEvent &e)
{
  switch(e.key){
  	case OIS::KC_RIGHT:
      _derecha = false;
      break;
    case OIS::KC_LEFT:
      _izquierda = false;
      break;
    case OIS::KC_UP:
      _arriba = false;
      break;
    case OIS::KC_DOWN:
      _abajo = false;
      break;
    case OIS::KC_SPACE:
      _shoot = false;
      shoot();
      _fuerza = MIN_FUERZA;
      break;
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


