package  
{
	import flash.ui.Mouse;
	import flash.display.MovieClip;
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.external.ExternalInterface;


	
	/**
	 * ...
	 * @author Rubén Mulero
	 */
	public class MenuManager extends MovieClip 
	{

		public function MenuManager() {
			Mouse.hide();
			
			ExternalInterface.addCallback("loadMenu", loadMenu );
		}
		
	//////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////  METHODS  //////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////
		
		
		function loadMenu(menu: int): void {
			gotoAndPlay(menu);
		}
		
	//////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////  LISTENERS  ////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////
		

	}
}