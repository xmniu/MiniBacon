import FWCore.ParameterSet.Config as cms

process = cms.Process('MakingBacon')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration/StandardSequences/GeometryDB_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_condDBv2_cff')
process.load('Configuration/EventContent/EventContent_cff')
process.load('TrackingTools/TransientTrack/TransientTrackBuilder_cfi')

process.MessageLogger.cerr.FwkReport.reportEvery = 100
#process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v8'
process.GlobalTag.globaltag = '80X_dataRun2_2016SeptRepro_v3'

# trigger filter
import os
cmssw_base = os.environ['CMSSW_BASE']
hlt_filename = "MiniBacon/DataFormats/data/HLT_50nsGRun"
process.load('HLTrigger/HLTfilters/hltHighLevel_cfi')
process.hltHighLevel.throw = cms.bool(False)
process.hltHighLevel.HLTPaths = cms.vstring()

from CondCore.CondDB.CondDB_cfi import *

hlt_file = open(cmssw_base + "/src/" + hlt_filename, "r")
for line in hlt_file.readlines():
  line = line.strip()              # strip preceding and trailing whitespaces
  if (line[0:3] == 'HLT'):         # assumes typical lines begin with HLT path name (e.g. HLT_Mu15_v1)
    hlt_path = line.split()[0]
    process.hltHighLevel.HLTPaths.extend(cms.untracked.vstring(hlt_path))
    
    process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )
    process.source = cms.Source("PoolSource",
				fileNames = cms.untracked.vstring(
'/store/data/Run2016H/SingleMuon/RECO/PromptReco-v2/000/281/693/00000/000146F7-1686-E611-BD78-FA163E90B8E2.root',
				)
                                )
    process.source.inputCommands = cms.untracked.vstring("keep *",
                                                         "drop *_MEtoEDMConverter_*_*")
    
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool(False),
  Rethrow     = cms.untracked.vstring('ProductNotFound'),
  fileMode    = cms.untracked.string('NOMERGE')
  )

is_data_flag = True 
do_hlt_filter = False
process.ntupler = cms.EDAnalyzer('NtuplerMod',
                                 kipOnHLTFail = cms.untracked.bool(do_hlt_filter),
                                 outputName    = cms.untracked.string('Output.root'),
                                 TriggerFile   = cms.untracked.string(hlt_filename),
                                 edmPVName     = cms.untracked.string('offlinePrimaryVertices'),
                                 edmPFCandName = cms.untracked.string('particleFlow'),
                                 
                                 Info = cms.untracked.PSet(
    isActive             = cms.untracked.bool(True),
    edmPileupInfoName    = cms.untracked.string('addPileupInfo'),
    edmBeamspotName      = cms.untracked.string('offlineBeamSpot')
    ),
                                 
                                 PV = cms.untracked.PSet(
    isActive      = cms.untracked.bool(True),   
    edmName       = cms.untracked.string('offlinePrimaryVertices'),
    minNTracksFit = cms.untracked.uint32(0),
    minNdof       = cms.untracked.double(4),
    maxAbsZ       = cms.untracked.double(24),
    maxRho        = cms.untracked.double(2)
    ),
                                     
                                 Muon = cms.untracked.PSet(
    isActive      = cms.untracked.bool(True),
    minPt         = cms.untracked.double(20),
    edmName       = cms.untracked.string('muons'),
    edmPFCandName = cms.untracked.string('particleFlow'),
        
    # save general tracker tracks in our muon collection (used in tag-and-probe for muons)
    doSaveTracks = cms.untracked.bool(True),
    minTrackPt   = cms.untracked.double(20),
    edmTrackName = cms.untracked.string('generalTracks')
    
    ),
                                     
                                 )

process.baconSequence = cms.Sequence(
  process.ntupler)
    
process.p = cms.Path(process.baconSequence)
