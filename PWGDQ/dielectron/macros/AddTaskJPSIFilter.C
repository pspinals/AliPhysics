AliAnalysisTaskDielectronFilter* AddTaskJPSIFilter(Bool_t storeLS = kTRUE, Bool_t hasMC_aod = kFALSE, Bool_t storeTR = kTRUE){
  //get the current analysis manager
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    Error("AddTaskJPSIFilter", "No analysis manager found.");
    return 0;
  }
  
  //check for output aod handler
  if (!mgr->GetOutputEventHandler()||mgr->GetOutputEventHandler()->IsA()!=AliAODHandler::Class()) {
    Warning("AddTaskJPSIFilter","No AOD output handler available. Not adding the task!");
    return 0;
  }

  //Do we have an MC handler?
  Bool_t hasMC=(AliAnalysisManager::GetAnalysisManager()->GetMCtruthEventHandler()!=0x0)||hasMC_aod;
  
  //Do we run on AOD?
  Bool_t isAOD=mgr->GetInputEventHandler()->IsA()==AliAODInputHandler::Class();

  gROOT->LoadMacro("$ALICE_ROOT/PWG3/dielectron/macros/ConfigJpsi2eeFilter.C");
  AliDielectron *jpsi = reinterpret_cast<AliDielectron *>(gInterpreter->ProcessLine(Form("ConfigJpsi2eeFilter(%d)", isAOD)));
  
  if(isAOD) {
    //add options to AliAODHandler to duplicate input event
    AliAODHandler *aodHandler = (AliAODHandler*)mgr->GetOutputEventHandler();
    aodHandler->SetCreateNonStandardAOD();
    aodHandler->SetNeedsHeaderReplication();
    aodHandler->SetNeedsTracksBranchReplication();
    aodHandler->SetNeedsVerticesBranchReplication();
    aodHandler->SetNeedsV0sBranchReplication();
    aodHandler->SetNeedsCascadesBranchReplication();
    aodHandler->SetNeedsTrackletsBranchReplication();
    aodHandler->SetNeedsPMDClustersBranchReplication();
    aodHandler->SetNeedsJetsBranchReplication();
    aodHandler->SetNeedsFMDClustersBranchReplication();
    aodHandler->SetNeedsCaloClustersBranchReplication();
    //aodHandler->SetNeedsMCParticlesBranchReplication();
    aodHandler->SetNeedsDimuonsBranchReplication();
    if(hasMC) aodHandler->SetNeedsMCParticlesBranchReplication();
    jpsi->SetHasMC(hasMC);
  }
  
  //Create task and add it to the analysis manager
  AliAnalysisTaskDielectronFilter *task=new AliAnalysisTaskDielectronFilter("jpsi_DielectronFilter");
  
  if (!hasMC) task->UsePhysicsSelection();
  task->SetDielectron(jpsi);
  if(storeLS) task->SetStoreLikeSignCandidates(storeLS);
  task->SetStoreRotatedPairs(storeTR);  
  mgr->AddTask(task);

  //----------------------
  //create data containers
  //----------------------
  
  
  TString containerName = mgr->GetCommonFileName();
  containerName += ":PWGDQ_dielectronFilter";
  
  //create output container
  
  AliAnalysisDataContainer *cOutputHist1 =
    mgr->CreateContainer("jpsi_FilterQA",
                         THashList::Class(),
                         AliAnalysisManager::kOutputContainer,
                         containerName.Data());
  
  AliAnalysisDataContainer *cOutputHist2 =
    mgr->CreateContainer("jpsi_FilterEventStat",
                         TH1D::Class(),
                         AliAnalysisManager::kOutputContainer,
                         containerName.Data());
  
  
  mgr->ConnectInput(task,  0, mgr->GetCommonInputContainer());
  mgr->ConnectOutput(task, 1, cOutputHist1);
  mgr->ConnectOutput(task, 2, cOutputHist2);
  
  return task;
}
