#ifndef StiIstDetectorBuilder_h
#define StiIstDetectorBuilder_h

#include "Sti/StiDetectorBuilder.h"

class StIstDb;
class StiMaterial;
class StiPlacement;


class StiIstDetectorBuilder : public StiDetectorBuilder
{
public:

   StiIstDetectorBuilder(bool active, bool buildIdealGeom = true);
   virtual void buildDetectors(StMaker &source);
   virtual void useVMCGeometry();

protected:

   virtual void buildInactiveVolumes();

   bool          mBuildIdealGeom;
   StIstDb      *mIstDb;
};

#endif
