from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'data7'
config.General.workArea =  '2016G'

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'makingMiniBacon_DATA.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.outputFiles = ['Output.root']
config.section_("Data")
#config.Data.inputDataset = '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16DR80Premix-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext2-v1/AODSIM'
#config.Data.inputDataset = '/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/RunIISummer16DR80Premix-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/AODSIM'
#config.Data.inputDataset = '/WWTo2L2Nu_13TeV-powheg/RunIISummer16DR80Premix-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/AODSIM'
#config.Data.inputDataset = '/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/RunIISummer16DR80Premix-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/AODSIM'
#config.Data.inputDataset = '/ZZ_TuneCUETP8M1_13TeV-pythia8/RunIISummer16DR80Premix-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/AODSIM'
#config.Data.inputDataset = '/WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16DR80Premix-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/AODSIM'

#config.Data.inputDataset = '/SingleMuon/Run2016B-23Sep2016-v1/AOD'
#config.Data.inputDataset = '/SingleMuon/Run2016B-23Sep2016-v3/AOD'
#config.Data.inputDataset = '/SingleMuon/Run2016C-23Sep2016-v1/AOD'
#config.Data.inputDataset = '/SingleMuon/Run2016D-23Sep2016-v1/AOD'
#config.Data.inputDataset = '/SingleMuon/Run2016E-23Sep2016-v1/AOD'
#config.Data.inputDataset = '/SingleMuon/Run2016F-23Sep2016-v1/AOD'
config.Data.inputDataset = '/SingleMuon/Run2016G-23Sep2016-v1/AOD'
#config.Data.inputDataset = '/SingleMuon/Run2016H-PromptReco-v2/AOD'

config.Data.ignoreLocality = True
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
#config.Data.allowNonValidInputDataset = True
#config.Data.lumiMask = 'Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt'
config.Data.unitsPerJob = 10 
config.Data.outLFNDirBase = '/store/group/comm_luminosity/ZCounting'
#config.Data.outLFNDirBase = '/store/user/xniu/Moriond17MC'
config.Data.publication = False
config.Data.outputDatasetTag = 'Samples'
#config.Data.publishDataName = 'Samples'

config.section_("Site")
config.Site.storageSite = 'T2_CH_CERN'
