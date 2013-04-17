//---------------------------------------------------------------------------
// LobbyClientState.cpp
//---------------------------------------------------------------------------

/**
@file LobbyClientState.cpp

Contiene la implementaci�n del estado de lobby del cliente.

@see Application::CApplicationState
@see Application::CLobbyClientState

@author David Llans�
@date Agosto, 2010
*/

#include "LobbyClientState.h"

#include "Logic/Server.h"
#include "Logic/Maps/EntityFactory.h"
#include "Logic/Maps/Map.h"

#include "GUI/Server.h"
#include "Input\Server.h"
#include "Logic/Entity/Entity.h"
#include "Net/Manager.h"
#include "Net/Cliente.h"
#include "Net/factoriared.h"
#include "Net/paquete.h"
#include "Net/conexion.h"
#include "Net/buffer.h"
#include "Hikari.h"

#include <iostream>
#include <fstream>
using namespace std;

namespace Application {

	CLobbyClientState::~CLobbyClientState() 
	{
	} // ~CLobbyClientState

	//--------------------------------------------------------

	bool CLobbyClientState::init() 
	{
		CApplicationState::init();

		// Cargamos la ventana que muestra el men�
		
		_menu = GUI::CServer::getSingletonPtr()->addLayoutToState(this,"joinGame", Hikari::Position(Hikari::Center));
		_menu->load("MultiplayerClient.swf");
		_menu->bind("connect",Hikari::FlashDelegate(this, &CLobbyClientState::startReleased));
		_menu->bind("back",Hikari::FlashDelegate(this, &CLobbyClientState::backReleased));
		_menu->hide();
		
		return true;

	} // init

	//--------------------------------------------------------

	void CLobbyClientState::release() 
	{
		CApplicationState::release();

	} // release

	//--------------------------------------------------------

	void CLobbyClientState::activate() 
	{
		CApplicationState::activate();
		_menu->show();
		Net::CManager::getSingletonPtr()->addObserver(this);
		// Activar la red
		Net::CManager::getSingletonPtr()->activateAsClient();

	} // activate

	//--------------------------------------------------------

	void CLobbyClientState::deactivate() 
	{	
		Net::CManager::getSingletonPtr()->removeObserver(this);
		_menu->hide();
		CApplicationState::deactivate();


	} // deactivate

	//--------------------------------------------------------

	void CLobbyClientState::tick(unsigned int msecs) {
		CApplicationState::tick(msecs);

	} // tick

	//--------------------------------------------------------

