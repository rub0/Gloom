/**
@file ShotGun.cpp

Contiene la implementaci�n del componente que representa a la escopeta.
 
@see Logic::ShotGun
@see Logic::CShootProjectile

@author Antonio Jesus Narv�ez 
@date mayo, 2013
*/

#include "ShotGun.h"


// Logica
#include "MagneticBullet.h"
#include "Logic/Maps/EntityFactory.h"
#include "Logic/Maps/Map.h"
#include "Logic/Entity/Entity.h"
#include "Logic/Server.h"
#include "PhysicDynamicEntity.h"
#include "Map/MapEntity.h"
#include "HudWeapons.h"

//Debug
#include "Logic/Messages/MessageHudDebugData.h"
#include "Logic/Messages/MessageCreateParticle.h"
#include "Logic/Messages/MessageDecal.h"

#include "Logic/Messages/MessageCreateParticle.h"
#include "Graphics/Particle.h"


namespace Logic {
	IMP_FACTORY(CShotGun);

	CShotGun::CShotGun() : IWeapon("shotGun"), 
		                             _dispersionAngle(0),
									 _primaryFireCooldownTimer(0) {

	}
	//________________________________________________

	CShotGun::~CShotGun() {
		// Nada que hacer
	}// CShotGun
	//________________________________________________

	bool CShotGun::spawn(CEntity* entity, CMap *map, const Map::CEntity *entityInfo){
		if(!IWeapon::spawn(entity, map, entityInfo))
			return false;

		// Nos aseguramos de tener todos los parametros que necesitamos
		assert( entityInfo->hasAttribute(_weaponName+"ShootForce") );
		assert( entityInfo->hasAttribute(_weaponName+"ProjectileRadius") );
		assert( entityInfo->hasAttribute(_weaponName+"PrimaryFireDispersion") );
		assert( entityInfo->hasAttribute(_weaponName+"DamageBurned") );
		assert( entityInfo->hasAttribute(_weaponName+"PrimaryFireCooldown") );
		assert( entityInfo->hasAttribute(_weaponName+"NumberOfShots") );

		assert( entityInfo->hasAttribute(_weaponName + "PrimaryFireDamage") );

		// Leemos los atributos
		_projectileShootForce = entityInfo->getFloatAttribute(_weaponName + "ShootForce");
		_projectileRadius = entityInfo->getFloatAttribute(_weaponName + "ProjectileRadius");
		_dispersionAngle  = entityInfo->getFloatAttribute(_weaponName+"PrimaryFireDispersion");
		_damageBurned = entityInfo->getFloatAttribute(_weaponName+"DamageBurned");
		_defaultPrimaryFireCooldown = _primaryFireCooldown = entityInfo->getFloatAttribute(_weaponName+"PrimaryFireCooldown") * 1000;
		_numberOfShots = entityInfo->getIntAttribute(_weaponName+"NumberOfShots");
		_defaultPrimaryFireDamage = _primaryFireDamage = entityInfo->getFloatAttribute(_weaponName + "PrimaryFireDamage");

		return true;
	}// spawn
	//________________________________________________

	void CShotGun::onActivate() {
		
		//Reiniciamos el cooldown
		reduceCooldown(0);
		//y los da�os
		amplifyDamage(0);
	}

	//__________________________________________________________________

	void CShotGun::secondaryFire() {
		// yo le digo que deben de volver, y a partir de ahi, ellas solas encuentran el camino :D
		if(!_projectiles.empty()){
			for(auto it = _projectiles.begin(); it != _projectiles.end(); ++it){
				(*it)->getComponent<CMagneticBullet>("CMagneticBullet")->setProjectileDirection(_entity->getPosition() + Vector3(0,_heightShoot, 0));
			} 
		}
	} // secondaryShoot
	//__________________________________________________________________

	void CShotGun::onTick(unsigned int msecs) {
		/*
		// Controlamos el cooldown del disparo primario y secundario
		if(_primaryFireCooldownTimer > 0) {
			_primaryFireCooldownTimer -= msecs;
			
			if(_primaryFireCooldownTimer < 0)
				_primaryFireCooldownTimer = 0;
		}
		*/
	}
	//__________________________________________________________________

	void CShotGun::primaryFire() {
		

		/*std::shared_ptr<CMessageCreateParticle> particleMsg = std::make_shared<CMessageCreateParticle>();
		particleMsg->setParticle("test");
		Vector3 position2 = this->getEntity()->getPosition();
		position2.y += _heightShoot;

		float fOffset = 2.0f;
		std::cout << "posicion= " << position2 ;
		Vector3 orientation = Math::getDirection(this->getEntity()->getOrientation())*fOffset;			
		position2 += orientation;

		particleMsg->setPosition(position2);
		_entity->emitMessage(particleMsg);*/

		int shots = _numberOfShots <= _currentAmmo ? _numberOfShots : _currentAmmo;
		for(int i = 0; i < shots; ++i) {
			Vector3 direction = _entity->getOrientation()*Vector3::NEGATIVE_UNIT_Z;
			Ogre::Radian angle = Ogre::Radian( (  (((float)(rand() % 100))*0.01f) * (_dispersionAngle)) *0.01f);
			Vector3 dispersionDirection = direction.randomDeviant(angle);
			dispersionDirection.normalise();

			Vector3 position = _entity->getPosition();
			position.y += _heightShoot;

			CEntity *projectileEntity= CEntityFactory::getSingletonPtr()->createEntity( 
				CEntityFactory::getSingletonPtr()->getInfo("MagneticBullet"),
				Logic::CServer::getSingletonPtr()->getMap(),
				position,
				Quaternion::IDENTITY
			);
			projectileEntity->activate();
			projectileEntity->start();
			_projectileShootForce = 0.0f;
			projectileEntity->getComponent<CMagneticBullet>("CMagneticBullet")->setProperties(this, _projectileShootForce, dispersionDirection, _heightShoot, _primaryFireDamage, _damageBurned);
			_projectiles.insert(projectileEntity);		
			
		}
			
	} // fireWeapon
	//_________________________________________________


	void CShotGun::destroyProjectile(CEntity *projectile, CEntity *killedBy){
		if(killedBy->getType() == "World"){
			drawDecal(killedBy, projectile->getPosition());

			// A�ado aqui las particulas de dado en la pared.
			auto m = std::make_shared<CMessageCreateParticle>();
			m->setPosition(projectile->getPosition());
			m->setParticle("impactParticle");
			// esto no es correcto en realidad, pero hasata que los decals esten en el otro lao, lo dejo asi.
			m->setDirectionWithForce(-(_entity->getOrientation()*Vector3::NEGATIVE_UNIT_Z));
			killedBy->emitMessage(m);
		}

		CEntityFactory::getSingletonPtr()->deferredDeleteEntity(projectile,true);
		_projectiles.erase(projectile);
	} // destroyProjectile
	//_________________________________________________
	
	void CShotGun::amplifyDamage(unsigned int percentage) {
		if(percentage == 0) {
			_primaryFireDamage = _defaultPrimaryFireDamage;
		}
		// Sino aplicamos el porcentaje pasado por par�metro
		else {
			_primaryFireDamage += percentage * _primaryFireDamage * 0.01f;
		}
	}
	//_________________________________________________

	void CShotGun::setCurrentAmmo(unsigned int ammo){
		_currentAmmo = ammo;
	} // setCurrentAmmo
	//_________________________________________________

} // namespace Logic

