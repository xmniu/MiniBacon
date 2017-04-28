#include "NtuplerMod.hh"

// bacon classes and constants
#include "MiniBacon/DataFormats/interface/MiniBaconDefs.hh"
#include "MiniBacon/DataFormats/interface/TEventInfo.hh"
#include "MiniBacon/DataFormats/interface/TMuon.hh"
#include "MiniBacon/DataFormats/interface/TVertex.hh"
#include "MiniBacon/Utils/interface/TTrigger.hh"

#include "MiniBacon/DataFillers/interface/FillerEventInfo.hh"
#include "MiniBacon/DataFillers/interface/FillerVertex.hh"
#include "MiniBacon/DataFillers/interface/FillerMuon.hh"

// tools to parse HLT name patterns
#include <boost/foreach.hpp>
#include "FWCore/Utilities/interface/RegexMatch.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

// data format classes
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

// ROOT classes
#include <TFile.h>
#include <TH1D.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TMath.h>


//--------------------------------------------------------------------------------------------------
NtuplerMod::NtuplerMod(const edm::ParameterSet &iConfig):
  fSkipOnHLTFail     (iConfig.getUntrackedParameter<bool>("skipOnHLTFail",false)),
  fHLTTag            ("TriggerResults","","HLT"),
  fHLTObjTag         ("hltTriggerSummaryAOD","","HLT"),
  fHLTFile           (iConfig.getUntrackedParameter<std::string>("TriggerFile","HLT")),
  fPVName            (iConfig.getUntrackedParameter<std::string>("edmPVName","offlinePrimaryVertices")),
  fFillerEvtInfo     (0),
  fFillerPV          (0),
  fFillerMuon        (0),
  fTrigger           (0),
  fIsActiveEvtInfo   (false),
  fIsActivePV        (false),
  fIsActiveMuon      (false),
  fOutputName        (iConfig.getUntrackedParameter<std::string>("outputName", "ntuple.root")),
  fOutputFile        (0),
  fTotalEvents       (0),
  fEventTree         (0),
  fEvtInfo           (0),
  fMuonArr           (0),
  fPVArr             (0)
{
  fHLTTag_token = consumes<edm::TriggerResults>(fHLTTag);
  fHLTObjTag_token = consumes<trigger::TriggerEvent>(fHLTObjTag);
  fPVName_token = consumes<reco::VertexCollection>(fPVName);
  // Don't write TObject part of the objects
  baconhep::TEventInfo::Class()->IgnoreTObjectStreamer();
  baconhep::TMuon::Class()->IgnoreTObjectStreamer();
  baconhep::TVertex::Class()->IgnoreTObjectStreamer();
  
  //
  // Set up bacon objects and configure fillers
  // 
  if(iConfig.existsAs<edm::ParameterSet>("Info",false)) {
    edm::ParameterSet cfg(iConfig.getUntrackedParameter<edm::ParameterSet>("Info"));
    fIsActiveEvtInfo = cfg.getUntrackedParameter<bool>("isActive");

    if(fIsActiveEvtInfo) {
      fEvtInfo       = new baconhep::TEventInfo();            assert(fEvtInfo);
      fFillerEvtInfo = new baconhep::FillerEventInfo(cfg,consumesCollector());    assert(fFillerEvtInfo);
    }
  }

  if(iConfig.existsAs<edm::ParameterSet>("PV",false)) {
    edm::ParameterSet cfg(iConfig.getUntrackedParameter<edm::ParameterSet>("PV"));
    fIsActivePV = cfg.getUntrackedParameter<bool>("isActive");
    
    // create array and filler even if vertices won't be saved to output (i.e. fIsActivePV == false),
    // because FillerVertex::fill(...) is used to find the event primary vertex
    // (not elegant, but I suppose a dedicated PV finding function can be implemented somewhere...)
    fPVArr    = new TClonesArray("baconhep::TVertex"); assert(fPVArr);
    fFillerPV = new baconhep::FillerVertex(cfg);       assert(fFillerPV);
  }

  if(iConfig.existsAs<edm::ParameterSet>("Muon",false)) {
    edm::ParameterSet cfg(iConfig.getUntrackedParameter<edm::ParameterSet>("Muon"));
    fIsActiveMuon = cfg.getUntrackedParameter<bool>("isActive");
    if(fIsActiveMuon) {
      fMuonArr    = new TClonesArray("baconhep::TMuon"); assert(fMuonArr);
      fFillerMuon = new baconhep::FillerMuon(cfg,consumesCollector());       assert(fFillerMuon);
    }
  }  
}

//--------------------------------------------------------------------------------------------------
NtuplerMod::~NtuplerMod()
{
  delete fFillerEvtInfo;
  delete fFillerPV;
  delete fFillerMuon;
  
  delete fTrigger;
  delete fEvtInfo;
  delete fMuonArr;
  delete fPVArr;
}