	void CLobbyClientState::dataPacketReceived(Net::CPaquete* packet) {
		Net::CBuffer buffer(packet->getDataLength());
		buffer.write(packet->getData(), packet->getDataLength());
		buffer.reset();

		Net::NetMessageType msg;
		//memcpy(&msg, packet->getData(), sizeof(msg));
		buffer.read(&msg, sizeof(msg));

		switch (msg) {
			case Net::SEND_PLAYER_INFO: {
				// Obtenemos el nickname del player, que de momento es la �nica informaci�n asociada al
				// player que tenemos
				std::string playerNick = _menu->callFunction("getNick",Hikari::Args()).getString();

				// Enviamos los datos del player al servidor
				Net::NetMessageType msg = Net::PLAYER_INFO;
				
				// El tama�o del buffer es: cabecera + entero para el tam del nick + num de letras del nick
				Net::CBuffer playerData( sizeof(msg) + sizeof(unsigned int) + (playerNick.size() * sizeof(char)) );
				playerData.write( &msg, sizeof(msg) ); // Por problemas con enumerados serializamos manualmente
				playerData.serialize(playerNick, false);

				Net::CManager::getSingletonPtr()->send( playerData.getbuffer(), playerData.getSize() );

				break;
			}
			case Net::LOAD_MAP: {
				// Cargamos el archivo con las definiciones de las entidades del nivel.
				std::string map;
				buffer.deserialize(map);
				if (!Logic::CEntityFactory::getSingletonPtr()->loadBluePrints("blueprints_client.txt")) {
					Net::CManager::getSingletonPtr()->deactivateNetwork();
					_app->exitRequest();
				}
				if (!Logic::CEntityFactory::getSingletonPtr()->loadArchetypes("archetypes.txt")) {
					Net::CManager::getSingletonPtr()->deactivateNetwork();
					_app->exitRequest();
				}
				// Cargamos el nivel a partir del nombre del mapa. 
				if (!Logic::CServer::getSingletonPtr()->loadLevel(map+"_client.txt")) {
					Net::CManager::getSingletonPtr()->deactivateNetwork();
					_app->exitRequest();
				}
				else {
					//Avisamos de que hemos terminado la carga.
					Net::NetMessageType ackMsg = Net::MAP_LOADED;
					Net::CBuffer ackBuffer(sizeof(ackMsg));

					ackBuffer.write(&ackMsg, sizeof(ackMsg));
					Net::CManager::getSingletonPtr()->send(ackBuffer.getbuffer(), ackBuffer.getSize());
				}

				break;
			}
			case Net::LOAD_PLAYER: {
				// Creamos el player
				Net::NetID id;
				Logic::TEntityID entityID;
				//memcpy(&id, packet->getData() + sizeof(msg), sizeof(id));
				buffer.read(&id, sizeof(id));
				buffer.read(&entityID, sizeof(entityID));
				std::string type, name;
				buffer.deserialize(type);
				buffer.deserialize(name);
				//llamo al metodo de creacion del jugador
				Logic::CEntity * player = Logic::CServer::getSingletonPtr()->getMap()->createPlayer(name, type, entityID);
				//Enviamos el mensaje de que se ha creado el jugador
				Net::NetMessageType ackMsg = Net::PLAYER_LOADED;
				Net::CBuffer ackBuffer(sizeof(ackMsg) + sizeof(id));
				ackBuffer.write(&ackMsg, sizeof(ackMsg));
				ackBuffer.write(&id, sizeof(id));
				Net::CManager::getSingletonPtr()->send(ackBuffer.getbuffer(), ackBuffer.getSize());
				
				break;
			}
			case Net::START_GAME: {
				_app->setState("gameClient");
				break;
			}
		}
	} // dataPacketReceived

	//--------------------------------------------------------

	bool CLobbyClientState::keyPressed(Input::TKey key)
	{
	   return false;

	} // keyPressed

	//--------------------------------------------------------

	bool CLobbyClientState::keyReleased(Input::TKey key)
	{
		switch(key.keyId)
		{
		case Input::Key::ESCAPE:
			Net::CManager::getSingletonPtr()->deactivateNetwork();
			_app->setState("netmenu");
			break;
		case Input::Key::RETURN:
			doStart(_menu->callFunction("getIp",Hikari::Args()).getString());
			break;
		default:
			return false;
		}
		return true;

	} // keyReleased

	//--------------------------------------------------------

	bool CLobbyClientState::mouseMoved(const Input::CMouseState &mouseState)
	{
		return true;

	} // mouseMoved

	//--------------------------------------------------------

	bool CLobbyClientState::mousePressed(const Input::CMouseState &mouseState)
	{
		return true;

	} // mousePressed

	//--------------------------------------------------------


	bool CLobbyClientState::mouseReleased(const Input::CMouseState &mouseState)
	{
		return true;

	} // mouseReleased

	//--------------------------------------------------------

	Hikari::FlashValue CLobbyClientState::startReleased(Hikari::FlashControl* caller, const Hikari::Arguments& args)
	{
		doStart(args.at(0).getString());

		return true;
	} // startReleased

	//--------------------------------------------------------

	Hikari::FlashValue CLobbyClientState::backReleased(Hikari::FlashControl* caller, const Hikari::Arguments& args)
	{
		Net::CManager::getSingletonPtr()->deactivateNetwork();
		_app->setState("netmenu");
		return true;

	} // backReleased

	//--------------------------------------------------------

	void CLobbyClientState::doStart(string ip)
	{
		// Conectamos
		if (!Net::CManager::getSingletonPtr()->connectTo((char*)ip.c_str(),1234,1)){
			_menu->callFunction("enableButton",Hikari::Args());
		}
	} // doStart

} // namespace Application