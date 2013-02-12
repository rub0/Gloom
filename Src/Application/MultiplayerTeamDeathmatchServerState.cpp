//---------------------------------------------------------------------------
// MultiplayerTeamDeathmatchServerState.cpp
//---------------------------------------------------------------------------

/**
@file MultiplayerTeamDeathmatchServerState.cpp

Contiene la implementaci�n del estado de juego.

@see Application::MultiplayerTeamDeathmatchServerState
@see Application::CGameState

@author Francisco Aisa Garc�a
@date Febrero, 2013
*/

#include "MultiplayerTeamDeathmatchServerState.h"

namespace Application {


	void CMultiplayerTeamDeathmatchServerState::activate() 
	{
		CGameState::activate();

		Net::CManager::getSingletonPtr()->addObserver(this);
	} // activate

	//--------------------------------------------------------

	void CMultiplayerTeamDeathmatchServerState::deactivate() 
	{
		Net::CManager::getSingletonPtr()->removeObserver(this);
		Net::CManager::getSingletonPtr()->deactivateNetwork();

		CGameState::deactivate();
	} // deactivate

	//--------------------------------------------------------

	/*void CGameStateServer::doStart()
	{
		_waiting = false;

		// Construimos un buffer para alojar el mensaje de carga de mapa que
		// enviaremos a todos los clientes mediante send
		Net::NetMessageType msg = Net::LOAD_MAP;
		Net::CBuffer buffer(sizeof(msg));
		buffer.write(&msg, sizeof(msg));

		Net::CManager::getSingletonPtr()->send(buffer.getbuffer(), buffer.getSize());

		// Cargamos el archivo con las definiciones de las entidades del nivel.
		if (!Logic::CEntityFactory::getSingletonPtr()->loadBluePrints("blueprints_server.txt"))
		{
			Net::CManager::getSingletonPtr()->deactivateNetwork();
			_app->exitRequest();
		}

		// Cargamos el nivel a partir del nombre del mapa. 
		if (!Logic::CServer::getSingletonPtr()->loadLevel("map_server.txt"))
		{
			Net::CManager::getSingletonPtr()->deactivateNetwork();
			_app->exitRequest();
		}
	} // doStart

	//--------------------------------------------------------

	void CGameStateServer::dataPacketReceived(Net::CPaquete* packet)
	{
		Net::CBuffer buffer(packet->getDataLength());
		buffer.write(packet->getData(), packet->getDataLength());
		// Desplazamos el puntero al principio para realizar la lectura
		buffer.reset();

		Net::NetMessageType msg;
		//memcpy(&msg, packet->getData(),sizeof(msg));
		buffer.read(&msg, sizeof(msg));
		
		switch (msg)
		{
		case Net::MAP_LOADED:
		{

			// @todo Borrar las siguientes 3 l�neas. Est�n de momento para que
			// se lance el estado de juego al cargar el mapa

			//Almacenamos el ID del usuario que se ha cargado el mapa.
			_mapLoadedByClients.push_back(packet->getConexion()->getId());

			//Si todos los clientes han cargado los mapas pasamos a crear jugadores.
			if(_clients.size() == _mapLoadedByClients.size())
			{
				// Se debe crear un jugador por cada cliente registrado.
				for(TNetIDList::const_iterator it = _clients.begin(); it != _clients.end(); it++)
				{
					//Preparamos la lista de control de carga de jugadores.
					//Esto quiere decir que el cliente (*it) ha cargado 0 jugadores
					TNetIDCounterPair elem(*it,0);
					_playersLoadedByClients.insert(elem);

					// @todo Hay que enviar un paquete tipo LOAD_PLAYER con 
					// el NetID del cliente del que estamos creando el jugador (*it)
					Net::NetMessageType msg = Net::LOAD_PLAYER;
					Net::NetID netId = *it;

					
					Net::CBuffer buffer(sizeof(msg) + sizeof(netId) + sizeof(Logic::EntityID));
					buffer.write(&msg, sizeof(msg));
					buffer.write(&netId, sizeof(netId));

					// Mandar el ID del cliente actual a todos los clientes para que
					// lo carguen
					
					// Creamos el player. Deber�amos poder propocionar caracter�asticas
					// diferentes seg�n el cliente (nombre, modelo, etc.). Esto es una
					// aproximaci�n, solo cambiamos el nombre y decimos si es el jugador
					// local
					std::string name("Player");
					std::stringstream number;
					number << (*it);
					name.append(number.str());

					// @todo Llamar al m�todo de creaci�n del jugador. Deberemos decidir
					// si el jugador es el jugador local. Al ser el servidor ninguno lo es

					Logic::CEntity * player = Logic::CServer::getSingletonPtr()->getMap()->createPlayer(name);

					Logic::TEntityID id = player->getEntityID();
					buffer.write(&id, sizeof(id));


					Net::CManager::getSingletonPtr()->send(buffer.getbuffer(), buffer.getSize());
				}
			}
			break;
		}
		case Net::PLAYER_LOADED:
		{
			//Aumentamos el n�mero de jugadores cargados por el cliente
			(*_playersLoadedByClients.find(packet->getConexion()->getId())).second++;

			// @todo Comprobar si todos los clientes han terminado de 
			// cargar todos los jugadores
			bool loadFinished = true;
			TNetIDCounterMap::const_iterator it2;
			for(TNetIDList::const_iterator it = _clients.begin(); it != _clients.end() && loadFinished; it++) {
				// Obtener la pareja (NetID, jugadores cargados)
				it2 = _playersLoadedByClients.find(*it);
				
				if(it2 != _playersLoadedByClients.end()) {
					// Si el numero de jugadores cargados para este cliente es menor
					// que el numero total de clientes cargar, entonces es que aun
					// hay clientes que no han cargado a todos los jugadores
					if(it2->second < _clients.size()) {
						loadFinished = false;
					}
				}
			}
			//Si todos los clientes han cargado todos los players
			if(loadFinished)
			{
				//Enviamos el mensaje de que empieza el juego a todos los clientes
				Net::NetMessageType msg = Net::START_GAME;
				Net::CManager::getSingletonPtr()->send(&msg, sizeof(msg));
				_app->setState("game");
			}
			break;
		}
		}

	} // dataPacketReceived

	//--------------------------------------------------------

	void CGameStateServer::connexionPacketReceived(Net::CPaquete* packet)
	{
		if(_waiting)
		{
			//Mostramos un poco de informaci�n en el status
			char id[100];
			unsigned int ip = packet->getConexion()->getAddress();
			byte* p = (byte*)&ip;
			sprintf_s(id,"\nClient conected: %d.%d.%d.%d:%d\nGame can start...\n",p[0],p[1],p[2],p[3], packet->getConexion()->getPort()); 				
			CEGUI::WindowManager::getSingleton().getWindow("NetLobbyServer/Status")->appendText(id);
			
			//Habilitamos el bot�n de start.
			CEGUI::WindowManager::getSingleton().getWindow("NetLobbyServer/Start")->setEnabled(true);

			//Almacenamos el ID del usuario que se ha conectado.
			_clients.push_back(packet->getConexion()->getId());
		}

	} // connexionPacketReceived

	//--------------------------------------------------------

	void CGameStateServer::disconnexionPacketReceived(Net::CPaquete* packet)
	{
		// @todo gestionar desconexiones.
		// TODO gestionar desconexiones.
		//Eliminamos el ID del usuario que se ha desconectado.
		_clients.remove( packet->getConexion()->getId() );
		_mapLoadedByClients.remove( packet->getConexion()->getId() );

		TNetIDCounterMap::const_iterator pairIt = _playersLoadedByClients.find(packet->getConexion()->getId());
		if(pairIt != _playersLoadedByClients.end())
			_playersLoadedByClients.erase(pairIt);

		if(!_clients.empty()) {
			CEGUI::WindowManager::getSingleton().getWindow("NetLobbyServer/Status")
				->setText("Client disconnected. Waiting for new clients...");					
		} else{
			CEGUI::WindowManager::getSingleton().getWindow("NetLobbyServer/Status")
				->setText("All clients disconnected. Waiting for some client...");
			CEGUI::WindowManager::getSingleton().getWindow("NetLobbyServer/Start")->setEnabled(false); //Dehabilitamos el bot�n de start.
		}

	} // disconnexionPacketReceived
	*/
} // namespace Application