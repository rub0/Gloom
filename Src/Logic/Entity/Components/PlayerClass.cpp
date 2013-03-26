/**
@file PlayerClass.cpp

Contiene la implementación del componente que define
el comportamiento general de las habilidades de cada
clase.
 
@see Logic::CPlayerClass
@see Logic::IComponent

@author Francisco Aisa García
@date Marzo, 2013
*/

#include "PlayerClass.h"
#include "Hound.h"
#include "Archangel.h"
#include "Shadow.h"
#include "Screamer.h"
#include "Logic/Entity/Entity.h"
#include "Map/MapEntity.h"

#include "Logic/Messages/MessageChangePlayerClass.h"
#include "Logic/Messages/MessageControl.h"

namespace Logic {
	//IMP_FACTORY(CPlayerClass);
	
	CPlayerClass::CPlayerClass(const std::string& playerClassName) : _primarySkillTimer(0),
																	 _secondarySkillTimer(0),
																	 _playerClassName(playerClassName) {
		
		// No hay memoria dinamica que reservar
	}

	//__________________________________________________________________

	CPlayerClass::~CPlayerClass() {
		// No hay memoria dinamica que liberar
	}

	//__________________________________________________________________

	bool CPlayerClass::spawn(CEntity* entity, CMap* map, const Map::CEntity* entityInfo) {
		if( !IComponent::spawn(entity,map,entityInfo) ) return false;

		std::string primarySkillCooldownName = _playerClassName + "PrimarySkillCooldown";
		std::string secondarySkillCooldownName = _playerClassName + "SecondarySkillCooldown";

		// Leer el tiempo de cooldown de las habilidades primarias y secundarias
		assert( entityInfo->hasAttribute(primarySkillCooldownName) );
		// Pasamos el tiempo a msecs
		_primarySkillCooldown = entityInfo->getFloatAttribute(primarySkillCooldownName) * 1000;

		assert( entityInfo->hasAttribute(secondarySkillCooldownName) );
		// Pasamos el tiempo a msecs
		_secondarySkillCooldown = entityInfo->getFloatAttribute(secondarySkillCooldownName) * 1000;

		return true;
	} // spawn
	
	//__________________________________________________________________

	bool CPlayerClass::accept(CMessage* message) {
		Logic::TMessageType msgType = message->getMessageType();

		return msgType == Message::CONTROL || msgType == Message::CHANGE_PLAYER_CLASS;
	} // accept
	
	//__________________________________________________________________

	void CPlayerClass::process(CMessage* message) {
		Logic::TMessageType msgType = message->getMessageType();

		switch(msgType) {
			case Message::CHANGE_PLAYER_CLASS: {
				CMessageChangePlayerClass* changeClassMsg = static_cast<CMessageChangePlayerClass*>(message);

				// Cambiar a la clase que toque en funcion del enumerado recibido
				changePlayerClass( changeClassMsg->getPlayerClass() );

				break;
			}
			case Message::CONTROL: {
				ControlType type = static_cast<CMessageControl*>(message)->getType();

				if(type == ControlType::USE_PRIMARY_SKILL) {
					if( canUsePrimarySkill() ) {
						primarySkill();
						_primarySkillTimer = _primarySkillCooldown;
					}
				}
				else if(type == ControlType::USE_SECONDARY_SKILL) {
					if( canUseSecondarySkill() ) {
						secondarySkill();
						_secondarySkillTimer = _secondarySkillCooldown;
					}
				}

				break;
			}
		}
	} // process

	//__________________________________________________________________

	void CPlayerClass::tick(unsigned int msecs) {
		// Se encarga de procesar los mensajes encolados
		IComponent::tick(msecs);

		// Controlamos el cooldown de la habilidad primaria y secundaria
		if(_primarySkillTimer > 0) {
			_primarySkillTimer -= msecs;
			
			if(_primarySkillTimer < 0)
				_primarySkillTimer = 0;
		}

		if(_secondarySkillTimer > 0) {
			_secondarySkillTimer -= msecs;

			if(_secondarySkillTimer < 0)
				_secondarySkillTimer = 0;
		}
	} // tick

	//__________________________________________________________________

	void CPlayerClass::changePlayerClass(unsigned int classType) {
		deactivate();

		IComponent* selectedClassComponent;
		switch(classType) {
			case CPlayerClass::Type::eHOUND:
				selectedClassComponent = _entity->getComponent<CHound>("CHound");
				break;
			case CPlayerClass::Type::eARCHANGEL:
				selectedClassComponent = _entity->getComponent<CArchangel>("CArchangel");
				break;
			case CPlayerClass::Type::eSCREAMER:
				selectedClassComponent = _entity->getComponent<CScreamer>("CScreamer");
				break;
			case CPlayerClass::Type::eSHADOW:
				selectedClassComponent = _entity->getComponent<CShadow>("CShadow");
				break;
		}

		if(selectedClassComponent != NULL)
			selectedClassComponent->activate();
	}

	//__________________________________________________________________

	bool CPlayerClass::canUsePrimarySkill() {
		return _primarySkillTimer == 0;
	}

	//__________________________________________________________________

	bool CPlayerClass::canUseSecondarySkill() {
		return _secondarySkillTimer == 0;
	}

} // namespace Logic

