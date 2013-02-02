/**
@file Shoot.cpp

Contiene la implementaci�n del componente que gestiona las armas y que administra el disparo.
 
@see Logic::CShoot
@see Logic::IComponent

*/

#include "ShootMiniGun.h"

#include "Logic/Entity/Entity.h"
#include "Map/MapEntity.h"
#include "Physics/Server.h"
#include "Graphics/Server.h"
#include "Graphics/Scene.h"
#include "Logic/Entity/Components/ArrayGraphics.h"
#include "Logic/Entity/Components/Life.h"

#include "Graphics/Camera.h"



#include <OgreSceneManager.h>
#include <OgreMaterialManager.h>
#include <OgreManualObject.h>

namespace Logic 
{
	IMP_FACTORY(CShootMiniGun);
	
	//---------------------------------------------------------

	bool CShootMiniGun::spawn(CEntity *entity, CMap *map, const Map::CEntity *entityInfo) 
	{
		if(!IComponent::spawn(entity,map,entityInfo))
			return false;

		if(entityInfo->hasAttribute("physic_radius")){
			_capsuleRadius = entityInfo->getFloatAttribute("physic_radius");
		}
						
		std::stringstream aux;
		aux << "weapon" << "MiniGun";
		std::string weapon = aux.str();
												
		_weapon.name = entityInfo->getStringAttribute(weapon+"Name");
		_weapon.damage= (unsigned char) entityInfo->getIntAttribute(weapon+"Damage");
		_weapon.dispersion = entityInfo->getFloatAttribute(weapon+"Dispersion");
		_weapon.distance = entityInfo->getFloatAttribute(weapon+"Distance");
		_weapon.numberShoots = (unsigned char) entityInfo->getIntAttribute(weapon+"NumberShoots");
		_weapon.coolDown = (unsigned char) entityInfo->getIntAttribute(weapon+"CoolDown");
		_weapon.ammo = entityInfo->getIntAttribute(weapon+"ammo");
		
		return true;

	} // spawn
	//---------------------------------------------------------

	void CShootMiniGun::desactivate(){
		
	}
	//---------------------------------------------------------
	bool CShootMiniGun::accept(CMessage *message)
	{
		return message->getMessageType() == Message::CONTROL;
	} // accept
	//---------------------------------------------------------


	void CShootMiniGun::process(CMessage *message)
	{
		
		switch(message->getMessageType())
		{
			case Message::CONTROL:
				if(((CMessageControl*)message)->getType()==Control::LEFT_CLICK){
					shoot();
				}
				break;
			break;
		}
		
	} // process
	//---------------------------------------------------------

	void CShootMiniGun::shoot(){
		
		//Generaci�n del rayo habiendo obtenido antes el origen y la direcci�n
		Graphics::CCamera* camera = Graphics::CServer::getSingletonPtr()->getActiveScene()->getCamera();
		
		
		// Para generalizar las armas, todas tendras tantas balas como la variable numberShoots
		for(int i = 0; i < _weapon.numberShoots; ++i)
		{

			// Se corrige la posicion de la camara
			Vector3 direction = camera->getTargetCameraPosition() - camera->getCameraPosition();
			direction.normalise();
			//El origen debe ser m�nimo la capsula y por si miramos al suelo la separaci�n m�nima debe ser 1.5f ( en un futuro es probable que sea recomendable cambiar por no chocar con el grupo de uno mismo )
			Vector3 origin = camera->getCameraPosition() + ((_capsuleRadius + 1.5f) * direction);


			//Me dispongo a calcular la desviacion del arma, en el map.txt se pondra en grados de dispersion (0 => sin dispersion)
			Ogre::Radian angle = Ogre::Radian(_weapon.dispersion/100);

			//Esto hace un random total, lo que significa, por ejemplo, que puede que todas las balas vayan hacia la derecha 
			Vector3 dispersionDirection = direction.randomDeviant(angle);
			dispersionDirection.normalise();

			// Creamos el ray desde el origen en la direccion del raton (desvido ya aplicado)
			Ray ray(origin, dispersionDirection);
			


			////////////////////////////////////////////////Dibujo del rayo


				Graphics::CScene *scene = Graphics::CServer::getSingletonPtr()->getActiveScene();
				Ogre::SceneManager *mSceneMgr = scene->getSceneMgr();

				
				std::stringstream aux;
				aux << "laserMiniGun" << _temporal;
				++_temporal;
				std::string laser = aux.str();


				Ogre::ManualObject* myManualObject =  mSceneMgr->createManualObject(laser); 
				Ogre::SceneNode* myManualObjectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(laser+"_node"); 
 

				myManualObject->begin("laser", Ogre::RenderOperation::OT_LINE_STRIP);
				Vector3 v = ray.getOrigin();
				myManualObject->position(v.x,v.y,v.z);


			for(int i=0; i < _weapon.distance;++i){
				Vector3 v = ray.getPoint(i);
				myManualObject->position(v.x,v.y,v.z);
				// etc 
			}

				myManualObject->end(); 
				myManualObjectNode->attachObject(myManualObject);


			//////////////////////////////////fin del dibujado del rayo

			//Rayo lanzado por el servidor de f�sicas de acuerdo a la distancia de potencia del arma
			CEntity *entity = Physics::CServer::getSingletonPtr()->raycastClosest(ray, _weapon.distance);
		
			//Si hay colisi�n env�amos a dicha entidad un mensaje de da�o
			if(entity)
			{
				
				printf("\nimpacto con %s", entity->getName().c_str());

				// LLamar al componente que corresponda con el da�o hecho
				//entity->

				Logic::CMessageDamaged *m=new Logic::CMessageDamaged(Logic::Message::DAMAGED);
				m->setDamage(_weapon.damage);
				entity->emitMessage(m);
			}
		
		
		}// fin del bucle para multiples disparos
	} // shoot

} // namespace Logic

