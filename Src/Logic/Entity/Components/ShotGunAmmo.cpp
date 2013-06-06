/**
@file ShotGun.cpp

Contiene la implementacion del componente
de disparo de la cabra.

@see Logic::CShotGunAmmo
@see Logic::IAmmo

@author Francisco Aisa Garc�a
@date Mayo, 2013
*/

#include "ShotGunAmmo.h"
#include "FireBallController.h"
#include "HudWeapons.h"

#include "Logic/Maps/EntityFactory.h"
#include "Logic/Maps/Map.h"
#include "Logic/Server.h"
#include "Map/MapEntity.h"

using namespace std;

namespace Logic {

	IMP_FACTORY(CShotGunAmmo);

	//__________________________________________________________________

	CShotGunAmmo::CShotGunAmmo() : IAmmo("shotGun"),
											_primaryFireIsActive(false),
											_elapsedTime(0),
											_ammoSpentTimer(0),
											_currentSpentAmmo(0),
											_primaryFireCooldownTimer(0) {
		// Nada que hacer
	}

	//__________________________________________________________________

	CShotGunAmmo::~CShotGunAmmo() {
		// Nada que hacer
	}

	//__________________________________________________________________

	bool CShotGunAmmo::spawn(CEntity* entity, CMap *map, const Map::CEntity *entityInfo) {
		if( !IAmmo::spawn(entity, map, entityInfo) ) return false;

		// Nos aseguramos de tener todos los atributos que necesitamos
		assert( entityInfo->hasAttribute(_weaponName + "PrimaryFireCooldown") );

		// Cooldown del disparo principal
		_defaultPrimaryFireCooldown = _primaryFireCooldown = entityInfo->getFloatAttribute(_weaponName + "PrimaryFireCooldown") * 1000;

		return true;
	}

	//__________________________________________________________________

	void CShotGunAmmo::onActivate() {
		_currentSpentAmmo = _ammoSpentTimer = _elapsedTime = 0;
	}

	//__________________________________________________________________

	void CShotGunAmmo::onAvailable() {
		IAmmo::onAvailable();
		_currentSpentAmmo = _ammoSpentTimer = _elapsedTime = 0;
	}

	//__________________________________________________________________

	void CShotGunAmmo::onTick(unsigned int msecs) {
		// Si el jugador esta dejando pulsado el disparo primario, aumentamos
		// el tama�o de la bola y reducimos la velocidad hasta un limite
		if(_primaryFireIsActive) {
			if(_currentAmmo > 0 && _currentSpentAmmo < _maxAmmoPerShot) {
				if(_elapsedTime < _maxLoadingTime) {
					// Contamos el tiempo que hemos mantenido pulsado el raton
					_elapsedTime += msecs;
					// Actualizamos el timer que se encarga de reducir la municion
					_ammoSpentTimer += msecs;
					if(_ammoSpentTimer >= _ammoSpentTimeStep) {
						decrementAmmo();
						++_currentSpentAmmo;
						_ammoSpentTimer = 0;
					}

					if(_elapsedTime >= _maxLoadingTime) {
						_elapsedTime = _maxLoadingTime;
					}
				}
			}
		}
		
		// Controlamos el cooldown
		if(_primaryFireCooldownTimer > 0) {
			_primaryFireCooldownTimer -= msecs;
			
			if(_primaryFireCooldownTimer < 0)
				_primaryFireCooldownTimer = 0;
		}
	}

	//__________________________________________________________________

	bool CShotGunAmmo::canUsePrimaryFire() {
		return _primaryFireCooldownTimer == 0 && _currentAmmo > 0;
	}

	//__________________________________________________________________

	bool CShotGunAmmo::canUseSecondaryFire() {
		return true;
	}

	//__________________________________________________________________

	void CShotGunAmmo::primaryFire() {
		IAmmo::primaryFire();

		_primaryFireCooldownTimer = _primaryFireCooldown;

		decrementAmmo();
		++_currentSpentAmmo;
	}

	//__________________________________________________________________

	void CShotGunAmmo::stopPrimaryFire() {
		IAmmo::stopPrimaryFire();
		
		if(!_primaryFireIsActive) return;

		_primaryFireIsActive = false;

		// Reseteamos el reloj
		_currentSpentAmmo = _ammoSpentTimer = _elapsedTime = 0;
	}

	//__________________________________________________________________

	void CShotGunAmmo::reduceCooldown(unsigned int percentage) {
		// Si es 0 significa que hay que restaurar al que habia por defecto,
		// sino decrementamos conforme al porcentaje dado.
		_primaryFireCooldown = percentage == 0 ? _defaultPrimaryFireCooldown : (_defaultPrimaryFireCooldown - (percentage * _primaryFireCooldown * 0.01f));
	}

}//namespace Logic