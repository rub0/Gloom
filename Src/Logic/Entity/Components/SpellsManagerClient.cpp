/**
@file SpellManagerClient.cpp

Contiene la implementación del componente que gestiona las armas y que administra el disparo.
 
@see Logic::CSpellManagerClient
@see Logic::IComponent

@Author Antonio Jesus Narvaez
@date July, 2013
*/

#include "SpellsManagerClient.h"

#include "Logic/Entity/Entity.h"
#include "Map/MapEntity.h"

#include "AmplifyDamageClient.h"
#include "ComeBackClient.h"
#include "HungryClient.h"
#include "GravityClient.h"
#include "HardDeathClient.h"
#include "ShieldClient.h"
#include "CoolDownClient.h"
#include "ResurrectionClient.h"

namespace Logic 
{
	IMP_FACTORY(CSpellsManagerClient);

	//---------------------------------------------------------
	
	CSpellsManagerClient::CSpellsManagerClient() {
		
	}

	//---------------------------------------------------------

	bool CSpellsManagerClient::spawn(CEntity *entity, CMap *map, const Map::CEntity *entityInfo) {
		if(!IComponent::spawn(entity,map,entityInfo)) return false;

		assert( entityInfo->hasAttribute("primarySpell") && "Debe de tener campo primarySpell");
		assert( entityInfo->hasAttribute("secondarySpell") && "Debe de tener campo secondarySpell");
		
		_primarySpell = (SpellType::Enum)entityInfo->getIntAttribute("primarySpell");
		_secondarySpell = (SpellType::Enum)entityInfo->getIntAttribute("secondarySpell");


		entitySpawn = entity;
		mapSpawn = map;
		entityInfoSpawn = entityInfo;

		return true;

	} // spawn
	
	//---------------------------------------------------------

	void CSpellsManagerClient::onStart(){
	// El resto de las armas están desactivadas, ya que no las tenemos
		addComponent();
	} // onStart
	//---------------------------------------------------------

	void CSpellsManagerClient::addComponent(){
		IComponent *temp;
		std::string ID;
		int i;
		SpellType::Enum spell;
		for(i = 0, spell = _primarySpell; i<2 ;++i, spell = _secondarySpell){
			switch(spell){
				case SpellType::eAMPLIFY_DAMAGE: 
						temp = new CAmplifyDamageClient();
						ID = std::string("CAmplifyDamageClient");
				break;
				case SpellType::eCOME_BACK: 
						temp = new CComeBackClient();
						ID = std::string("CComeBackClient");
				break;
				case SpellType::eCOOLDOWN: 
						temp = new CCoolDownClient();
						ID = "CCoolDownClient";
				break;
				case SpellType::eGRAVITY: 
						temp = new CGravityClient();
						ID = "CGravityClient";
				break;
				case SpellType::eHARD_DEATH: 
						temp = new CHardDeathClient();
						ID = "CHardDeathClient";
				break;
				case SpellType::eHUNGRY: 
						temp = new CHungryClient();
						ID = "CHungryClient";
				break;
				case SpellType::eSHIELD: 
						temp = new CShieldClient();
						ID = "CShieldClient";
				break;
				case SpellType::eRESURECTION: 
						temp = new CResurrectionClient();
						ID = "CResurrectionClient";
				break;
			}
			temp->spawn(entitySpawn, mapSpawn, entityInfoSpawn);
			temp->activate();
			_entity->addComponent(temp,ID);
		}
		
	} // addComponent
	//---------------------------------------------------------


} // namespace Logic

