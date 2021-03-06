/**
@file PositionInterpolator.h


@see Logic::CPositionInterpolator
@see Logic::IComponent

@author Francisco Aisa Garc�a
@date Mayo, 2013
*/

#ifndef __Logic_PositionInterpolator_H
#define __Logic_PositionInterpolator_H

#include "Logic/Entity/Component.h"

#include <deque>

namespace Logic {
	
	/**
	
    @ingroup logicGroup

	@author Francisco Aisa Garc�a
	@date Mayo, 2013
	*/
	
	class CPositionInterpolator : public IComponent {
		DEC_FACTORY(CPositionInterpolator);
	public:


		// =======================================================================
		//                      CONSTRUCTORES Y DESTRUCTOR
		// =======================================================================


		/** Constructor por defecto. */
		CPositionInterpolator();

		//__________________________________________________________________

		/** Destructor. */
		virtual ~CPositionInterpolator();


		// =======================================================================
		//                    METODOS HEREDADOS DE ICOMPONENT
		// =======================================================================


		/**
		Inicializaci�n del componente a partir de la informaci�n extraida de la entidad
		leida del mapa:
		<ul>
			<li><strong>interpolationTimestep:</strong> Tiempo de interpolaci�n. </li>
		</ul>

		@param entity Entidad a la que pertenece el componente.
		@param map Mapa L�gico en el que se registrar� el objeto.
		@param entityInfo Informaci�n de construcci�n del objeto le�do del fichero de disco.
		@return Cierto si la inicializaci�n ha sido satisfactoria.
		*/
		virtual bool spawn(CEntity* entity, CMap *map, const Map::CEntity *entityInfo);

		//__________________________________________________________________

		/** 
		Este componente acepta los siguientes mensajes:

		<ul>
			<li></li>
		</ul>
		
		@param message Mensaje a chequear.
		@return true si el mensaje es aceptado.
		*/
		virtual bool accept(const std::shared_ptr<CMessage>& message);

		//__________________________________________________________________

		/**
		M�todo virtual que procesa un mensaje.

		@param message Mensaje a procesar.
		*/
		virtual void process(const std::shared_ptr<CMessage>& message);

		
		// =======================================================================
		//                            METODOS PROPIOS
		// =======================================================================


	protected:


		// =======================================================================
		//                    METODOS HEREDADOS DE ICOMPONENT
		// =======================================================================


		/**
		M�todo llamado en cada frame. En este componente se encarga
		de reducir la vida del individuo paulatinamente en base a los
		par�metros fijados desde fichero.

		@param msecs Milisegundos transcurridos desde el �ltimo tick.
		*/
		virtual void onFixedTick(unsigned int msecs);


	private:


		void interpolateSnapshot(const std::vector<Vector3>& buffer);

		// =======================================================================
		//                          MIEMBROS PRIVADOS
		// =======================================================================
		

		unsigned int _lostTicks;
		unsigned int _ticksPerSample;
		unsigned int _samplesPerSnapshot;

		std::deque<Vector3> _buffer;

	}; // class CPositionInterpolator

	REG_FACTORY(CPositionInterpolator);

} // namespace Logic

#endif // __Logic_SnapshotInterpolator_H