//--------------------------------------------------------------------------------------------------
void NtuplerMod::beginJob()
{  
  //
  // Create output file, trees, and histograms
  //
  fOutputFile  = new TFile(fOutputName.c_str(), "RECREATE");
  fTotalEvents = new TH1D("TotalEvents","TotalEvents",1,-10,10);
  fEventTree   = new TTree("Events","Events");
  
  if(fIsActiveEvtInfo){ fEventTree->Branch("Info",fEvtInfo); }
  if(fIsActiveMuon)   { fEventTree->Branch("Muon",     &fMuonArr); }
  if(fIsActivePV)     { fEventTree->Branch("PV",       &fPVArr); }
  //
  // Triggers
  //
  setTriggers();
}

//--------------------------------------------------------------------------------------------------
void NtuplerMod::endJob() 
{
  //
  // Save to ROOT file
  //
  //fEventTree->Print();
  fOutputFile->cd();
  fTotalEvents->Write();
  fOutputFile->Write();
  fOutputFile->Close();

}

//--------------------------------------------------------------------------------------------------
void NtuplerMod::setTriggers()
{
  std::string cmssw_base_src = getenv("CMSSW_BASE"); cmssw_base_src+="/src/";
  fTrigger = new baconhep::TTrigger(cmssw_base_src + fHLTFile);
}

//--------------------------------------------------------------------------------------------------
void NtuplerMod::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  fTotalEvents->Fill(1);
  
  edm::Handle<edm::TriggerResults> hTrgRes;
  //iEvent.getByLabel(fHLTTag,hTrgRes);
  iEvent.getByToken(fHLTTag_token,hTrgRes);
  assert(hTrgRes.isValid());  
  const edm::TriggerNames &triggerNames = iEvent.triggerNames(*hTrgRes);
  Bool_t config_changed = false;
  if(fTriggerNamesID != triggerNames.parameterSetID()) {
    fTriggerNamesID = triggerNames.parameterSetID();
    config_changed  = true;
  }
  if(config_changed) {
    initHLT(*hTrgRes, triggerNames);
  }
  
  TriggerBits triggerBits;
  for(unsigned int irec=0; irec<fTrigger->fRecords.size(); irec++) {
    if(fTrigger->fRecords[irec].hltPathIndex == (unsigned int)-1) continue;
    if(hTrgRes->accept(fTrigger->fRecords[irec].hltPathIndex)) {
      triggerBits [fTrigger->fRecords[irec].baconTrigBit] = 1;
    }
  }
  if(fSkipOnHLTFail && triggerBits == 0) return;  


  fPVArr->Clear();
  int nvertices = 0;
  const reco::Vertex *pv = fFillerPV->fill(fPVArr, nvertices, iEvent);
  assert(pv);
  
  if(fIsActiveEvtInfo) {
    fFillerEvtInfo->fill(fEvtInfo, iEvent, *pv, (nvertices>0), triggerBits);//,fSusyGen);
  }
  
  edm::Handle<trigger::TriggerEvent> hTrgEvt;
  iEvent.getByToken(fHLTObjTag_token,hTrgEvt);
  
  if(fIsActiveMuon) {
    fMuonArr->Clear();  
    fFillerMuon->fill(fMuonArr, iEvent, iSetup, *pv, fTrigger->fRecords, *hTrgEvt);
  }

  fEventTree->Fill();
}

//--------------------------------------------------------------------------------------------------
void NtuplerMod::initHLT(const edm::TriggerResults& result, const edm::TriggerNames& triggerNames)
{
  for(unsigned int irec=0; irec<fTrigger->fRecords.size(); irec++) {
    fTrigger->fRecords[irec].hltPathName  = "";
    fTrigger->fRecords[irec].hltPathIndex = (unsigned int)-1;
    const std::string pattern = fTrigger->fRecords[irec].hltPattern;
    if(edm::is_glob(pattern)) {  // handle pattern with wildcards (*,?)
      std::vector<std::vector<std::string>::const_iterator> matches = edm::regexMatch(triggerNames.triggerNames(), pattern);
      if(matches.empty()) {
        std::cout << "requested pattern [" << pattern << "] does not match any HLT paths" << std::endl;
      } else {
        BOOST_FOREACH(std::vector<std::string>::const_iterator match, matches) {
          fTrigger->fRecords[irec].hltPathName = *match;
        }
      }
    } else {  // take full HLT path name given
      fTrigger->fRecords[irec].hltPathName = pattern;
    }
    // Retrieve index in trigger menu corresponding to HLT path
    unsigned int index = triggerNames.triggerIndex(fTrigger->fRecords[irec].hltPathName);
    if(index < result.size()) {  // check for valid index
      fTrigger->fRecords[irec].hltPathIndex = index;
    }
  }
}

//--------------------------------------------------------------------------------------------------
void NtuplerMod::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
void NtuplerMod::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup){}
void NtuplerMod::endRun  (const edm::Run& iRun, const edm::EventSetup& iSetup){}
void NtuplerMod::beginLuminosityBlock(const edm::LuminosityBlock& iLumi, const edm::EventSetup& iSetup){}
void NtuplerMod::endLuminosityBlock  (const edm::LuminosityBlock& iLumi, const edm::EventSetup& iSetup){}


//define this as a plug-in
DEFINE_FWK_MODULE(NtuplerMod);
