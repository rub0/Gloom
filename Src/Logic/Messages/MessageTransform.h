#ifndef __Logic_MessageTransform_H
#define __Logic_MessageTransform_H

#include "Message.h"

namespace Logic {

	class CMessageTransform : public CMessage{
	DEC_FACTORYMESSAGE(CMessageTransform);
	public:
		CMessageTransform();
		Matrix4 getTransform();
		void setTransform(Matrix4 transform);
		~CMessageTransform(){};
		
		virtual Net::CBuffer* serialize();
		// En el deserialize llamar primero posicion y luego pitch y roll

	private:
		Matrix4 _transform;
	};
	REG_FACTORYMESSAGE(CMessageTransform);
};

#endif