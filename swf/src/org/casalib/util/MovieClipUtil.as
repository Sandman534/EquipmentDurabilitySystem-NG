class org.casalib.util.MovieClipUtil
{
   function MovieClipUtil()
   {
   }
   static function attachMovieRegisterClass(className, target, linkageId, instanceName, depth, initObject)
   {
      Object.registerClass(linkageId,className);
      var _loc8_ = org.casalib.util.MovieClipUtil.attachMovie(target,linkageId,instanceName,depth,initObject);
      Object.registerClass(linkageId,null);
      return _loc8_;
   }
   static function createMovieRegisterClass(classPath, target, instanceName, depth, initObject)
   {
      return org.casalib.util.MovieClipUtil.attachMovieRegisterClass(eval(classPath),target,"__Packages." + classPath,instanceName,depth,initObject);
   }
   static function createEmptyMovieClip(target, instanceName, depth, initObject)
   {
      var _loc6_ = target.createEmptyMovieClip(instanceName,depth != undefined ? depth : target.getNextHighestDepth());
      for(var prop in initObject)
      {
         _loc6_[prop] = initObject[prop];
      }
      return _loc6_;
   }
   static function attachMovie(target, linkageId, instanceName, depth, initObject)
   {
      return target.attachMovie(linkageId,instanceName != undefined ? instanceName : linkageId,depth != undefined ? depth : target.getNextHighestDepth(),initObject);
   }
}
