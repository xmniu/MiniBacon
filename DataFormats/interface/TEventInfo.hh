#ifndef MINIBACON_DATAFORMATS_TEVENTINFO_HH
#define MINIBACON_DATAFORMATS_TEVENTINFO_HH

#include "MiniBacon/DataFormats/interface/MiniBaconDefs.hh"
#include <TObject.h>

namespace baconhep 
{
  class TEventInfo : public TObject
  {
    public:
      TEventInfo():
      runNum(0), evtNum(0), lumiSec(0),
      nPU(0), nPUm(0), nPUp(0),
      nPUmean(0), nPUmeanm(0), nPUmeanp(0),
      pvx(0), pvy(0), pvz(0),
      bsx(0), bsy(0), bsz(0), 
      triggerBits(0),
      hasGoodPV(false)
      {}
      ~TEventInfo(){}

      unsigned int  runNum, evtNum, lumiSec;                   // run number, event number, lumi section in data
      unsigned int  nPU, nPUm, nPUp;                           // number of reconstructed pile-up vertices
      float         nPUmean, nPUmeanm, nPUmeanp;               // expected number of reconstructed pile-up vertices
      float         pvx, pvy, pvz;                             // best primary vertex
      float         bsx, bsy, bsz;                             // beamspot
      TriggerBits   triggerBits;                               // fired trigger bits
      bool          hasGoodPV;                                 // event has a good PV?

    ClassDef(TEventInfo,3)
  };
}
#endif
