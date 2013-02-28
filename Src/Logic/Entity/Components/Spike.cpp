/**
@file Spike.cpp

Contiene la implementaci�n del componente que controla el movimiento de un ascensor.
 
@see Logic::CSpike
@see Logic::IComponent

@author Jose Antonio Garc�a Y��ez
@date Febrero, 2013
*/

#include "Spike.h"

#include "Logic/Entity/Entity.h"
#include "Map/MapEntity.h"
#include "PhysicController.h"
#include "Logic/Maps/Map.h"
#include "Logic/Server.h"

#include "Logic/Messages/MessageActivate.h"
#include "Logic/Messages/MessageDeactivate.h"
#include "Logic/Messages/MessageDamaged.h"
#include "Logic/Messages/MessageTouched.h"
#include "Logic/Messages/MessageKinematicMove.h"

namespace Logic 
{
	IMP_FACTORY(CSpike);
	
	//---------------------------------------------------------
	
	bool CSpike::spawn(CEntity *entity, CMap *map, const Map::CEntity *entityInfo) 
	{
		if(!IComponent::spawn(entity,map,entityInfo))
			return false;

		if(entityInfo->hasAttribute("timeToShow"))
			_timeToShow = entityInfo->getIntAttribute("timeToShow");

		if(entityInfo->hasAttribute("timeToUnshow"))
			_timeToUnshow = entityInfo->getIntAttribute("timeToUnshow");

		if(entityInfo->hasAttribute("positionInitial"))
			_positionInitial = entityInfo->getVector3Attribute("positionInitial");

		if(entityInfo->hasAttribute("positionFinal"))
			_positionFinal = entityInfo->getVector3Attribute("positionFinal");

		if(entityInfo->hasAttribute("velocity"))
			_velocity = entityInfo->getFloatAttribute("velocity");


		return true;

	} // spawn
	
	//---------------------------------------------------------


	
	void CSpike::activate()
	{
		IComponent::activate();
		_timer=0;
		_active=false;
		_timeToShow*=1000;
		_timeToUnshow*=1000;

		_directionInitial=(_positionInitial-_positionFinal);
		_directionFinal=(_positionFinal-_positionInitial);
		_directionInitial.normalise();
		_directionFinal.normalise();
		_toFinal=false;

	} // activate
	//---------------------------------------------------------


	bool CSpike::accept(CMessage *message)
	{
		return message->getMessageType() == Message::TOUCHED;
	} // accept
	
	//---------------------------------------------------------

	void CSpike::process(CMessage *message)
	{
		Logic::CMessageDamaged* m;
		switch(message->getMessageType())
		{
		case Message::TOUCHED:
			m = new Logic::CMessageDamaged();
			m->setDamage(1000);
			m->setEnemy(_entity);
			((CMessageTouched*)message)->getEntity()->emitMessage(m);
			break;
		}

	} // process
	//----------------------------------------------------------

	void CSpike::tick(unsigned int msecs)
	{
		IComponent::tick(msecs);
		Vector3 toDirection;
		_timer+=msecs;
		//Si ha pasado el tiempo mostramos los pinchos
		if(_timer>_timeToShow && !_active){
			_timer=0;
			_active=true;
			_toFinal=true;
		}
		//Si los pinchos estan fuera solamente duran X segundos
		if(_active && _timer>_timeToUnshow){
			_active=false;
			_timer=0;
			_toFinal=false;
		}

		//Hacia la posicion final
		if(_toFinal){
			if(_entity->getPosition().distance(_positionFinal)>0.2){
				toDirection = _directionFinal * msecs * _velocity;
				Logic::CMessageKinematicMove* m = new Logic::CMessageKinematicMove();
				m->setMovement(toDirection);
				_entity->emitMessage(m);
			}
		}
		//Hacia la posicion inicial
		else{
			if(_entity->getPosition().distance(_positionInitial)>0.2){
				toDirection = _directionInitial * msecs * _velocity;
				Logic::CMessageKinematicMove* m = new Logic::CMessageKinematicMove();
				m->setMovement(toDirection);
				_entity->emitMessage(m);
			}
		}


	} // tick
	//----------------------------------------------------------


} // namespace Logic

