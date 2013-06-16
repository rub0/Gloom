//---------------------------------------------------------------------------
// ComeBackFeedback.h
//---------------------------------------------------------------------------

/**
@file ComeBackFeedback.h

Contiene la declaraci�n de la cabecera del componente
de disparo de la cabra.

@see Logic::CComeBackFeedback
@see Logic::ISpellFeedback

@author Jaime Chapinal Cervantes
@date Junio, 2013
*/

#ifndef __Logic_ComeBackFeedback_H
#define __Logic_ComeBackFeedback_H

#include "Logic/Entity/Components/SpellFeedback.h"

#include <set>

namespace Logic {

	/**
    @ingroup logicGroup

	Este componente implementa la funcionalidad de la cabra.

	@author Francisco Aisa Garc�a
	@date Mayo, 2013
	*/

	class CComeBackFeedback : public ISpellFeedback {
		DEC_FACTORY(CComeBackFeedback);

	public:

		/** Constructor por defecto. */
		CComeBackFeedback();

		//__________________________________________________________________

		virtual ~CComeBackFeedback();

		//__________________________________________________________________

		virtual bool spawn(CEntity* entity, CMap *map, const Map::CEntity *entityInfo);

		//__________________________________________________________________

		/**
		M�todo virtual que debe ser implementado por las clases derivadas para
		especificar que ocurre al usar el spellCast
		*/
		virtual void spell();

		//__________________________________________________________________
		
		/**
		Este m�todo es invocado cuando se deja de pulsar el bot�n de disparo
		primario.
		*/
		virtual void stopSpell();

		//__________________________________________________________________
	};

	REG_FACTORY(CComeBackFeedback);

} // namespace Logic

#endif // __Logic_ComeBackFeedback_H