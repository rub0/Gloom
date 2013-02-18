//---------------------------------------------------------------------------
// Overlay.h
//---------------------------------------------------------------------------

/**
@file Overlay.h

Contiene la declaraci�n de la clase que representa un Overlay.

@see Graphics::COverlay

@author Antonio Jesus Narvaez
@date Febrero, 2013
*/

#ifndef __Graphics_Overlay_H
#define __Graphics_Overlay_H

#include "BaseSubsystems/Math.h"
#include <map>

// Predeclaraci�n de clases para ahorrar tiempo de compilaci�n
namespace Ogre 
{
	class Overlay;
	class OverlayManager;
	class OverlayContainer;
	class TextAreaOverlayElement;
}

namespace Graphics 
{
	class CScene;
}

namespace Graphics 
{
	/**
	Clase que representa un Overlay. Contiene una referencia a
	una entidad de Ogre y al Nodo que la contiene.
	Oculta los detalles escabrosos del motor gr�fico.
	<p>
	
	
	@ingroup graphicsGroup

	@author Antonio Narvaez
	@date Febrero, 2013
	*/
	class COverlay 
	{
	public:

		/**
		Constructor de la clase.
		Genera un Overlay estandar, que poseera a su ver Contenedores con las respectivas imagenes.

		@param name Nombre del Overlay.

		*/
		//COverlay();

		/**
		Constructor de la clase.
		Genera un Overlay, puede ser TextArea o Panel, que luego sera introducido en un overlay

		@param name Nombre del Overlay.
		
		*/
		COverlay(const std::string &name, const std::string& type = NULL);

		/**
		Destructor de la aplicaci�n.
		*/
		virtual ~COverlay();

		/**
		Indica si un Overlay sera visible o invisible

		@param visible. Indicara si se ha de poner visible (true) u oculto (false)
		*/
		void setVisible(bool visible);

		/**
		Indica si un Overlay es visible o no

		@return Indicara si es visible (true) u oculto (false)
		*/
		bool isVisible();

		/**
		A�ade un oerlay a otro para establecer grupos. 

		@param overlayContainer, overlay que tendra ubicado dentro, tiene que ser un <b>Panel</b>.
		El material de estos debe de tener ciertas caracteristicas, mirar setMaterial para mas info.
		*/
		void add2D(COverlay* overlayContainer);

		/**
		A�ade una escena al overlay, para que esta se pinte superpuesta en la pantalla.
		IMPORTANTE: en el material del arma debe de tener indicado el la entrada "depth_check off"

		@param scene, escena que se a�adira al overlay.
		*/
		void add3D(CScene* scene);

		/**
		Establece donde se posicionara el Overlay.
		<p>
		Solo aplicable a los contenedores. Se puede usar en pixeles o en posiciones de 0 a 1 (Siendo 1,1 la esquina superior izquierda).
		Para cambiar de sistema de unidades usar el metodo 
		</p>
		@param left, establece el x donde comenzara el overlay
		@param top, establece el y donde comenzara el overlay
		orientarse la entidad.
		*/
		void setPosition(int left, int top);

		/**
		Establece las dimensiones del overlay.
		<p>
		Solo aplicable a los contenedores. Se puede usar en pixeles o en posiciones de 0 a 1 (Siendo 1,1 la esquina superior izquierda).
		Para cambiar de sistema de unidades usar el metodo 
		</p>
		@param width, establece el ancho del overlay
		@param height, establece el largo del overlay
		orientarse la entidad.
		*/
		void setDimensions(int width, int height);
		
		/**
		Establece la unidad de medida que se usara, debe indicarse por cada overlay. Por defecto se usa en pixeles.
		
		@param unit, unidad de medida que se usara, "pixel" para pixeles, "relative" para relativas
		*/
		void setMetricsMode(std::string unit);

		/**
		Solo aplicable a los contenedores. Asigna al overlay un material mediante su nombre. El material debera existir ya que no sino dara una petada.
		Dicho material poseera una textura con la imagen que se quiera mostrar por pantalla. El material debera de tener las siguientes propiedades.
		<ul>
		<li>depth_check off</li>
		<li>depth_write off</li>
		<li>scene_blend alpha_blend</li>
		<li>lighting off</li>
		<li>cull_hardware none</li>
		<li>cull_software none</li>
		</ul>

		@param material, nombre del material que tendra el overlay.
		*/
		void setMaterial(const std::string& material);

		/**
		Asigna un texto a un overlay

		@param text, texto que se le asiganara al overlay
		*/
		void setText(const std::string& text);

		/**
		Cambia el tama�o de la letra por el indicado

		@param textSize, tama�o de la letra
		*/
		void setTextSize(const int textSize);

		/**
		Especifica la fuente que se usara, la fuente ha de tener ciertas catacteristicas, mirar la carpeta fonts para mas info.

		@param font, fuente que se usara.
		*/
		void setFont(const std::string& font);


		/**
		A�ade un overlayContainer a otro, estableciendo una secuencia de padres e hijos.
		Asegurate que el overlay hijo es del tipo <b> TextArea </b> y el padre del tipo <b> Panel </b>
		@param child overlay que sera el hijo.
		*/
		void addChild(COverlay* child);

		/**
		metodo que devuelve el overlayContainer

		@return la variable que contiene el elemento
		*/
		Ogre::OverlayContainer* getOverlayContainer(){ return _overlayContainer;};
		
		/**
		metodo que devuelve el overlayContainer

		@return la variable que contiene el elemento
		*/
		Ogre::TextAreaOverlayElement* getOverlayText(){ return _overlayText;};
		
	protected:

		/*
		// CScene es la �nica que puede a�adir o eliminar entidades de una 
		// escena y por tanto cargar o descargar entidades.
		// Por otro lado cada entidad debe pertenecer a una escena. Solo 
		// permitimos a la escena actualizar el estado.
		friend class CScene;

		*/


		/**
		
		*/
		std::string _type;
		Ogre::Overlay *_overlay;
		Ogre::OverlayContainer *_overlayContainer;
		Ogre::TextAreaOverlayElement *_overlayText;


	}; // class COverlay

} // namespace Graphics

#endif // __Graphics_Overlay_H