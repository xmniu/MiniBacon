#ifndef MINIBACON_DATAFORMATS_LINKDEF_H
#define MINIBACON_DATAFORMATS_LINKDEF_H
#include "MiniBacon/DataFormats/interface/TEventInfo.hh"
#include "MiniBacon/DataFormats/interface/TMuon.hh"
#include "MiniBacon/DataFormats/interface/TVertex.hh"
#endif

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;
#pragma link C++ namespace baconhep;

#pragma link C++ class baconhep::TEventInfo+;
#pragma link C++ class baconhep::TMuon+;
#pragma link C++ class baconhep::TVertex+;
#endif
