#ifndef __Logic_ShootShotGun_H
#define __Logic_ShootShotGun_H

#include "Logic/Entity/Components/Shoot.h"

//declaraci�n de la clase
namespace Logic 
{
/**
	Este componente controla la capacidad de disparo de una entidad, asi como las armas que puede tener. Procesa mensajes de tipo 
	SHOOT (indican que la entidad ha disparado)
	<p>
	Poseera una vector, con las armas que tiene, asi como su da�o y alcance.
	Tambien poseera un vector de booleanos que indicara las armas que posee.

*/


	class CShootShotGun : public CShoot
	{
		DEC_FACTORY(CShootShotGun);

	public:

		/**
		Constructor por defecto; en la clase base no hace nada.
		*/

		CShootShotGun() : CShoot("ShotGun") {}
	};

	REG_FACTORY(CShootShotGun);

} // namespace Logic

#endif // __Logic_Shoot_H