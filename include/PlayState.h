/*********************************************************************
 * Módulo 1. Curso de Experto en Desarrollo de Videojuegos
 * Autor: David Vallejo Fernández    David.Vallejo@uclm.es
 *
 * Código modificado a partir de Managing Game States with OGRE
 * http://www.ogre3d.org/tikiwiki/Managing+Game+States+with+OGRE
 * Inspirado en Managing Game States in C++
 * http://gamedevgeek.com/tutorials/managing-game-states-in-c/
 *
 * You can redistribute and/or modify this file under the terms of the
 * GNU General Public License ad published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * and later version. See <http://www.gnu.org/licenses/>.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  
 *********************************************************************/

#ifndef PlayState_H
#define PlayState_H

#define MAX_FUERZA 40.0
#define MIN_FUERZA 10.0
#define INC_FUERZA 0.75

#include <Ogre.h>
#include <OIS/OIS.h>
#include <cstdlib>

#include <OgreBulletDynamicsRigidBody.h>

#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include <Shapes/OgreBulletCollisionsConvexHullShape.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes/OgreBulletCollisionsTrimeshShape.h>

#include <Utils/OgreBulletCollisionsMeshToShapeConverter.h> 

#include <btBulletDynamicsCommon.h>

#include "GameState.h"
#include "EndState.h"
#include "Graph.h"
#include "Ghost.h"
#include "TrackManager.h"
#include "SoundFXManager.h"


class PlayState : public Ogre::Singleton<PlayState>, public GameState
{
 public:
  PlayState () {}

  void enter ();
  void exit ();
  void pause ();
  void resume ();

  void keyPressed (const OIS::KeyEvent &e);
  void keyReleased (const OIS::KeyEvent &e);

  bool frameStarted (const Ogre::FrameEvent& evt);
  bool frameEnded (const Ogre::FrameEvent& evt);

  // Heredados de Ogre::Singleton.
  static PlayState& getSingleton ();
  static PlayState* getSingletonPtr ();

  void createScene();
  void pacmanMove();
  void destroyAllAttachedMovableObjects(Ogre::SceneNode* node);
  void removeScene();
  void createOverlay();
  void createInitialWorld();
  void shoot();

 protected:
  Ogre::Root* _root;
  Ogre::SceneManager* _sceneMgr;
  Ogre::Viewport* _viewport;
  Ogre::Camera* _camera;
  Ogre::Light* _light;


  bool _exitGame, _derecha, _arriba, _abajo, _izquierda, _shoot;

  int _perspective, _score, _numBall;


  float _fuerza;

  Ogre::Real _deltaT;
  Ogre::Vector3 _pacMove;
  
  Ogre::OverlayManager* _overlayManager;
  Ogre::Overlay *_ovJuego;
  Ogre::OverlayElement *_ovPunt, *_ovVida, *_ovScore;

  TrackPtr _mainTrack;
  SoundFXPtr _simpleEffect;
  TrackManager* _pTrackManager;
  SoundFXManager* _pSoundFXManager;

  Ogre::AnimationState *_animState;

  Graph* _level;

  OgreBulletDynamics::DynamicsWorld * _world;
  OgreBulletCollisions::DebugDrawer * _debugDrawer;

  std::deque <OgreBulletDynamics::RigidBody *>         _bodies;
  std::deque <OgreBulletCollisions::CollisionShape *>  _shapes;

  Ogre::SceneNode *_nodCannion, *_nodBase, *_nodShoot;

  Ogre::Vector3 _dir;
  
};

#endif
