//---------------------------------------------------------------------------
// MultiplayerTeamDeathmatchServerState.h
//---------------------------------------------------------------------------

/**
@file MultiplayerTeamDeathmatchServerState.h

Contiene la declaraci�n del estado de juego.

@see Application::MultiplayerTeamDeathmatchServerState
@see Application::CGameState

@author Francisco Aisa Garc�a
@date Febrero, 2013
*/

#ifndef __Application_MultiplayerTeamDeathmatchServerState_H
#define __Application_MultiplayerTeamDeathmatchServerState_H

#include "GameState.h"
#include "Net/Manager.h"

namespace Application {
	
	/**
	@ingroup applicationGroup

	Representa el estado de juego online para el servidor. En el futuro
	tendremos diferentes estados por cada modo de juego, pero de momento
	se queda asi hasta que por lo menos tengamos un deathmatch.

	Hereda de CGameState por que es un estado de juego y de Net::CManager::IObserver
	para que se le avise de que ha recibido paquetes.

	@author Francisco Aisa Garc�a
	@date Febrero, 2013
	*/

	class CMultiplayerTeamDeathmatchServerState : public CGameState, public Net::CManager::IObserver {
	public:


		// =======================================================================
		//                      CONSTRUCTORES Y DESTRUCTOR
		// =======================================================================


		/**
		Constructor por parametro.

		@param app Aplicacion que se encarga de manejar los estados.
		*/
		CMultiplayerTeamDeathmatchServerState(CBaseApplication *app) : CGameState(app) {}


		// =======================================================================
		//                           METODOS HEREDADOS
		// =======================================================================


		/**
		Funci�n llamada por la aplicaci�n cuando se activa
		el estado.
		*/
		virtual void activate();

		//______________________________________________________________________________

		/**
		Funci�n llamada por la aplicaci�n cuando se desactiva
		el estado.
		*/
		virtual void deactivate();

		//______________________________________________________________________________

		/**
		Se dispara cuando se recibe un paquete de datos.

		@see Net::CManager::IObserver
		@param packet Paquete de datos recibido.
		*/
		virtual void dataPacketReceived(Net::CPaquete* packet);

		//______________________________________________________________________________

		/**
		Se dispara cuando se recibe un paquete de conexion.

		@see Net::CManager::IObserver
		@param packet Paquete de conexion recibido.
		*/
		virtual void connexionPacketReceived(Net::CPaquete* packet);

		//______________________________________________________________________________

		/**
		Se dispara cuando se recibe un paquete de desconexion.

		@see Net::CManager::IObserver
		@param packet Paquete de desconexion recibido.
		*/
		virtual void disconnexionPacketReceived(Net::CPaquete* packet);

	}; // CMultiplayerTeamDeathmatchServerState

} // namespace Application

#endif //  __Application_GameState_H
