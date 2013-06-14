/**
@file Gravity.cpp

Contiene la implementacion del componente
de disparo de la cabra.

@see Logic::CGravityFeedback
@see Logic::ISpellFeedback

@author Francisco Aisa Garc�a
@date Mayo, 2013
*/

#include "GravityFeedback.h"
#include "HudWeapons.h"

#include "Logic/Maps/EntityFactory.h"
#include "Logic/Maps/Map.h"
#include "Logic/Server.h"
#include "Map/MapEntity.h"

using namespace std;

namespace Logic {

	IMP_FACTORY(CGravityFeedback);

	//__________________________________________________________________

	CGravityFeedback::CGravityFeedback() : ISpellFeedback("gravity") {
		// Nada que hacer
	}

	//__________________________________________________________________

	CGravityFeedback::~CGravityFeedback() {
		// Nada que hacer
	}


	//__________________________________________________________________

	bool CGravityFeedback::spawn(CEntity* entity, CMap *map, const Map::CEntity *entityInfo) {
		if( !ISpellFeedback::spawn(entity, map, entityInfo) ) return false;

		return true;
	} // spawn
	//__________________________________________________________________

	void CGravityFeedback::spell(){
		// Efecto cuando se activa la gravedad

	} // spell
	//__________________________________________________________________
		
	void CGravityFeedback::stopSpell(){
		// Efecto cuando se desactiva la gravedad

	} // stopSpell
	//__________________________________________________________________

}//namespace Logic