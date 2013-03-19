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

#include <CEGUISystem.h>
#include <CEGUIWindowManager.h>
#include <CEGUIWindow.h>
#include <elements/CEGUIPushButton.h>

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
		CEGUI::WindowManager::getSingletonPtr()->loadWindowLayout("NetLobbyClient.layout");
		_menuWindow = CEGUI::WindowManager::getSingleton().getWindow("NetLobbyClient");
		
		// Asociamos los botones del men� con las funciones que se deben ejecutar.
		CEGUI::WindowManager::getSingleton().getWindow("NetLobbyClient/Connect")->
			subscribeEvent(CEGUI::PushButton::EventClicked, 
				CEGUI::SubscriberSlot(&CLobbyClientState::startReleased, this));
		
		CEGUI::WindowManager::getSingleton().getWindow("NetLobbyClient/Back")->
			subscribeEvent(CEGUI::PushButton::EventClicked, 
				CEGUI::SubscriberSlot(&CLobbyClientState::backReleased, this));
	
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

		// Activamos la ventana que nos muestra el men� y activamos el rat�n.
		CEGUI::System::getSingletonPtr()->setGUISheet(_menuWindow);
		_menuWindow->setVisible(true);
		_menuWindow->activate();
		CEGUI::MouseCursor::getSingleton().show();

		Net::CManager::getSingletonPtr()->addObserver(this);
		// Activar la red
		Net::CManager::getSingletonPtr()->activateAsClient();

	} // activate

	//--------------------------------------------------------

	void CLobbyClientState::deactivate() 
	{	
		Net::CManager::getSingletonPtr()->removeObserver(this);

		// Desactivamos la ventana GUI con el men� y el rat�n.
		CEGUI::MouseCursor::getSingleton().hide();
		_menuWindow->deactivate();
		_menuWindow->setVisible(false);

		CApplicationState::deactivate();


	} // deactivate

	//--------------------------------------------------------

	void CLobbyClientState::tick(unsigned int msecs) 
	{
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

		switch (msg)
		{
		case Net::LOAD_PLAYER_INFO:
		{
			// Obtenemos el nombre del mesh que va a usar el player
			std::string playerModel = std::string( CEGUI::WindowManager::getSingleton().getWindow("NetLobbyClient/ModelBox")->getText().c_str() );

			// Obtenemos el nombre del player
			std::string playerNick = std::string( CEGUI::WindowManager::getSingleton().getWindow("NetLobbyClient/NickBox")->getText().c_str() );

			// Enviamos los datos del player al servidor
			Net::NetMessageType msg = Net::PLAYER_INFO;

			Net::CBuffer playerData(sizeof(msg) + playerNick.size() + playerModel.size());
			playerData.write(&msg, sizeof(msg)); // Por problemas con enumerados serializamos manualmente
			playerData.serialize(playerNick, false);
			playerData.serialize(playerModel, false);
		
			Net::CManager::getSingletonPtr()->send(playerData.getbuffer(), playerData.getSize());
			
			break;
		}
		case Net::LOAD_MAP:
			// Cargamos el archivo con las definiciones de las entidades del nivel.
			if (!Logic::CEntityFactory::getSingletonPtr()->loadBluePrints("blueprints_client.txt"))
			{
				Net::CManager::getSingletonPtr()->deactivateNetwork();
				_app->exitRequest();
			}
			if (!Logic::CEntityFactory::getSingletonPtr()->loadArchetypes("archetypes.txt")) {
				Net::CManager::getSingletonPtr()->deactivateNetwork();
				_app->exitRequest();
			}
			// Cargamos el nivel a partir del nombre del mapa. 
			if (!Logic::CServer::getSingletonPtr()->loadLevel("map_client.txt"))
			{
				Net::CManager::getSingletonPtr()->deactivateNetwork();
				_app->exitRequest();
			}
			else
			{
				//Avisamos de que hemos terminado la carga.
				Net::NetMessageType ackMsg = Net::MAP_LOADED;
				Net::CBuffer ackBuffer(sizeof(ackMsg));

				ackBuffer.write(&ackMsg, sizeof(ackMsg));
				Net::CManager::getSingletonPtr()->send(ackBuffer.getbuffer(), ackBuffer.getSize());
			}
			break;
		case Net::LOAD_PLAYER:
			{
			// Creamos el player. Deber�amos poder propocionar caracter�asticas
			// diferentes seg�n el cliente (nombre, modelo, etc.). Esto es una
			// aproximaci�n, solo cambiamos el nombre y decimos si es el jugador
			// local. Los datos deber�an llegar en el paquete de red.
			Net::NetID id;
			Logic::TEntityID entityID;
			//memcpy(&id, packet->getData() + sizeof(msg), sizeof(id));
			buffer.read(&id, sizeof(id));
			buffer.read(&entityID, sizeof(entityID));
			std::string name;
			buffer.deserialize(name);

			//llamo al metodo de creacion del jugador
			if(id == Net::CManager::getSingletonPtr()->getID()) {//si soy yo, me creo como jugador local
				Logic::CEntity * player = Logic::CServer::getSingletonPtr()->getMap()->createLocalPlayer(name, entityID);
			}else{//si no soy yo, me creo como jugador remoto
				Logic::CEntity * player = Logic::CServer::getSingletonPtr()->getMap()->createPlayer(name, entityID);
			}
			
			//Enviamos el mensaje de que se ha creado el jugador
			Net::NetMessageType ackMsg = Net::PLAYER_LOADED;
			Net::CBuffer ackBuffer(sizeof(ackMsg) + sizeof(id));
			ackBuffer.write(&ackMsg, sizeof(ackMsg));
			ackBuffer.write(&id, sizeof(id));
			Net::CManager::getSingletonPtr()->send(ackBuffer.getbuffer(), ackBuffer.getSize());
			}
			break;
		case Net::START_GAME:
			_app->setState("multiplayerTeamDeathmatchClient");
			break;
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
			doStart();
			break;
		default:
			return false;
		}
		return true;

	} // keyReleased

	//--------------------------------------------------------
	
	bool CLobbyClientState::mouseMoved(const Input::CMouseState &mouseState)
	{
		return false;

	} // mouseMoved

	//--------------------------------------------------------
		
	bool CLobbyClientState::mousePressed(const Input::CMouseState &mouseState)
	{
		//CEGUI::System::getSingleton().injectMouseButtonDown(mouseState.);
		return false;

	} // mousePressed

	//--------------------------------------------------------


	bool CLobbyClientState::mouseReleased(const Input::CMouseState &mouseState)
	{
		return false;

	} // mouseReleased
			
	//--------------------------------------------------------
		
	bool CLobbyClientState::startReleased(const CEGUI::EventArgs& e)
	{
		doStart();
		return true;
	} // startReleased
			
	//--------------------------------------------------------

	bool CLobbyClientState::backReleased(const CEGUI::EventArgs& e)
	{
		Net::CManager::getSingletonPtr()->deactivateNetwork();
		_app->setState("netmenu");
		return true;

	} // backReleased
			
	//--------------------------------------------------------

	void CLobbyClientState::doStart()
	{
		// Deshabilitamos el bot�n de Start
		CEGUI::WindowManager::getSingleton().getWindow("NetLobbyClient/Connect")->setEnabled(false);
		
		// Actualizamos el status
		CEGUI::Window * status = CEGUI::WindowManager::getSingleton().getWindow("NetLobbyClient/Status");
		status->setText("Status: Connecting...");

		// Obtenemos la ip desde el Editbox
		CEGUI::String ip = CEGUI::WindowManager::getSingleton().getWindow("NetLobbyClient/IPBox")->getText();

		// Conectamos
		Net::CManager::getSingletonPtr()->connectTo((char*)ip.c_str(),1234,1);

		// Actualizamos el status
		status->setText("Status: Connected to server. Waiting to start game...");

		//tickNet is in charge of receiving the message and change the state

	} // doStart

} // namespace Application
