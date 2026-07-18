class Main
{
   function Main()
   {
   }
   static function mainStart(swfRoot)
   {
      DurabilityWidget(org.casalib.util.MovieClipUtil.createMovieRegisterClass("DurabilityWidget.as",swfRoot,"widget",swfRoot.getNextHighestDepth(),{_x:0,_y:0}));
      swfRoot.widget = new DurabilityWidget(swfRoot);
   }
}
