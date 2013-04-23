/**
@file SpectatorController.cpp

Contiene la implementaci�n del componente que controla el movimiento 
del espectador.
 
@see Logic::CSpectatorController
@see Logic::IComponent

@author Francisco Aisa Garc�a
@date Abril, 2013
*/

#include "SpectatorController.h"
#include "Logic/Entity/Entity.h"
#include "Map/MapEntity.h"
#include "Logic/Entity/Components/PhysicController.h"
#include "Logic/Entity/Components/Camera.h"

#include "Logic/Messages/MessageControl.h"
#include "Logic/Messages/MessageMouse.h"

namespace Logic {
	IMP_FACTORY(CSpectatorController);

	CSpectatorController::CSpectatorController() : _frictionCoef(0.95f),
												   _acceleration(0.0018f), 
												   _maxVelocity(2.0f) {
		// Inicializamos el array que contiene los vectores
		// de cada tecla de movimiento
		initMovementCommands();
	}

	//________________________________________________________________________

	bool CSpectatorController::spawn(CEntity *entity, CMap *map, const Map::CEntity *entityInfo) {
		if( !IComponent::spawn(entity,map,entityInfo) ) return false;

		assert( entityInfo->hasAttribute("acceleration") && "Error: No se ha definido el atributo acceleration en el mapa" );
		//_acceleration = entityInfo->getFloatAttribute("acceleration");
		//_acceleration = 0.0018f;

		assert( entityInfo->hasAttribute("maxVelocity") && "Error: No se ha definido el atributo maxVelocity en el mapa" );
		//_maxVelocity = entityInfo->getFloatAttribute("maxVelocity");
		//_maxVelocity = 2.0f;

		return true;

	} // spawn

	//________________________________________________________________________

	void CSpectatorController::activate() {
		IComponent::activate(); // Necesario para el onStart
		_displacementDir = _momentum = Vector3::ZERO;
	} // activate

	//________________________________________________________________________

	bool CSpectatorController::accept(CMessage *message) {
		return message->getMessageType() == Message::CONTROL;
	} // accept

	//________________________________________________________________________

	void CSpectatorController::process(CMessage *message) {
		switch( message->getMessageType() ) {
			case Message::CONTROL: {
				ControlType commandType = static_cast<CMessageControl*>(message)->getType();

				// Comando de movimiento
				if(commandType >=0 && commandType < 8) {
					executeMovementCommand(commandType);
				}
				// Comando de raton
				else if(commandType == Control::MOUSE) {
					CMessageMouse* mouseMsg = static_cast<CMessageMouse*>(message);
					mouse( mouseMsg->getMouse() );
				}

				break;
			}
		}

	} // process	

	//________________________________________________________________________

	void CSpectatorController::mouse(float XYturn[]) {
		_entity->yaw(XYturn[0]);
		_entity->pitch(XYturn[1]);
	} // turn

	//________________________________________________________________________

	void CSpectatorController::onStart(unsigned int msecs) {
		IComponent::onStart(msecs);

		// Nos quedamos con el puntero al componente CPhysicController
		// que es el que realmente se encargar� de desplazar al controlador
		// del jugador.
		_physicController = _entity->getComponent<CPhysicController>("CPhysicController");
		assert(_physicController && "Error: El player no tiene un controlador fisico");
	}

	//________________________________________________________________________

	void CSpectatorController::tick(unsigned int msecs) {
		//@deprecated
		IComponent::tick(msecs);

		// Calculamos el vector de desplazamiento
		estimateMotion(msecs);

		// Movemos la c�psula en la direcci�n estimada por el vector de
		// desplazamiento y acumulada en el vector momentum
		_physicController->move(_momentum, msecs);
	} // tick

	//________________________________________________________________________

	Vector3 CSpectatorController::estimateMotionDirection() {
		// Si nuestro movimiento es nulo no hacemos nada
		if(_displacementDir == Vector3::ZERO) return Vector3::ZERO;

		// Mediante trigonometria basica sacamos el angulo que forma el desplazamiento
		// que queremos llevar a cabo
		float displacementYaw = asin(_displacementDir.normalisedCopy().x);
		// Obtenemos una copia de la matriz de transformaci�n del personaje
		Matrix4 characterTransform = _entity->getTransform();
		// Si estamos andando hacia atras, invertimos el giro
		if(_displacementDir.z < 0) displacementYaw *= -1;

		// Rotamos la matriz de transformacion tantos grados como diga el vector 
		// desplazamiento calculado de pulsar las teclas
		Math::yaw(displacementYaw, characterTransform);
		// Obtenemos el vector unitario de orientaci�n de la matriz de transformaci�n
		Vector3 motionDirection = Math::getDirection(characterTransform);
		// Invertimos el vector resultante si nos estamos desplazando hacia atras
		// porque el yaw se calcula de forma contraria al andar hacia atras
		if(_displacementDir.z < 0) motionDirection *= -1;

		// Devolvemos la direcci�n del movimiento estimado
		return motionDirection;
	}

	//________________________________________________________________________

	void CSpectatorController::estimateMotion(unsigned int msecs) {
		// Aplicamos un coeficiente de rozamiento constante
		// OJO! Al no tener en cuenta los msecs, los clientes que tengan
		// un mayor frame rate se desplazaran de forma distinta
		_momentum *= _frictionCoef;

		// s = u � t + 1/2 a � t^2 <- Formula del desplazamiento
		if(_displacementDir.z == 0)
			_momentum += Vector3(1, 0, 1) * estimateMotionDirection() * _acceleration * msecs * msecs * 0.5f;
		else
			_momentum += estimateMotionDirection() * _acceleration * msecs * msecs * 0.5f;

		// Seteamos la velocidad m�xima a la que se puede ir
		normalizeDirection();
	}

	//________________________________________________________________________

	void CSpectatorController::normalizeDirection() {
		// Normalizamos la velocidad al m�ximo
		float momVelocity = _momentum.length();
		if(momVelocity >_maxVelocity) {
			double coef = _maxVelocity / momVelocity;
			_momentum *= coef;
			
		}
	}

	//________________________________________________________________________

	void CSpectatorController::executeMovementCommand(ControlType commandType) {
		_displacementDir += _movementCommands[commandType];
	}

	//________________________________________________________________________

	void CSpectatorController::initMovementCommands() {
		_movementCommands[Control::WALK]				= Vector3(0,0,1);
		_movementCommands[Control::WALKBACK]			= Vector3(0,0,-1);
		_movementCommands[Control::STRAFE_LEFT]			= Vector3(1,0,0);
		_movementCommands[Control::STRAFE_RIGHT]		= Vector3(-1,0,0);
		_movementCommands[Control::STOP_WALK]			= Vector3(0,0,-1);
		_movementCommands[Control::STOP_WALKBACK]		= Vector3(0,0,1);
		_movementCommands[Control::STOP_STRAFE_LEFT]	= Vector3(-1,0,0);
		_movementCommands[Control::STOP_STRAFE_RIGHT]	= Vector3(1,0,0);
	}

} // namespace Logic