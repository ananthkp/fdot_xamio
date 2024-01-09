// Contributed by Mahdi Nazm Bojnordi
//
// The ESESC/BSD License
//
// Copyright (c) 2005-2016, Regents of the University of Rochester and
// the ESESC Project.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
//   - Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
//   - Neither the name of the University of California, Santa Cruz nor the
//   names of its contributors may be used to endorse or promote products
//   derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.


#include "SescConf.h"
#include "MemorySystem.h"
#include "WideIO.h"
#include "../../../apps-ananth/esesc/simusignal.h"
//#include <iostream>
//#include "stdlib.h"
//#include <queue>
//#include <cmath>


// WideIO clock, scaled wrt globalClock
//Time_t globalClock;
std::queue<ServicedRequest> WideIOServicedQ;
std::queue<WideIOReference *> WideIOFetchedQ;
std::queue<WideIOReference *> WideIOSkippedQ;
std::queue<WideIOWriteBack> WideIOWriteBackQ;
std::queue<WideIOReference *> WideIOCompleteQ;
std::queue<WideIOReference *> WideIODelayedQ;
std::queue<WideIOReference *> KMeansCompleteQ;

bool KMEANSREQRECEIVED = false;
bool KMEANSLASTREQRECVD = false;
bool KMEANS_FIRSTCALL = false;
int firstaddreqcall;
AddrType KMeans_start_addr;
int PushedReqKMeans = 0;
AddrType currservaddr;

long long sendupcounter;

#define NO_OF_POINTS 100000
#define NUM_ITERATIONS 100
#define NO_OF_CENTROIDS 8
#define NO_OF_DIMENSIONS 3

// memory reference constructor
WideIOReference::WideIOReference()
  /* constructor {{{1 */
  : front(NULL)
  , back(NULL)
  , prev(NULL)
  , next(NULL)
  , state(UNKNOWN)
  , mreq(NULL)
  , rankID(0)
  , bankID(0)
  , rowID(0)
  , openRowID(0)
  , fchCount(0)
  , insCount(0)
  , evcCount(0)
  , wbkCount(0)
  , numGrain(1)
  , numGrainLog2(0)
  , prechargePending(false)
  , activatePending(false)
  , readPending(false)
  , writePending(false)
  , loadPending(false)
  , updatePending(false)
  , burstPending(false)
  , burstType(NONE)
  //, ready(false)
  , read(true)
  , sentUp(false)
  , tagIdeal(false)
  , numMatch(0)
  , numRead(0)
  , numWrite(0)
  , numLoad(0)
  , numUpdate(0)
  , numEvict(0)
  , numInstall(0)
  //, writeAfterHit(false)
  //, fillAfterMiss(false)
  , folded(false)
  , replica(false)
  , canceled(false)
{
  birthTime = globalClock;
  climbTime = globalClock;
  deathTime = globalClock;
  //blkTag.value = false;
}


// send a response up
void WideIOReference::sendUp() {
  sendupcounter++;
  //printf("sendupcounter %ld\n", sendupcounter); fflush(stdout);
  if((mreq != NULL) && (sentUp != true)) {
    climbTime = globalClock;
    deathTime = globalClock;
    ServicedRequest sreq;
    sreq.time = climbTime - birthTime;
    sreq.mreq = mreq;
    WideIOServicedQ.push(sreq);
    sentUp    = true;
    addLog("climb");
  }
  else if((mreq == NULL) && (sentUp != true)) {                                                                                //Ananth Changes to Account for KMEANS requests
    //climbTime = globalClock;
    //deathTime = globalClock;
    //ServicedRequest sreq;
    //sreq.time = globalClock;
    //sreq.mreq = NULL;
    //WideIOServicedQ.push(sreq);
    KMeansCompleteQ.push(this);
    sentUp    = true;
    addLog("climb");

  }
}


// add text to the reference log
void WideIOReference::addLog(const char *format, ...) {
    //return;
    char *temp = (char *)malloc(1024);
    va_list ap;
    va_start(ap, format);
    vsprintf(temp, format, ap);
    va_end(ap);
    logText.push_back(temp);
    logTime.push_back((Time_t)globalClock);
    //printf("\t@ %ld\t%s\t%s\n", globalClock, temp, folded? "[F]": ""); fflush(stdout);
    //if(globalClock > 4699036) exit(0);
    //if(logText.size() > 100) { printState(); printLog(); exit(0); }
}


// bank constructor
WideIOBank::WideIOBank(const char *name, AddrType numRows)
  : state(IDLE)
  , openRowID(0)
  , lastOpenRowID(0)
  , nextOpenRowID(0)
  , lastAction(0)
  , lastRefresh(0)
  , lastPrecharge(0)
  , lastActivate(0)
  , lastRead(0)
  , lastWrite(0)
  , rowHit(false)
  , pending(false)
  , fetching(false)
  , readCount(0)
  , numReads("%s:reads", name)
  , numWrites("%s:writes", name)
  , numActivates("%s:activates", name)
  , numPrecharges("%s:precharges", name)
  , numRefreshes("%s:refreshes", name)
  , numRowHits("%s:rowHits", name)
  , numRowMisses("%s:rowMisses", name)
{
  for(long i = 0; i < numRows; ++i) {
      TagRow tagRow;
      tags.push_back(tagRow);
  }
}

// refresh bank
void WideIOBank::refresh()
{
  state         = IDLE;
  nextOpenRowID = openRowID;
  lastOpenRowID = openRowID;
  openRowID     = 0;
  lastRefresh   = globalClock;
  lastAction    = globalClock;
  rowHit        = false;
  fetching      = false;
  readCount     = 0;
  numRefreshes.inc();
}

// Precharge bank
void WideIOBank::precharge()
{
  state         = IDLE;
  nextOpenRowID = openRowID + openRowID - lastOpenRowID;
  lastOpenRowID = openRowID;
  openRowID     = 0;
  lastPrecharge = globalClock;
  lastAction    = globalClock;
  rowHit        = false;
  fetching      = false;
  readCount     = 0;
  numPrecharges.inc();
}

// Activate row
void WideIOBank::activate(AddrType rowID)
{
  state        = ACTIVE;
  openRowID    = rowID;
  lastActivate = globalClock;
  lastAction   = globalClock;
  rowHit       = false;
  fetching     = false;
  readCount    = 0;
  numActivates.inc();
}

// Read from open row
TagRow WideIOBank::read(AddrType colID)
{
  if(rowHit) {
    numRowHits.inc();
  }
  else {
    numRowMisses.inc();
  }
  lastRead   = globalClock;
  lastAction = globalClock;
  rowHit     = true;
  readCount  = readCount + 1;
  numReads.inc();
  //printf("READ\tRow:%08lX\t%s\t%s\t%ld\t%08lX\n", openRowID, tags[openRowID].valid? "VALID": "", tags[openRowID][blkID].dirty? "DIRTY": "", tags[openRowID][blkID].count, tags[openRowID][blkID].value, openRowID);
  return tags[openRowID];
}

// Write to open row
void WideIOBank::write(AddrType colID, TagRow *tagRow)
{
  //printf("%s\t%s\t%ld\t%08lX\t%08lX\n", tags[openRowID][blkID].valid? "VALID": "", tags[openRowID][blkID].dirty? "DIRTY": "", tags[openRowID][blkID].count, tags[openRowID][blkID].value, openRowID);
  if(tagRow != NULL) {
    tags[openRowID].setTagRow(*tagRow, openRowID);
  }
  //tags[openRowID][blkID] = tag;
  //printf("WRITE\t\t%s\t%s\t%ld\t%08lX\t%08lX\n", tags[openRowID][blkID].valid? "VALID": "", tags[openRowID][blkID].dirty? "DIRTY": "", tags[openRowID][blkID].count, tags[openRowID][blkID].value, openRowID);
  if(rowHit) {
    numRowHits.inc();
  }
  else {
    numRowMisses.inc();
  }
  lastWrite  = globalClock;
  lastAction = globalClock;
  rowHit     = true;
  numWrites.inc();
}



// rank constructor
WideIORank::WideIORank(const char *section, const char *name, AddrType numBanks, AddrType numRows)
  : emr(SLOW)
  , powerState(sIDD2N)
  , rankEnergy(0)
  , cke(true)
  //, urgentRefresh(false)
  //, timeRefresh(0)
  , lastRefresh(0)
  , lastPrecharge(0)
  , lastActivate(0)
  , lastActive_1(0)
  , lastActive_2(0)
  , lastRead(0)
  , lastWrite(0)
    //, stampRefresh("WideIO_%d_rank_%d_stamp_refresh", _vaultID, _rankID)
{
  this->numBanks = numBanks;

  // Check timing parameters
  SescConf->isInt(section, "tREFI");     // Refresh Interval period
  SescConf->isInt(section, "tRFC");      // Refresh Cycle time
  SescConf->isInt(section, "tRTRS");     // Rank-To-Rank switching time
  SescConf->isInt(section, "tOST");      // Output Switching Time
  SescConf->isInt(section, "tRCD");      // Row to Column Delay
  SescConf->isInt(section, "tCAS");      // Column Access Strobe latency (aka tCL)
  SescConf->isInt(section, "tCWD");      // Column Write Delay (aka tWL)
  SescConf->isInt(section, "tCCD");      // Column to Column Delay
  SescConf->isInt(section, "tWTR");      // Write To Read delay time
  SescConf->isInt(section, "tWR");       // Write Recovery time
  SescConf->isInt(section, "tRTP");      // Read to Precharge
  SescConf->isInt(section, "tRP");       // Row Precharge
  SescConf->isInt(section, "tRRD");      // Row activation to Row activation Delay
  SescConf->isInt(section, "tRAS");      // Row Access Strobe
  SescConf->isInt(section, "tRC");       // Row Cycle
  SescConf->isInt(section, "tBL");       // Burst Length
  SescConf->isInt(section, "tCPDED");    // Command Pass Disable Delay
  SescConf->isInt(section, "tPD");       // Minimum power down duration
  SescConf->isInt(section, "tXP");       // Time to exit fast power down
  SescConf->isInt(section, "tXPDLL");    // Time to exit slow power down
  SescConf->isInt(section, "tFAW");      // Four bank Activation Window

  // Check power/energy parameters
  SescConf->isInt(section, "VDD");       // Operating voltage
  SescConf->isInt(section, "VDDMax");    // Maximum Operating voltage
  SescConf->isInt(section, "IDD0");      // Operating current: One bank active-precharge
  SescConf->isInt(section, "IDD2PF");    // Precharge power-down current, Fast PDN Exit; MRS[12] = 0
  SescConf->isInt(section, "IDD2PS");    // Precharge power-down current, Slow PDN Exit; MRS[12] = 1
  SescConf->isInt(section, "IDD2N");     // Precharge standby current
  SescConf->isInt(section, "IDD3P");     // Active power-down current
  SescConf->isInt(section, "IDD3N");     // Active standby current
  SescConf->isInt(section, "IDD4R");     // Operating burst read current
  SescConf->isInt(section, "IDD4W");     // Operating burst write current
  SescConf->isInt(section, "IDD5");      // Burst refresh current
  //SescConf->isInt(section, "cycleTime"); // DRAM clock cycle time
  //SescConf->isInt(section, "numChips");  // Number of chips per rank

  // Check bank count
  //SescConf->isInt(section, "numBanks");


  // Read timing parameters
  tREFI      = SescConf->getInt(section, "tREFI");
  tRFC       = SescConf->getInt(section, "tRFC");
  tRTRS      = SescConf->getInt(section, "tRTRS");
  tOST       = SescConf->getInt(section, "tOST");
  tRCD       = SescConf->getInt(section, "tRCD");
  tCAS       = SescConf->getInt(section, "tCAS");
  tCWD       = SescConf->getInt(section, "tCWD");
  tCCD       = SescConf->getInt(section, "tCCD");
  tWTR       = SescConf->getInt(section, "tWTR");
  tWR        = SescConf->getInt(section, "tWR");
  tRTP       = SescConf->getInt(section, "tRTP");
  tRP        = SescConf->getInt(section, "tRP");
  tRRD       = SescConf->getInt(section, "tRRD");
  tRAS       = SescConf->getInt(section, "tRAS");
  tRC        = SescConf->getInt(section, "tRC");
  tBL        = SescConf->getInt(section, "tBL");
  tCPDED     = SescConf->getInt(section, "tCPDED");
  tPD        = SescConf->getInt(section, "tPD");
  tXP        = SescConf->getInt(section, "tXP");
  tXPDLL     = SescConf->getInt(section, "tXPDLL");
  tFAW       = SescConf->getInt(section, "tFAW");

  // handling no refresh
  SescConf->isInt(section, "refAlgorithm");
  if(SescConf->getInt(section, "refAlgorithm") == 0) {
    tRFC = 0;
  }


  // Read power/energy parameters
  VDD        = SescConf->getInt(section, "VDD");
  VDDMax     = SescConf->getInt(section, "VDDMax");
  IDD0       = SescConf->getInt(section, "IDD0");
  IDD2PF     = SescConf->getInt(section, "IDD2PF");
  IDD2PS     = SescConf->getInt(section, "IDD2PS");
  IDD2N      = SescConf->getInt(section, "IDD2N");
  IDD3P      = SescConf->getInt(section, "IDD3P");
  IDD3N      = SescConf->getInt(section, "IDD3N");
  IDD4R      = SescConf->getInt(section, "IDD4R");
  IDD4W      = SescConf->getInt(section, "IDD4W");
  IDD5       = SescConf->getInt(section, "IDD5");
  //cycleTime  = SescConf->getInt(section, "cycleTime");
  //numChips   = SescConf->getInt(section, "numChips");
  // new formulas based on micron power calculator
  //long vt    = numChips * cycleTime * VDD * VDD / VDDMax;
  //eIDD2PF    = IDD2PF * cycleTime * 8 * VDD * VDD / VDDMax;
  //eIDD2PS    = IDD2PS * cycleTime * 8 * VDD * VDD / VDDMax;
  //eIDD3P     = IDD3P  * cycleTime * 8 * VDD * VDD / VDDMax;
  //eIDD2N     = IDD2N  * cycleTime * 8 * VDD * VDD / VDDMax;
  //eIDD3N     = IDD3N  * cycleTime * 8 * VDD * VDD / VDDMax;
  //eActivate  =  (IDD0 - ((IDD3N* tRAS)/tRC  + (IDD2N * (tRC - tRAS)/tRC))) * VDD * tRC *  cycleTime * 8 * VDD / VDDMax; //((IDD0 * tRC) - (IDD3N * tRAS + IDD2N * (tRP))) * vt;
  //// tBL* ((number of reads + write)* tBL / dramcycles) /2
  //ePrecharge = IDD2N * (tRP) * vt;
  //eRead      = (IDD4R - IDD3N) * VDD * (tBL/8) * (tBL/2) * cycleTime * 8 * VDD / VDDMax;
  ////eRead      = 0;
  //eWrite     = (IDD4W - IDD3N) * VDD * (tBL/8) * (tBL/2) * cycleTime * 8 * VDD / VDDMax;
  ////eWrite     = 0;
  //eRefresh   = ((IDD5  - IDD3N) * tRFC * VDD)/tREFI * cycleTime * 8 * VDD / VDDMax;

  // Read bank count
  //numBanks = SescConf->getInt(section, "numBanks");
  //numBanksLog2 = log2(numBanks);


  banks = *(new std::vector<WideIOBank *>(numBanks));
  for(int b = 0; b < numBanks; ++b) {
    char temp[256]; sprintf(temp, "%s:%d", name, b);
    banks[b] = new WideIOBank(temp, numRows);
  }
}


// enable refresh
void WideIORank::enableRefresh(double offset)
{
  Time_t delay = (double)tREFI*offset + 1;

  // schedule the first refresh
  CallbackBase *cb = refreshCB::create(this);
  EventScheduler::schedule((TimeDelta_t) delay, cb);
}

// check for an ongoing refresh
bool WideIORank::isRefreshing()
{
    //printf("(lastRefresh{%ld} > 0) && (globalClock{%ld} < lastRefresh{%ld} + tRFC{%ld})\n", lastRefresh, globalClock, lastRefresh, tRFC);
    return (lastRefresh > 0) && (globalClock < lastRefresh + tRFC);
}

// number of needed refreshes ?
//int WideIORank::neededRefreshes(AddrType bankID)
//{
//  return (globalClock - timeRefresh) / tREFI;
//}


// can a refresh issue now ?
//bool WideIORank::canIssueRefresh(AddrType bankID)
//{
//  // Enforce tRP
//  if(globalClock < lastPrecharge + tRP) {
//    return false;
//  }
//
//  // Enforce tRFC
//  if(globalClock < lastRefresh + tRFC) {
//    return false;
//  }
//
//  return true;
//}


// can a precharge issue now to the given bank ?
bool WideIORank::canIssuePrecharge(AddrType bankID)
{
  // Check for ongoing refresh
  if(isRefreshing()) {
      return false;
  }

  //Enforce tWR
  if(globalClock < (banks[bankID]->getLastWrite() + tCWD + tBL) + tWR) {
    return false;
  }

  //Enforce tRTP
  if(globalClock < banks[bankID]->getLastRead() + tRTP) {
    return false;
  }

  //Enforce tRAS
  if(globalClock < banks[bankID]->getLastActivate() + tRAS) {
    return false;
  }

  return true;
}


// can an activate issue now to the given bank ?
bool WideIORank::canIssueActivate(AddrType bankID)
{
  // Enforce tRFC
  //if(globalClock < lastRefresh + tRFC) {
  if(isRefreshing()) {
      return false;
  }

  // Enforce tFAW
  if(globalClock < lastActive_2 + tFAW) {
    return false;
  }

  // Enforce tRP
  if(globalClock < banks[bankID]->getLastPrecharge() + tRP) {
    return false;
  }

  // Enforce tRRD
  for(int i=0; i < numBanks; i++) {
    if(i != bankID) {
      if(globalClock < banks[i]->getLastActivate() + tRRD) {
        return false;
      }
    }
  }

  // Enforce tRC
  if(globalClock < banks[bankID]->getLastActivate() + tRC) {
    return false;
  }

  return true;
}

// can a read issue to the given bank and row ?
bool WideIORank::canIssueRead(AddrType bankID, AddrType rowID)
{
  // Check for ongoing refresh
  if(isRefreshing()) {
      return false;
  }

  //Enforce page misses
  if(banks[bankID]->getOpenRowID() != rowID) {
    return false;
  }

  //Enforce tRCD
  if(globalClock < banks[bankID]->getLastActivate() + tRCD) {
    return false;
  }

  //Enforce tCCD
  if((globalClock < lastRead + tCCD) || (globalClock < lastWrite + tCCD)) {
    return false;
  }

  //Enforce tWTR
  if(globalClock < (lastWrite + tCWD + tBL) + tWTR) {
    return false;
  }

  return true;
}


// Can a write issue to the given bank and row ?
bool WideIORank::canIssueWrite(AddrType bankID, AddrType rowID)
{
  // Check for ongoing refresh
  if(isRefreshing()) {
      return false;
  }

  //Enforce page misses
  if(banks[bankID]->getOpenRowID() != rowID) {
    return false;
  }

  //Enforce tRCD
  if(globalClock < banks[bankID]->getLastActivate() + tRCD) {
    return false;
  }

  //Enforce tCCD
  if((globalClock < lastRead + tCCD) || (globalClock < lastWrite + tCCD)) {
    return false;
  }

  return true;
}


// Refresh
void WideIORank::refresh(void)
{
//printf("refresh @ %lld\n", globalClock);
  //Timing
  lastRefresh  = globalClock;
  //timeRefresh += tREFI;
  //stampRefresh.sample(globalClock);

  //refresh all the banks
  for(int i=0; i < numBanks; ++i) {
    banks[i]->refresh();
  }

  //Energy
  rankEnergy += eRefresh;

  //power
  cke = true;

  // schedule a refresh
  CallbackBase *cb = refreshCB::create(this);
  EventScheduler::schedule((TimeDelta_t) tREFI, cb);
}


// Precharge given bank
void WideIORank::precharge(AddrType bankID)
{
  //Timing
  lastPrecharge = globalClock;
  banks[bankID]->precharge();

  //Energy
  rankEnergy += ePrecharge;

  //power
  cke = true;
  //PowerClock = 0;
}

//Activate given row in given bank
void WideIORank::activate(AddrType bankID, AddrType rowID)
{
  //Timing
  lastActive_2 = lastActive_1;
  lastActive_1 = lastActivate;
  lastActivate = globalClock;
  banks[bankID]->activate(rowID);

  //Energy
  rankEnergy += eActivate;

  //power
  cke = true;
  //PowerClock = 0;
}

//Read from given row in given bank
TagRow WideIORank::read(AddrType bankID, AddrType colID)
{
  //Energy
  rankEnergy += eRead;

  //power
  cke = true;
  //PowerClock = 0;

  //Timing
  lastRead = globalClock;
  return banks[bankID]->read(colID);
}

//Write to given row in given bank
void WideIORank::write(AddrType bankID, AddrType colID, TagRow *tagRow)
{
  //Energy
  rankEnergy += eWrite;

  //power
  cke = true;
  //PowerClock = 0;

  //Timing
  lastWrite = globalClock;
  banks[bankID]->write(colID, tagRow);
}


// Get WideIO rank's background energy
long WideIORank::getForegroundEnergy()
{
  long energy = rankEnergy;
  rankEnergy  = 0;
  return energy;
}

// Get WideIO rank's background energy
long WideIORank::getBackgroundEnergy()
{
  //Return energy for current power state
  switch (powerState) {
  case sIDD2PF:
    return eIDD2PF;
    break;
  case sIDD2PS:
    return eIDD2PS;
    break;
  case sIDD3P:
    return eIDD3P;
    break;
  case sIDD2N:
    return eIDD2N;
    break;
  case sIDD3N:
    return eIDD3N;
    break;
  default:
    printf("ERROR: Illegal rank power state '%d'!\n", powerState);
    exit(1);
    break;
  }
}

// Update WideIO power state
void WideIORank::updatePowerState()
{
  // Flag indicating all banks are precharged
  bool allBanksPrecharged = true;

  for(int i=0; i < numBanks; i++) {
    if(banks[i]->getState() == ACTIVE) {
      allBanksPrecharged = false;
    }
  }

  // If CKE is low
  if(cke == false) {
    // If all banks are precharged
    if(allBanksPrecharged) {
      //If using fast-exit mode
      if(emr == FAST) {
        powerState = sIDD2PF;
      }
      else{
        powerState = sIDD2PS;
      }
    }
    else{
      powerState = sIDD3P;
    }
  }
  else{
    //If all banks are precharged
    if(allBanksPrecharged) {
      powerState = sIDD2N;
    }
    else{
      powerState = sIDD3N;
    }
  }
}


// WideIO Vault Controller
WideIOVault::WideIOVault(const char *section, const char *name, AddrType numRanks, AddrType numBanks, AddrType numRows)
  : backEnergy("%s:background_energy", name)
  , foreEnergy("%s:foreground_energy", name)

  //, tracBlokLifeSpan("%s:tracBlokLifeSpan", name)
  //, sackLastWrites("%s:sackLastWrites", name)

  //, freqPageUsage("%s:freqPageUsage", name)
  //, freqPageAlpha("%s:freqPageAlpha", name)
  //, freqPageMType("%s:freqPageMType", name)

  //, histMReqType("%s:histMReqType", name)

  //, blockLifeSpan(1)
  //, blockLifeSpanSum(0.0)
  //, blockLifeSpanCnt(0.0)
  , counter(0)
{
  this->numRanks = numRanks;
  this->numBanks = numBanks;
  this->numRows  = numRows;
  //this->numSets  = numSets;
  //this->numBlks  = numBlks;

  // check user defined parameters
  SescConf->isGT(section, "schQueueSize", 0);
  SescConf->isGT(section, "repQueueSize", 0);
  //SescConf->isGT(section, "fetchRowSize", 0);
  //SescConf->isGT(section, "grnSize", 0);
  //SescConf->isPower2(section, "softPage", 0);

  //SescConf->isInt(section, "mtypeControl");
  //SescConf->isInt(section, "alphaCounter");
  //SescConf->isInt(section, "gammaCounter");
  //SescConf->isInt(section, "refAlgorithm");
  SescConf->isInt(section, "schAlgorithm");
  //SescConf->isInt(section, "rowAlgorithm");
  //SescConf->isInt(section, "cachingMode");
  SescConf->isPower2(section, "rowSize", 0);
  SescConf->isPower2(section, "blkSize", 0);
  SescConf->isPower2(section, "tagSize", 0);
  SescConf->isPower2(section, "numWays", 0);
  SescConf->isPower2(section, "grnSize", 0);
  //SescConf->isPower2(section, "numRanks", 0);
  //SescConf->isPower2(section, "numBanks", 0);

  SescConf->isInt(section, "tREFI");
  SescConf->isInt(section, "tRFC");
  SescConf->isInt(section, "tRTRS");
  SescConf->isInt(section, "tOST");
  SescConf->isInt(section, "tCAS");
  SescConf->isInt(section, "tCWD");
  SescConf->isInt(section, "tBL");

  // read in user-defined parameters
  schQueueSize = SescConf->getInt(section, "schQueueSize");
  repQueueSize = SescConf->getInt(section, "repQueueSize");
  //refAlgorithm = SescConf->getInt(section, "refAlgorithm");
  schAlgorithm = SescConf->getInt(section, "schAlgorithm");
  //cachingMode = SescConf->getInt(section, "cachingMode");
  //grnSize = SescConf->getInt(section, "grnSize");
  //rowAlgorithm = SescConf->getInt(section, "rowAlgorithm");
  //fetchRowSize = SescConf->getInt(section, "fetchRowSize");
  //softPage = SescConf->getInt(section, "softPage");
  //mtypeControl = SescConf->getInt(section, "mtypeControl");
  //alphaCounter = SescConf->getInt(section, "alphaCounter");
  //gammaCounter = SescConf->getInt(section, "gammaCounter");
  rowSize      = SescConf->getInt(section, "rowSize");
  blkSize      = SescConf->getInt(section, "blkSize");
  tagSize      = SescConf->getInt(section, "tagSize");
  numWays      = SescConf->getInt(section, "numWays");
  grnSize      = SescConf->getInt(section, "grnSize");
  numBlks      = rowSize / ((grnSize * blkSize) + tagSize);
  numSets      = numBlks / numWays + ((numBlks % numWays)? 1: 0);
  setSize      = numWays*grnSize*blkSize;
  if((numSets * setSize) > rowSize) {
      printf("ERROR: Invalid row configuration requiring at least %ldB per row!\n", numWays*grnSize*blkSize);
      printf("\trowSize: %ld\n\tblkSize: %ld\n\ttagSize: %ld\n\tnumWays: %ld\n\tgrnSize: %ld\n\tnumBlks: %ld\n\tnumSets: %ld\n\tsetSize: %ld\n", rowSize, blkSize, tagSize, numWays, grnSize, numBlks, numSets, setSize);
      exit(0);
  }
  tREFI        = SescConf->getInt(section, "tREFI");
  tRFC         = SescConf->getInt(section, "tRFC");
  tRTRS        = SescConf->getInt(section, "tRTRS");
  tOST         = SescConf->getInt(section, "tOST");
  tCAS         = SescConf->getInt(section, "tCAS");
  tCWD         = SescConf->getInt(section, "tCWD");
  tBL          = SescConf->getInt(section, "tBL");

  //softPageLog2 = log2(softPage);
  blkSizeLog2 = log2(blkSize);
  grnSizeLog2 = log2(grnSize);

  // statistics
  //tracBlokLifeSpan.setPFName(SescConf->getCharPtr("", "reportFile", 0));
  //tracBlokLifeSpan.setPlotLabels("Execution Time (Cycles)", "Observed Block Life Span");
  //sackLastWrites.setPlotLabels("Number of Last Writes per Block", "Frequency");
  //freqPageUsage.setPFName(SescConf->getCharPtr("", "reportFile", 0));
  //freqPageUsage.setPlotLabels("Page Usage", "Frequency");
  //freqPageAlpha.setPlotLabels("Page Alpha", "Frequency");
  //freqPageMType.setPlotLabels("Page MType", "Frequency");
  //histMReqType.setPFName(SescConf->getCharPtr("", "reportFile", 0));
  //histMReqType.setPlotLabels("0: ma_setInvalid, 1: ma_setValid, 2: ma_setDirty 3: ma_setShared, 4: ma_setExclusive, 5: ma_MMU, 6: ma_VPCWU, 7: ma_MAX", "Frequency");

  ranks = *(new std::vector<WideIORank *>(numRanks));
  for(int r = 0; r < numRanks; ++r) {
    char temp[256]; sprintf(temp, "%s:%d", name, r);
    ranks[r] = new WideIORank(section, temp, numBanks, numRows);
  }

  bankRowActivated = false;
  retunReqReceived = false;
  tagBufferUpdated = false;
  inputRefReceived = false;
  tagBuffer.resize(numRanks, std::vector<TagRow>(numBanks));
  for(int r = 0; r < numRanks; ++r) {
    for(int b = 0; b < numBanks; ++b) {
      for(int i = 0; i < numBlks/numWays; ++i) {
        tagBuffer[r][b].addSet(numWays);
      }
      if(numBlks%numWays) {
        tagBuffer[r][b].addSet(numBlks%numWays);
      }
    }
  }
}


// adding memory reference to WideIO vault
bool WideIOVault::isFull()
{
    return (schQueue.size() >= schQueueSize);
}

// adding memory reference to WideIO vault
void WideIOVault::addReference(WideIOReference *mref)
{
    //AddrType colID, tagID, blkID;
    //STATE bankState = getBankState(mref->getRankID(), mref->getBankID());
    //AddrType openRowID = getOpenRowID(mref->getRankID(), mref->getBankID());
    //AddrType lastOpenRowID = getLastOpenRowID(mref->getRankID(), mref->getBankID());
    //bool pending = false;
    //for(int i = 0; !pending && (i < schQueue.size()); ++i) {
    //    if((mref->getRankID() == schQueue[i]->getRankID()) && (mref->getBankID() == schQueue[i]->getBankID())) {
    //        pending = true;
    //    }
    //}
    //mref->setBankPend(pending);
    //mref->setCurRBHit(mref->getRowID() == openRowID);
    //mref->setPreRBHit(mref->getRowID() == lastOpenRowID);
    //mref->setDieRBHit(false);
    mref->setBlkSize(blkSize);
    mref->setNumGrain(grnSize);
    //mref->setTagIdeal(cachingMode == 0);

    // update memory reference state in the queue
    mref->setSetID((mref->getColID() / setSize) % numSets);
    mref->setTagID(mref->getMAddr() >> (blkSizeLog2 + grnSizeLog2));
    //mref->setTimeStamp(globalClock);
    //mref->setTagCheckPending(mref->getState() == READTAG);
    //mref->setPrechargePending(bankState == ACTIVE && mref->getRowID() != openRowID);
    //mref->setActivatePending(bankState == IDLE || mref->getRowID() != openRowID);

    mref->setReadPending((mref->getState() == SCRATCH) && mref->isRead());
    mref->setWritePending((mref->getState() == SCRATCH) && !mref->isRead());
    mref->setBurstPending(false, NONE);
    //mref->setBlockEviction(false);
    //mref->setFillAfterMiss(false);
    inputRefReceived = true;

    // add the memory reference to the scheduling queue
    schQueue.push_back(mref);
}


// adding a returning reference to the WideIO Vault
void WideIOVault::retReference(WideIOReference *mref)
{
    retQueue.push(mref);
}


// Update the state of tag buffer
//void WideIOVault::updateRowTBState()
//{
//    for(int r=0; r < numRanks; ++r) {
//        for(int b=0; b < numBanks; ++b) {
//            tagBuffer[r][b].setReadOnly((getBankState(r, b) == IDLE) || (tagBuffer[r][b].getRowID() != getOpenRowID(r, b)));
//        }
//    }
//}

// enable refresh at rank level
void WideIOVault::enableRefresh(AddrType rankID, double offset)
{
    ranks[rankID]->enableRefresh(offset);
}

// check for an ongoing refresh
bool WideIOVault::isRefreshing(AddrType rankID)
{
    return ranks[rankID]->isRefreshing();
}

// number of needed refresh operations to a given bank ?
//int WideIOVault::neededRefreshes(AddrType rankID, AddrType bankID)
//{
//    return ranks[rankID]->neededRefreshes(bankID);
//}

// Can a refresh to given rank issue now ?
//bool WideIOVault::canIssueRefresh(AddrType rankID, AddrType bankID)
//{
//    return ranks[rankID]->canIssueRefresh(bankID);
//}

//Can a precharge to given rank and bank issue now ?
bool WideIOVault::canIssuePrecharge(AddrType rankID, AddrType bankID)
{
    //Check intra-rank constraints
    return ranks[rankID]->canIssuePrecharge(bankID);
}

//Can an activate to given rank and bank issue now ?
bool WideIOVault::canIssueActivate(AddrType rankID, AddrType bankID)
{
    //Check intra-rank constraints
    return ranks[rankID]->canIssueActivate(bankID);
}


// Can a read to given rank, bank, and row issue now
bool WideIOVault::canIssueRead(AddrType rankID, AddrType bankID, AddrType rowID)
{
    // Check intra-rank constraints
    if(!ranks[rankID]->canIssueRead(bankID, rowID)) {
        return false;
    }

    // Enforce tRTRS
    for(int i=0; i < numRanks; i++) {
        if(i != rankID) {
            // Consecutive read commands
            if(globalClock < (ranks[i]->getLastRead() + tBL) + tRTRS) {
                return false;
            }

            // Write command followed by read command
            //if(globalClock < (ranks[i]->getLastWrite() + tCWD + tBL - tCAS) + tRTRS) {
            if((globalClock + tCAS) < (ranks[i]->getLastWrite() + tCWD + tBL) + tRTRS) {
                return false;
            }
        }
    }

    return true;
}

// Can a write to given rank, bank, and row issue now
bool WideIOVault::canIssueWrite(AddrType rankID, AddrType bankID, AddrType rowID) {
    // Check intra-rank constraints
    if(!ranks[rankID]->canIssueWrite(bankID, rowID)) {
        return false;
    }

    // Enforce tRTRS
    for(int i=0; i < numRanks; i++) {
        // Read followed by write, any two ranks
        //if(globalClock < (ranks[i]->getLastRead() + tCAS + tBL - tCWD) + tRTRS) {
        if((globalClock + tCWD) < (ranks[i]->getLastRead() + tCAS + tBL) + tRTRS) {
            return false;
        }

        // Write followed by write, different ranks
        if(i != rankID) {
            // Enforce tOST
            if(globalClock < (ranks[i]->getLastWrite() + tBL) + tOST) {
                return false;
            }
        }
    }

    return true;
}

// Refresh a rank
//void WideIOVault::refresh(AddrType rankID, AddrType bankID)
//{
//    ranks[rankID]->refresh(bankID);
//    lastRefresh  = globalClock;
//    timeRefresh += tREFI;
//}

// Precharge a bank
void WideIOVault::precharge(AddrType rankID, AddrType bankID)
{
    ranks[rankID]->precharge(bankID);
    lastPrecharge = globalClock;
}

// Activate a row
void WideIOVault::activate(AddrType rankID, AddrType bankID, AddrType rowID)
{
    ranks[rankID]->activate(bankID, rowID);
    lastActivate = globalClock;
    bankRowActivated = true;
}

// Read a block
TagRow WideIOVault::read(AddrType rankID, AddrType bankID, AddrType colID)
{
    lastRead = globalClock;
    return ranks[rankID]->read(bankID, colID);
}

// Write a block
void WideIOVault::write(AddrType rankID, AddrType bankID, AddrType colID, TagRow *tagRow)
{
    lastWrite = globalClock;
    ranks[rankID]->write(bankID, colID, tagRow);
}

// Send a clock signal to the WideIO vault controller
void WideIOVault::clock()
{
    // Process the entries of retQueue
    receiveReturned();

    // replace tag in the tag buffer
    performReplace();

    // Complete a pending tag check if needed
    performTagCheck();

    // Complete a pending data burst if needed
    performBurst();

    // Update queue and power states
    updateQueueState();
    updatePowerState();

    // Include installs in the schedule
    scheduleInstall();

    // Schedule a WideIO command if needed
    switch(schAlgorithm) {
      case 0: // FCFS
          scheduleFCFS();
          break;
      case 1: // FRFCFS
          scheduleFRFCFS();
          break;
      case 2: // TEST
          scheduleTEST();
          break;
      default:
          printf("ERROR: Illegal ID for WideIO command scheduler!\n");
          exit(0);
    }

    // Schedule a command for row (page) management
    //switch(rowAlgorithm) {
    //    case 0: // Open Row
    //        break;
    //    case 1: // Close Row
    //        scheduleRBClose();
    //        break;
    //    //case 2: // Cache Row
    //    //    scheduleRBCache();
    //    //    break;
    //    //case 3: // Fetch Row
    //    //    scheduleRBFetch();
    //    //    break;
    //    //case 4: // ReAct Row
    //    //    scheduleRBReAct();
    //    //    break;
    //    //case 5: // ReTag Row
    //    //    scheduleRBReTag();
    //    //    break;
    //    default:
    //        printf("ERROR: Illegal ID for WideIO row management!\n");
    //        exit(0);
    //}

    // update stats
    //if((globalClock & 0x3FFFF) == 0) {
    //    printf("======== cycle %lld ========\n", globalClock);
    //    for(long i = 0; i < schQueue.size(); ++i) {
    //        schQueue[i]->printState();
    //    }
    //}
    //if((globalClock & 0x3FFFF) == 0) {
    //	tracBlokLifeSpan.sample(true, globalClock, blockLifeSpanSum/blockLifeSpanCnt);
    //  blockLifeSpanSum = 0.0;
    //  blockLifeSpanCnt = 0.0;
    //}
    //else {
    //  blockLifeSpanSum += blockLifeSpan;
    //  blockLifeSpanCnt += 1.0;
    //}
}


// Process the entries of retQueue
void WideIOVault::receiveReturned()
{
    bool blocked = false;
    while((retQueue.size() > 0) && (blocked == false)) {
        WideIOReference *mref = retQueue.front();
        if(mref->isFolded()) {
            if(repQueue.size() < repQueueSize) {
                mref->addLog("returned");
                mref->setState(DOPLACE);
                repQueue.push_back(mref);
                retQueue.pop();
            }
            else {
                blocked = true;
            }
        }
        else {
            if(schQueue.size() < schQueueSize) {
                mref->addLog("returned");
                mref->setState(DOPLACE);
                retunReqReceived = true;
                schQueue.push_back(mref);
                retQueue.pop();
            }
            else {
                blocked = true;
            }
        }
    }
}


// Replace tag in the tag buffer
void WideIOVault::performReplace()
{
  if(retunReqReceived || tagBufferUpdated || bankRowActivated) {
    for(int i=0; i < schQueue.size(); ++i) {
      WideIOReference *mref = schQueue[i];
      if(mref->getState() == DOPLACE) {
        if(tagBuffer[mref->getRankID()][mref->getBankID()].matchRowID(mref->getRowID())) {
          if((getBankState(mref->getRankID(), mref->getBankID()) == ACTIVE) && (getOpenRowID(mref->getRankID(), mref->getBankID()) == mref->getRowID())) {
            mref->setState(FINDTAG);
          }
        }
      }
      //else if(mref->getState() == FINDTAG) {
      //  //if(!tagBuffer[mref->getRankID()][mref->getBankID()].matchRowID(mref->getRowID()) ||
      //  if(!((getBankState(mref->getRankID(), mref->getBankID()) == ACTIVE) && (getOpenRowID(mref->getRankID(), mref->getBankID()) == mref->getRowID()))) {
      //    mref->addLog("++ %ld =? %ld =? %ld", mref->getRowID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID(), getOpenRowID(mref->getRankID(), mref->getBankID()));
      //    mref->setState(DOPLACE);
      //  }
      //}
    }
  }
}


// Performing Tag Check
void WideIOVault::performTagCheck()
{
  // update tag flags
  if(inputRefReceived || retunReqReceived || tagBufferUpdated || bankRowActivated) {
    for(int i=0; i < schQueue.size(); ++i) {
      WideIOReference *mref = schQueue[i];
      AddrType rankID = mref->getRankID();
      AddrType bankID = mref->getBankID();
      AddrType rowID  = mref->getRowID();
      if(tagBuffer[rankID][bankID].matchRowID(rowID)) {
        if((mref->getState() == READTAG) || (mref->getState() == DOCHECK) || (mref->getState() == MATCHED)) {
          long index = mref->doTagCheck(tagBuffer[rankID][bankID].getSet(mref->getSetID()));
          if(index > -1) {
            if(mref->getState() != MATCHED) {
              mref->addLog("match %ld: %lx", index, tagBuffer[rankID][bankID].getIndexTag(mref->getSetID(), index).value);
              mref->setState(MATCHED);
              mref->updateSiblings(true);
              mref->setReadPending(mref->isRead());
              mref->setWritePending(!mref->isRead());
              tagBuffer[rankID][bankID].accessTagIndex(mref->getSetID(), index, !mref->isRead());
            }
          }
          else {
            mref->addLog("mismatch");
            mref->setState(DOFETCH);
            mref->updateSiblings(false);
            WideIOFetchedQ.push(mref);
            schQueue.erase(schQueue.begin() + i);
            --i;
          }
        }
        else if(mref->getState() == FINDTAG) {
          long index = tagBuffer[rankID][bankID].getVictimIndex(mref->getSetID());
          I(index>-1);
          TagType victim = tagBuffer[rankID][bankID].getIndexTag(mref->getSetID(), index);
          if(victim.valid && victim.dirty) {
            victim.valid = true;
            victim.dirty = false;
            victim.value = mref->getTagID();
            tagBuffer[rankID][bankID].setIndexTag(mref->getSetID(), index, victim);
            tagBuffer[rankID][bankID].accessTagIndex(mref->getSetID(), index, false);
            mref->addLog("dirty");
            mref->setState(DOEVICT);
          }
          else {
            tagBuffer[rankID][bankID].installTag(mref->getSetID(), mref->getTagID());
            mref->addLog("clean");
            mref->setState(INSTALL);
          }
        }
      }
      else {
        if(mref->getState() == FINDTAG) {
          printf("ERROR: Invalid memory refrence state!\n");
          mref->printState();
          mref->printLog();
          exit(0);
        }
      }
    }
    inputRefReceived = false;
    retunReqReceived = false;
    tagBufferUpdated = false;
    bankRowActivated = false;
  }
}


// Performing Data Burst
void WideIOVault::performBurst()
{
  int index;
  WideIOReference *mref = NULL;

  // Find the oldest ready to complete burst
  for(int i=0; (i < schQueue.size()) && (mref == NULL); ++i) {
    if(schQueue[i]->needsBurst()) {
      if(globalClock > (schQueue[i]->isOutBurst()? (lastRead + tCAS): (lastWrite + tCWD)) + tBL) {
        mref = schQueue[i];
        index = i;
      }
    }
  }

  // Complete data burst
  if(mref != NULL) {
    mref->addLog("burst");
    if(mref->getBurstType() == UPDATE) {
      tagBuffer[mref->getRankID()][mref->getBankID()].clear();
      tagBufferUpdated = true;
    }
    else if(mref->getBurstType() == LOAD) {
      tagBuffer[mref->getRankID()][mref->getBankID()].setTagRow(mref->getTagRow(), mref->getOpenRowID());
      tagBufferUpdated = true;
      mref->setState((mref->getState() == READTAG)? DOCHECK: FINDTAG);
      mref->addLog("tagBuffer[%ld][%ld]: rowID <- %ld", mref->getRankID(), mref->getBankID(), mref->getOpenRowID());
    }
    else {
      //printf("SHOULD NOT ENTER HERE !!!!"); fflush(stdout);
      if(mref->getState() == WRITEBK) {
        AddrType waddr = mref->getMAddr() >> (blkSizeLog2 + grnSizeLog2);
        waddr <<= (blkSizeLog2 + grnSizeLog2);
        do {
            WideIOWriteBack wb;
            wb.maddr = waddr;
            wb.mdata = NULL; //mref->getMReq()->getData();
            WideIOWriteBackQ.push(wb);
            mref->addLog("writeback 0x%lx", waddr);
            waddr += blkSize;
        }
        while(!mref->incWbkCount());
        mref->setState(INSTALL);
      }
      if(!mref->isSentUp()) {
        mref->sendUp();
      }
      WideIOCompleteQ.push(mref);
      schQueue.erase(schQueue.begin() + index);
    }
    mref->setBurstPending(false, NONE);
  }
}


// Update the state of the scheduling queue after issuing a DRAM command
void WideIOVault::updateQueueState()
{
    for(int i=0; i < schQueue.size(); ++i) {
        WideIOReference *mref = schQueue[i];
        if(mref->isCanceled()) {
            WideIOCompleteQ.push(mref);
            schQueue.erase(schQueue.begin() + i);
            --i;
        }
        else {
            AddrType rankID    = mref->getRankID();
            AddrType bankID    = mref->getBankID();
            AddrType rowID     = mref->getRowID();
            AddrType openRowID = getOpenRowID(rankID, bankID);
            STATE bankState    = getBankState(rankID, bankID);

            mref->setUpdatePending(false);
            mref->setLoadPending(false);
            mref->setPrechargePending(false);
            mref->setActivatePending(false);
            if(!tagBuffer[rankID][bankID].matchRowID(rowID) && !mref->needsBurst()) {
                if(tagBuffer[rankID][bankID].needsUpdate()) {
                    mref->setUpdatePending(true);
                    rowID = tagBuffer[rankID][bankID].getRowID();
                }
                else if(mref->needsTagLoad()) {
                    mref->setLoadPending(true);
                }
            }

            if(!mref->needsBurst()) {
                mref->setPrechargePending((bankState == ACTIVE) && (openRowID != rowID));
                mref->setActivatePending(isRefreshing(rankID) || (bankState == IDLE) || (openRowID != rowID));
            }
        }
    }
}

// update power state of the vault
void WideIOVault::updatePowerState()
{
    long fore = 0;
    long back = 0;

    for(int r=0; r < numRanks; ++r) {
        fore += ranks[r]->getForegroundEnergy();
        back += ranks[r]->getBackgroundEnergy();
        ranks[r]->updatePowerState();
    }

    backEnergy.add((double)back/1000000.0);
    foreEnergy.add((double)fore/1000000.0);
    //peakPower.sample((double)(fore+back)/cycleTime, true);
    //avrgPower.sample((double)(fore+back)/cycleTime, true);
}


// Include installs in the schedule
void WideIOVault::scheduleInstall()
{
    if((schQueue.empty() == true) || (repQueue.size() == repQueueSize)) {
        while((repQueue.empty() == false) && (schQueue.size() < schQueueSize)) {
            schQueue.push_back(repQueue[0]);
            repQueue.erase(repQueue.begin());
        }
    }
}

// FCFS command scheduling algorithm
void WideIOVault::scheduleFCFS()
{
    if(schQueue.empty()) {
        return;
    }

    WideIOReference *mref = schQueue[0];

    if(mref->needsPrecharge()) {
        if(canIssuePrecharge(mref->getRankID(), mref->getBankID())) {
            precharge(mref->getRankID(), mref->getBankID());
            mref->addLog("precharge %ld/%ld", mref->getRankID(), mref->getBankID());
        }
        return;
    }

    if(mref->needsActivate()) {
        if(mref->needsUpdate()) {
            if(canIssueActivate(mref->getRankID(), mref->getBankID())) {
                activate(mref->getRankID(), mref->getBankID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID());
                mref->addLog("reopen %ld/%ld/%ld", mref->getRankID(), mref->getBankID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID());
            }
        }
        else {
            if(canIssueActivate(mref->getRankID(), mref->getBankID())) {
                activate(mref->getRankID(), mref->getBankID(), mref->getRowID());
                mref->addLog("activate %ld/%ld/%ld", mref->getRankID(), mref->getBankID(), mref->getRowID());
            }
        }
        return;
    }

    if(mref->needsUpdate()) {
        if(canIssueWrite(mref->getRankID(), mref->getBankID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID())) {
            write(mref->getRankID(), mref->getBankID(), 0, &tagBuffer[mref->getRankID()][mref->getBankID()]);
            mref->setBurstPending(true, UPDATE);
            mref->addLog(":: %ld =? %ld =? %ld", mref->getRowID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID(), getOpenRowID(mref->getRankID(), mref->getBankID()));
            mref->addLog("update %ld/%ld/%ld/tag", mref->getRankID(), mref->getBankID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID());
            mref->incNumUpdate();
        }
        return;
    }

    if(mref->needsLoad()) {
        if(canIssueRead(mref->getRankID(), mref->getBankID(), mref->getRowID())) {
            TagRow tagRow = read(mref->getRankID(), mref->getBankID(), mref->getColID());
            mref->setTagRow(tagRow);
            mref->setBurstPending(true, LOAD);
            mref->setOpenRowID(getOpenRowID(mref->getRankID(), mref->getBankID()));
            mref->addLog("** %ld =? %ld =? %ld", mref->getRowID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID(), getOpenRowID(mref->getRankID(), mref->getBankID()));
            mref->addLog("load %ld/%ld/%ld/tag", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumLoad();
        }
        return;
    }


    if(mref->needsEviction()) {
        if(canIssueRead(mref->getRankID(), mref->getBankID(), mref->getRowID())) {
            read(mref->getRankID(), mref->getBankID(), 0);
            if(mref->incEvcCount()) {
                mref->setState(WRITEBK);
                mref->setBurstPending(true, READ);
            }
            mref->addLog("evict %ld/%ld/%ld/data", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumEvict();
        }
        return;
    }

    if(mref->needsRead()) {
        if(canIssueRead(mref->getRankID(), mref->getBankID(), mref->getRowID())) {
            read(mref->getRankID(), mref->getBankID(), mref->getColID());
            mref->setReadPending(false);
            mref->setBurstPending(true, READ);
            mref->addLog("read %ld/%ld/%ld/data", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumRead();
        }
        return;
    }

    if(mref->needsInstall()) {
        if(canIssueWrite(mref->getRankID(), mref->getBankID(), mref->getRowID())) {
            write(mref->getRankID(), mref->getBankID(), mref->getColID(), NULL);
            if(mref->incInsCount()) {
                mref->setState(JOBDONE);
                mref->setBurstPending(true, WRITE);
            }
            mref->addLog("install %ld/%ld/%ld/data", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumInstall();
        }
        return;
    }

    if(mref->needsWrite()) {
        if(canIssueWrite(mref->getRankID(), mref->getBankID(), mref->getRowID())) {
            write(mref->getRankID(), mref->getBankID(), mref->getColID(), NULL);
            mref->setWritePending(false);
            mref->setBurstPending(true, WRITE);
            mref->addLog("write %ld/%ld/%ld/data", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumWrite();
        }
        return;
    }
}


// FR-FCFS command scheduling algorithm
void WideIOVault::scheduleFRFCFS()
{
    WideIOReference *mref = NULL;
    DRAMCommand command = COMNON;

    // Find the oldest ready READ
    for(int i=0; (i < schQueue.size()) && (mref == NULL); i++) {
        if(schQueue[i]->needsSchedule() && !schQueue[i]->needsPrecharge() && !schQueue[i]->needsActivate() && !schQueue[i]->needsUpdate()) {
            if(schQueue[i]->needsLoad()) {
                if(canIssueRead(schQueue[i]->getRankID(), schQueue[i]->getBankID(), schQueue[i]->getRowID())) {
                    mref = schQueue[i];
                    command = COMLOD;
                }
            }
            else if(schQueue[i]->needsRead()) {
                if(canIssueRead(schQueue[i]->getRankID(), schQueue[i]->getBankID(), schQueue[i]->getRowID())) {
                    mref = schQueue[i];
                    command = COMRED;
                }
            }
            else if(schQueue[i]->needsEviction()) {
                if(canIssueRead(schQueue[i]->getRankID(), schQueue[i]->getBankID(), schQueue[i]->getRowID())) {
                    mref = schQueue[i];
                    command = COMEVT;
                }
            }
        }
    }

    // Find the oldest ready WRITE
    for(int i=0; (i < schQueue.size()) && (mref == NULL); i++) {
        if(schQueue[i]->needsSchedule() && !schQueue[i]->needsPrecharge() && !schQueue[i]->needsActivate()) {
            if(schQueue[i]->needsUpdate()) {
                if(canIssueWrite(schQueue[i]->getRankID(), schQueue[i]->getBankID(), tagBuffer[schQueue[i]->getRankID()][schQueue[i]->getBankID()].getRowID())) {
                    mref = schQueue[i];
                    command = COMUDT;
                }
            }
            else if(schQueue[i]->needsWrite()) {
                if(canIssueWrite(schQueue[i]->getRankID(), schQueue[i]->getBankID(), schQueue[i]->getRowID())) {
                    mref = schQueue[i];
                    command = COMWRT;
                }
            }
            else if(schQueue[i]->needsInstall()) {
                if(canIssueWrite(schQueue[i]->getRankID(), schQueue[i]->getBankID(), schQueue[i]->getRowID())) {
                    mref = schQueue[i];
                    command = COMINS;
                }
            }
        }
    }

    // Find the oldest ready ACTIVATE
    for(int i=0; (i < schQueue.size()) && (mref == NULL); ++i) {
        if(schQueue[i]->needsSchedule() && schQueue[i]->needsActivate()) {
            if(canIssueActivate(schQueue[i]->getRankID(), schQueue[i]->getBankID())) {
                mref = schQueue[i];
                command = schQueue[i]->needsUpdate()? COMOPN: COMACT;
            }
        }
    }

    // Find the oldest ready (update) PRECHARGE
    for(int i=0; (i < schQueue.size()) && (mref == NULL); ++i) {
        if(schQueue[i]->needsSchedule() && schQueue[i]->needsPrecharge()) {
            if(canIssuePrecharge(schQueue[i]->getRankID(), schQueue[i]->getBankID())) {
                mref = schQueue[i];
                command = COMPRE;
            }
        }
    }

    // Issue a ready command
    if(mref != NULL) {
        if(command == COMPRE) {
            precharge(mref->getRankID(), mref->getBankID());
            mref->addLog("precharge %ld/%ld", mref->getRankID(), mref->getBankID());
        }
        else if(command == COMACT) {
            activate(mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->addLog("activate %ld/%ld/%ld", mref->getRankID(), mref->getBankID(), mref->getRowID());
        }
        else if(command == COMOPN) {
            activate(mref->getRankID(), mref->getBankID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID());
            mref->addLog("reopen %ld/%ld/%ld", mref->getRankID(), mref->getBankID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID());
        }
        else if(command == COMRED) {
            read(mref->getRankID(), mref->getBankID(), mref->getColID());
            mref->setReadPending(false);
            mref->setBurstPending(true, READ);
            mref->addLog("read %ld/%ld/%ld/data", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumRead();
        }
        else if(command == COMWRT) {
            write(mref->getRankID(), mref->getBankID(), mref->getColID(), NULL);
            mref->setWritePending(false);
            mref->setBurstPending(true, WRITE);
            mref->addLog("write %ld/%ld/%ld/data", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumWrite();
        }
        else if(command == COMLOD) {
            TagRow tagRow = read(mref->getRankID(), mref->getBankID(), mref->getColID());
            mref->setTagRow(tagRow);
            mref->setBurstPending(true, LOAD);
            mref->setOpenRowID(getOpenRowID(mref->getRankID(), mref->getBankID()));
            mref->addLog("** %ld =? %ld =? %ld", mref->getRowID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID(), getOpenRowID(mref->getRankID(), mref->getBankID()));
            mref->addLog("load %ld/%ld/%ld/tag", mref->getRankID(), mref->getBankID(), getOpenRowID(mref->getRankID(), mref->getBankID()));
            mref->incNumLoad();
        }
        else if(command == COMUDT) {
            write(mref->getRankID(), mref->getBankID(), 0, &tagBuffer[mref->getRankID()][mref->getBankID()]);
            mref->setBurstPending(true, UPDATE);
            mref->addLog(":: %ld =? %ld =? %ld", mref->getRowID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID(), getOpenRowID(mref->getRankID(), mref->getBankID()));
            mref->addLog("update %ld/%ld/%ld/tag", mref->getRankID(), mref->getBankID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID());
            mref->incNumUpdate();
        }
        else if(command == COMEVT) {
            read(mref->getRankID(), mref->getBankID(), 0);
            if(mref->incEvcCount()) {
                mref->setState(WRITEBK);
                mref->setBurstPending(true, READ);
            }
            mref->addLog("evict %ld/%ld/%ld/data", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumEvict();
        }
        else if(command == COMINS) {
            write(mref->getRankID(), mref->getBankID(), mref->getColID(), NULL);
            if(mref->incInsCount()) {
                mref->setState(JOBDONE);
                mref->setBurstPending(true, WRITE);
            }
            mref->addLog("install %ld/%ld/%ld/data", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumInstall();
        }
        else {
            printf("ERROR: unknown command selected in FR-FCFS!\n");
            exit(0);
        }
    }
}


// TEST command scheduling algorithm
void WideIOVault::scheduleTEST()
{
    WideIOReference *mref = NULL;
    DRAMCommand command = COMNON;

    // Find the oldest ready (load) READ
    for(int i=0; (i < schQueue.size()) && (mref == NULL); i++) {
        if(schQueue[i]->needsSchedule() && !schQueue[i]->needsPrecharge() && !schQueue[i]->needsActivate() && !schQueue[i]->needsUpdate()) {
            if(schQueue[i]->needsLoad()) {
                if(canIssueRead(schQueue[i]->getRankID(), schQueue[i]->getBankID(), schQueue[i]->getRowID())) {
                    mref = schQueue[i];
                    command = COMLOD;
                }
            }
        }
    }

    // Find the oldest ready READ
    for(int i=0; (i < schQueue.size()) && (mref == NULL); i++) {
        if(schQueue[i]->needsSchedule() && !schQueue[i]->needsPrecharge() && !schQueue[i]->needsActivate() && !schQueue[i]->needsUpdate()) {
            if(schQueue[i]->needsRead()) {
                if(canIssueRead(schQueue[i]->getRankID(), schQueue[i]->getBankID(), schQueue[i]->getRowID())) {
                    mref = schQueue[i];
                    command = COMRED;
                }
            }
        }
    }

    // Find the oldest ready (evict) READ
    for(int i=0; (i < schQueue.size()) && (mref == NULL); i++) {
        if(schQueue[i]->needsSchedule() && !schQueue[i]->needsPrecharge() && !schQueue[i]->needsActivate() && !schQueue[i]->needsUpdate()) {
            if(schQueue[i]->needsEviction()) {
                if(canIssueRead(schQueue[i]->getRankID(), schQueue[i]->getBankID(), schQueue[i]->getRowID())) {
                    mref = schQueue[i];
                    command = COMEVT;
                }
            }
        }
    }

    // Find the oldest ready WRITE
    for(int i=0; (i < schQueue.size()) && (mref == NULL); i++) {
        if(schQueue[i]->needsSchedule() && !schQueue[i]->needsPrecharge() && !schQueue[i]->needsActivate() && !schQueue[i]->needsUpdate()) {
            if(schQueue[i]->needsWrite()) {
                if(canIssueWrite(schQueue[i]->getRankID(), schQueue[i]->getBankID(), schQueue[i]->getRowID())) {
                    mref = schQueue[i];
                    command = COMWRT;
                }
            }
        }
    }

    // Find the oldest ready (install) WRITE
    for(int i=0; (i < schQueue.size()) && (mref == NULL); i++) {
        if(schQueue[i]->needsSchedule() && !schQueue[i]->needsPrecharge() && !schQueue[i]->needsActivate() && !schQueue[i]->needsUpdate()) {
            if(schQueue[i]->needsInstall()) {
                if(canIssueWrite(schQueue[i]->getRankID(), schQueue[i]->getBankID(), schQueue[i]->getRowID())) {
                    mref = schQueue[i];
                    command = COMINS;
                }
            }
        }
    }

    // Find the oldest ready (update) WRITE
    for(int i=0; (i < schQueue.size()) && (mref == NULL); i++) {
        if(schQueue[i]->needsSchedule() && !schQueue[i]->needsPrecharge() && !schQueue[i]->needsActivate()) {
            if(schQueue[i]->needsUpdate()) {
                if(canIssueWrite(schQueue[i]->getRankID(), schQueue[i]->getBankID(), tagBuffer[schQueue[i]->getRankID()][schQueue[i]->getBankID()].getRowID())) {
                    mref = schQueue[i];
                    command = COMUDT;
                }
            }
        }
    }

    // Find the oldest ready ACTIVATE
    for(int i=0; (i < schQueue.size()) && (mref == NULL); ++i) {
        if(schQueue[i]->needsSchedule() && schQueue[i]->needsActivate() && !schQueue[i]->needsUpdate()) {
            if(canIssueActivate(schQueue[i]->getRankID(), schQueue[i]->getBankID())) {
                mref = schQueue[i];
                command = COMACT;
            }
        }
    }

    // Find the oldest ready PRECHARGE
    for(int i=0; (i < schQueue.size()) && (mref == NULL); ++i) {
        if(schQueue[i]->needsSchedule() && schQueue[i]->needsPrecharge() && !schQueue[i]->needsUpdate()) {
            if(canIssuePrecharge(schQueue[i]->getRankID(), schQueue[i]->getBankID())) {
                mref = schQueue[i];
                command = COMPRE;
            }
        }
    }

    // Find the oldest ready (update) ACTIVATE
    for(int i=0; (i < schQueue.size()) && (mref == NULL); ++i) {
        if(schQueue[i]->needsSchedule() && schQueue[i]->needsActivate()) {
            if(canIssueActivate(schQueue[i]->getRankID(), schQueue[i]->getBankID())) {
                mref = schQueue[i];
                command = schQueue[i]->needsUpdate()? COMOPN: COMACT;
            }
        }
    }

    // Find the oldest ready (update) PRECHARGE
    for(int i=0; (i < schQueue.size()) && (mref == NULL); ++i) {
        if(schQueue[i]->needsSchedule() && schQueue[i]->needsPrecharge()) {
            if(canIssuePrecharge(schQueue[i]->getRankID(), schQueue[i]->getBankID())) {
                mref = schQueue[i];
                command = COMPRE;
            }
        }
    }

    // Issue a ready command
    if(mref != NULL) {
        if(command == COMPRE) {
            precharge(mref->getRankID(), mref->getBankID());
            mref->addLog("precharge %ld/%ld", mref->getRankID(), mref->getBankID());
        }
        else if(command == COMACT) {
            activate(mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->addLog("activate %ld/%ld/%ld", mref->getRankID(), mref->getBankID(), mref->getRowID());
        }
        else if(command == COMOPN) {
            activate(mref->getRankID(), mref->getBankID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID());
            mref->addLog("reopen %ld/%ld/%ld", mref->getRankID(), mref->getBankID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID());
        }
        else if(command == COMRED) {
            read(mref->getRankID(), mref->getBankID(), mref->getColID());
            mref->setReadPending(false);
            mref->setBurstPending(true, READ);
            mref->addLog("read %ld/%ld/%ld/data", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumRead();
        }
        else if(command == COMWRT) {
            write(mref->getRankID(), mref->getBankID(), mref->getColID(), NULL);
            mref->setWritePending(false);
            mref->setBurstPending(true, WRITE);
            mref->addLog("write %ld/%ld/%ld/data", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumWrite();
        }
        else if(command == COMLOD) {
            TagRow tagRow = read(mref->getRankID(), mref->getBankID(), mref->getColID());
            mref->setTagRow(tagRow);
            mref->setBurstPending(true, LOAD);
            mref->setOpenRowID(getOpenRowID(mref->getRankID(), mref->getBankID()));
            mref->addLog("load %ld/%ld/%ld/tag", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumLoad();
        }
        else if(command == COMUDT) {
            write(mref->getRankID(), mref->getBankID(), 0, &tagBuffer[mref->getRankID()][mref->getBankID()]);
            mref->setBurstPending(true, UPDATE);
            mref->addLog(":: %ld =? %ld =? %ld", mref->getRowID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID(), getOpenRowID(mref->getRankID(), mref->getBankID()));
            mref->addLog("update %ld/%ld/%ld/tag", mref->getRankID(), mref->getBankID(), tagBuffer[mref->getRankID()][mref->getBankID()].getRowID());
            mref->incNumUpdate();
        }
        else if(command == COMEVT) {
            read(mref->getRankID(), mref->getBankID(), 0);
            if(mref->incEvcCount()) {
                mref->setState(WRITEBK);
                mref->setBurstPending(true, READ);
            }
            mref->addLog("evict %ld/%ld/%ld/data", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumEvict();
        }
        else if(command == COMINS) {
            write(mref->getRankID(), mref->getBankID(), mref->getColID(), NULL);
            if(mref->incInsCount()) {
                mref->setState(JOBDONE);
                mref->setBurstPending(true, WRITE);
            }
            mref->addLog("install %ld/%ld/%ld/data", mref->getRankID(), mref->getBankID(), mref->getRowID());
            mref->incNumInstall();
        }
        else {
            printf("ERROR: unknown command selected in FR-FCFS!\n");
            exit(0);
        }
    }
}


// Closed row buffer algorithm
void WideIOVault::scheduleRBClose()
{
    for(int r = 0; r < numRanks; ++r) {
        for(int b = 0; b < numBanks; ++b) {
            ranks[r]->setPending(b, false);
        }
    }
    for(int i = 0; i < schQueue.size(); ++i) {
        ranks[schQueue[i]->getRankID()]->setPending(schQueue[i]->getBankID(), true);
    }
    for(int r = 0; r < numRanks; ++r) {
        for(int b = 0; b < numBanks; ++b) {
            if(!ranks[r]->getPending(b) && (getBankState(r, b) == ACTIVE) && canIssuePrecharge(r, b)) {
                ranks[r]->precharge(b);
                lastPrecharge = globalClock;
            }
        }
    }
}


//// Cache row buffer algorithm
//void WideIOVault::scheduleRBCache()
//{
//    for(int r = 0; r < numRanks; ++r) {
//        for(int b = 0; b < numBanks; ++b) {
//            ranks[r]->setPending(b, false);
//        }
//    }
//    for(int i = 0; i < schQueue.size(); ++i) {
//        ranks[schQueue[i]->getRankID()]->setPending(schQueue[i]->getBankID(), true);
//    }
//    for(int r = 0; r < numRanks; ++r) {
//        for(int b = 0; b < numBanks; ++b) {
//            if(!ranks[r]->getPending(b) && (getBankState(r, b) == ACTIVE) && canIssuePrecharge(r, b)) {
//                lastRows[r][b].rowID = getOpenRowID(r, b);
//                lastRows[r][b].rowTags = getOpenRow(r, b);
//                ranks[r]->precharge(b);
//                lastPrecharge = globalClock;
//            }
//        }
//    }
//}
//
//
//// Fetch row buffer algorithm
//void WideIOVault::scheduleRBFetch()
//{
//    for(int r = 0; r < numRanks; ++r) {
//        for(int b = 0; b < numBanks; ++b) {
//            ranks[r]->setPending(b, false);
//        }
//    }
//    for(int i = 0; i < schQueue.size(); ++i) {
//        ranks[schQueue[i]->getRankID()]->setPending(schQueue[i]->getBankID(), true);
//    }
//    for(int r = 0; r < numRanks; ++r) {
//        for(int b = 0; b < numBanks; ++b) {
//            bool found = false;
//            for(long i = 0; !found && (i < fetchRows.size()); ++i) {
//                if((fetchRows[i].rankID == r) && (fetchRows[i].bankID == b) && (fetchRows[i].rowID == getOpenRowID(r, b))) {
//                    found = true;
//                }
//            }
//            if(!found && (getBankState(r, b) == ACTIVE)) { // already not fetched and bank is active
//                if(!ranks[r]->getPending(b) || ranks[r]->getFetching(b)) { // no outstanding requests to schedule
//                  //if(ranks[r]->getReadCount(b) > 20) {
//                    if(ranks[r]->getReadCount(b) < 28) { // row is not read fully
//                        if(canIssueRead(r, b, getOpenRowID(r, b))) {
//                            ranks[r]->setFetching(b, true);
//                            read(r, b, 0);
//                        }
//                    }
//                    else if(canIssuePrecharge(r, b)) {
//                        ranks[r]->setFetching(b, false);
//                        CachedRowType fetchRow;
//                        fetchRow.rankID  = r;
//                        fetchRow.bankID  = b;
//                        fetchRow.rowID   = getOpenRowID(r, b);
//                        fetchRow.rowTags = getOpenRow(r, b);
//                        fetchRows.push_back(fetchRow);
//                        ranks[r]->precharge(b);
//                        lastPrecharge = globalClock;
//                    }
//                  //}
//                }
//            }
//        }
//    }
//    while(fetchRows.size() > fetchRowSize) {
//        fetchRows.erase(fetchRows.begin());
//    }
//}
//
//
//// ReAct row buffer algorithm
//void WideIOVault::scheduleRBReAct()
//{
//    for(int r = 0; r < numRanks; ++r) {
//        for(int b = 0; b < numBanks; ++b) {
//            ranks[r]->setPending(b, false);
//        }
//    }
//    for(int i = 0; i < schQueue.size(); ++i) {
//        ranks[schQueue[i]->getRankID()]->setPending(schQueue[i]->getBankID(), true);
//    }
//    for(int r = 0; r < numRanks; ++r) {
//        for(int b = 0; b < numBanks; ++b) {
//            if((getBankState(r, b) == ACTIVE) && (ranks[r]->getReadCount(b) > 0)) { // bank is used active
//                if(!ranks[r]->getPending(b)) { // no outstanding requests to schedule
//                    AddrType row = getNextOpenRowID(r, b);
//                    if((row >= 0) && (row < numRows)) {
//                        //ranks[r]->precharge(b);
//                        //lastPrecharge = globalClock;
//                        ranks[r]->activate(b, row);
//                        lastActivate = globalClock;
//                    }
//                }
//            }
//        }
//    }
//}
//
//
//// Fetch row buffer algorithm
//void WideIOVault::scheduleRBReTag()
//{
//    for(int r = 0; r < numRanks; ++r) {
//        for(int b = 0; b < numBanks; ++b) {
//            ranks[r]->setPending(b, false);
//        }
//    }
//    for(int i = 0; i < schQueue.size(); ++i) {
//        ranks[schQueue[i]->getRankID()]->setPending(schQueue[i]->getBankID(), true);
//    }
//    for(int r = 0; r < numRanks; ++r) {
//        for(int b = 0; b < numBanks; ++b) {
//            bool found = false;
//            for(long i = 0; !found && (i < fetchRows.size()); ++i) {
//                if((fetchRows[i].rankID == r) && (fetchRows[i].bankID == b) && (fetchRows[i].rowID == getOpenRowID(r, b))) {
//                    found = true;
//                }
//            }
//            if(!found && (getBankState(r, b) == ACTIVE)) { // already not fetched and bank is active
//                if(!ranks[r]->getPending(b)) { // no outstanding requests to schedule
//                    if(canIssuePrecharge(r, b)) { // ready to precharge
//                        CachedRowType fetchRow;
//                        fetchRow.rankID  = r;
//                        fetchRow.bankID  = b;
//                        fetchRow.rowID   = getOpenRowID(r, b);
//                        fetchRow.rowTags = getOpenRow(r, b);
//                        fetchRows.push_back(fetchRow);
//                        ranks[r]->precharge(b);
//                        lastPrecharge = globalClock;
//                    }
//                }
//            }
//        }
//    }
//    while(fetchRows.size() > fetchRowSize) {
//        fetchRows.erase(fetchRows.begin());
//    }
//}


// Basic refresh algorithm
//void WideIOVault::scheduleBasicRefresh()
//{
//    // check all ranks for refresh
//    for(int r=0; r < numRanks; ++r) {
//        if(neededRefreshes(r, 0) && canIssueRefresh(r, 0)) {
//            refresh(r, 0);
//            updateQueueState();
//        }
//    }
//}

// DeferUntilEmpty refresh algorithm
//void WideIOVault::scheduleDUERefresh()
//{
//    // check all ranks for refresh
//    for(int r=0; r < numRanks; ++r) {
//        int backlog = neededRefreshes(r, 0);
//
//        if(ranks[r]->needsUrgentRefresh()) {
//            if(canIssueRefresh(r, 0)) {
//                refresh(r, 0);
//                updateQueueState();
//            }
//            ranks[r]->setUrgentRefresh(backlog != 0);
//        }
//        else if(backlog > 0) {
//            switch (backlog) {
//                case 0:
//                case 1:
//                case 2:
//                case 3:
//                case 4:
//                case 5:
//                case 6:
//
//                    // check for any pending requests in rank buffers
//                    for(int i=0; i < schQueue.size(); ++i) {
//                        if(schQueue[i]->getRankID() == r) {
//                            break;
//                        }
//                    }
//
//                    // issue the refresh command if allowed
//                    if(canIssueRefresh(r, 0)) {
//                        refresh(r, 0);
//                        updateQueueState();
//                    }
//                    break;
//
//                case 7:
//                    // high priority
//
//                    // issue the refresh command if allowed
//                    if(canIssueRefresh(r, 0)) {
//                        refresh(r, 0);
//                        updateQueueState();
//                    }
//                    ranks[r]->setUrgentRefresh(true);
//                    break;
//
//                default:
//                    printf("ERROR: number of postponed refresh commands exceeded the limit!\n");
//                    exit(0);
//            }
//        }
//    }
//}


// Count the number of pending requests per row
ReqCount WideIOVault::getReqCount(AddrType rankID, AddrType bankID, AddrType rowID)
{
    ReqCount load;
    load.vaultCount = schQueue.size();
    load.rankCount = load.bankCount = load.rowCount = 0;
    for(int i = 0; i < load.vaultCount; ++i) {
        if(schQueue[i]->getRankID() == rankID) {
            load.rankCount++;
            if(schQueue[i]->getBankID() == bankID) {
                load.bankCount++;
                if(schQueue[i]->getRowID() == rowID) {
                    load.rowCount++;
                }
            }
        }
    }
    return load;
}


// WideIO Interface
WideIO::WideIO(MemorySystem* current, const char *section, const char *name)
  /* constructor {{{1 */
  : MemObj(section, name)
  //, nCompletedRef(0)
  , clockScheduled(false)
  , countReceived("%s:received", name)
  , countServiced("%s:serviced", name)
  , countSentLower("%s:sentlower", name)
  , countWriteBack("%s:writeback", name)
  , countSkipped("%s:skipped", name)
  //, countWriteAfterHit("%s:writeafterhit", name)
  //, countFillAfterMiss("%s:fillaftermiss", name)

  , pieTagAccess("%s:pieTagAccess", name)
  , countHit("%s:hit", name)
  , countMiss("%s:miss", name)
  , countRead("%s:read", name)
  , countWrite("%s:write", name)
  , countLoad("%s:load", name)
  , countUpdate("%s:update", name)
  , countEvict("%s:evict", name)
  , countInstall("%s:install", name)
  , tracHitRatio("%s:tracHitRatio", name)

  , freqBlockUsage("%s:freqBlockUsage", name)
  , freqBlockAccess("%s:freqBlockAccess", name)
  //, freqPageUsage("%s:freqPageUsage", name)
  //, tracSkipRatio("%s:tracSkipRatio", name)
  //, heatRead("%s:heatRead", name)
  //, heatWrite("%s:heatWrite", name)
  //, heatReadHit("%s:heatReadHit", name)
  //, heatWriteHit("%s:heatWriteHit", name)
  //, heatBlockMiss("%s:heatBlockMiss", name)
  //, heatBlockUsage("%s:heatBlockUsage", name)

  , avgAccessTime("%s:avgAccessTime", name)
  , tracAccessTime("%s:tracAccessTime", name)
{
  MemObj *lower_level = NULL;

  I(current);

  // check config parameters
  SescConf->isInt(section, "dispatch");
  SescConf->isPower2(section, "softPage", 0);
  SescConf->isPower2(section, "memSize", 0);
  SescConf->isPower2(section, "hbmSize", 0);
  SescConf->isPower2(section, "rowSize", 0);
  SescConf->isPower2(section, "blkSize", 0);
  //SescConf->isPower2(section, "tagSize", 0);
  //SescConf->isPower2(section, "numWays", 0);
  //SescConf->isPower2(section, "grnSize", 0);
  SescConf->isPower2(section, "numVaults", 0);
  SescConf->isPower2(section, "numRanks", 0);
  SescConf->isPower2(section, "numBanks", 0);

  // read in config parameters
  dispatch  = SescConf->getInt(section, "dispatch");
  softPage  = SescConf->getInt(section, "softPage");
  memSize   = SescConf->getInt(section, "memSize") * 1024LL * 1024LL; // HBM Size (MB)
  hbmSize   = SescConf->getInt(section, "hbmSize") * 1024LL * 1024LL; // HBM Size (MB)
  rowSize   = SescConf->getInt(section, "rowSize");
  blkSize   = SescConf->getInt(section, "blkSize");
  //tagSize   = SescConf->getInt(section, "tagSize");
  //numWays   = SescConf->getInt(section, "numWays");
  //grnSize   = SescConf->getInt(section, "grnSize");
  //numBlks   = rowSize / ((grnSize * blkSize) + tagSize);
  //numSets   = numBlks / numWays + ((numBlks % numWays)? 1: 0);
  numVaults = SescConf->getInt(section, "numVaults");
  numRanks  = SescConf->getInt(section, "numRanks");
  numBanks  = SescConf->getInt(section, "numBanks");
  numRows   = hbmSize / (numVaults * numRanks * numBanks * rowSize);

  // read in folder parameters
  if(dispatch == 6) {
      SescConf->isInt(section, "foldOffset");
      //SescConf->isInt(section, "numFolders");
      //SescConf->isInt(section, "maxFolder");
      SescConf->isInt(section, "numScouts");
      SescConf->isInt(section, "maxScout");
      foldOffset = SescConf->getInt(section, "foldOffset");
      //numFolders = SescConf->getInt(section, "numFolders");
      //maxFolder  = SescConf->getInt(section, "maxFolder");
      numScouts  = SescConf->getInt(section, "numScouts");
      maxScout   = SescConf->getInt(section, "maxScout");
      if(numScouts < 2) {
          printf("ERROR: at least two scouts are needed!\n");
          exit(0);
      }
      printf("Folder [foldOffset: %ld, numScouts: %ld, maxScout: %d]\n", foldOffset, numScouts, maxScout);
  }

  // intermediate parameters
  softPageLog2  = log2(softPage);
  memSizeLog2   = log2(memSize);
  hbmSizeLog2   = log2(hbmSize);
  rowSizeLog2   = log2(rowSize);
  blkSizeLog2   = log2(blkSize);
  numVaultsLog2 = log2(numVaults);
  numRanksLog2  = log2(numRanks);
  numBanksLog2  = log2(numBanks);
  numRowsLog2   = log2(numRows);
  //tagExtLog2    = 0;
  //for(long i = tagSize; i > 0; ++tagExtLog2, i -= blkSize);

  V = numVaultsLog2;
  R = numRanksLog2;
  X = (numRanksLog2+numBanksLog2+numRowsLog2-numVaultsLog2-(rowSizeLog2-blkSizeLog2))>>1;
  X = (X < 0)? 0: (X > numBanksLog2)? numBanksLog2: X;
  Y = numBanksLog2 - X;
  if((numVaultsLog2+X+(rowSizeLog2-blkSizeLog2))<(numRanksLog2+Y+numRowsLog2)) {
      if(Y != 0) {
          Y--;
          X++;
      }
  }
  P = numRowsLog2;
  C = rowSizeLog2 - blkSizeLog2;

  // internal organization
  vaults = *(new std::vector<WideIOVault *>(numVaults));
  for(int v = 0; v < numVaults; ++v) {
    char temp[256]; sprintf(temp, "%s:%d", name, v);
    vaults[v] = new WideIOVault(section, temp, numRanks, numBanks, numRows);
  }

  // enable refresh
  SescConf->isInt(section, "refAlgorithm");
  if(SescConf->getInt(section, "refAlgorithm") == 1) {
      for(int v = 0; v < numVaults; ++v) {
          for(int r = 0; r < numRanks; ++r) {
              vaults[v]->enableRefresh(r, (double)(v*numRanks+r) / (double) (numVaults*numRanks));
          }
      }
  }

  // statistics
  pieTagAccess.setPFName(SescConf->getCharPtr("", "reportFile",0));
  pieTagAccess.setPlotLabel("Breakdown of Data Transfer");
  pieTagAccess.addCounter(&countRead, "Reads");
  pieTagAccess.addCounter(&countWrite, "Writes");
  pieTagAccess.addCounter(&countLoad, "Loads");
  pieTagAccess.addCounter(&countUpdate, "Updates");
  pieTagAccess.addCounter(&countEvict, "Evicts");
  pieTagAccess.addCounter(&countInstall, "Installs");
  tracHitRatio.setPFName(SescConf->getCharPtr("", "reportFile",0));
  tracHitRatio.setPlotLabels("Execution Time (x1024 cycles)", "Block Hit Rate");

  freqBlockUsage.setPFName(SescConf->getCharPtr("", "reportFile",0));
  freqBlockUsage.setPlotLabels("Block Usage", "Frequency");
  freqBlockUsage.setBucketSize(64);
  freqBlockAccess.setPFName(SescConf->getCharPtr("", "reportFile",0));
  freqBlockAccess.setPlotLabels("Block Access Type (0: unused; 1: read; 2: write)", "Frequency");
  //freqPageUsage.setPFName(SescConf->getCharPtr("", "reportFile",0));
  //freqPageUsage.setPlotLabels("Page Usage", "Frequency");
  //heatRead.setPFName(SescConf->getCharPtr("", "reportFile",0));
  //heatRead.setPlotFunctions(&isBorder, &computeDimX, &computeX, &computeDimY, &computeY);
  //heatWrite.setPFName(SescConf->getCharPtr("", "reportFile",0));
  //heatWrite.setPlotFunctions(&isBorder, &computeDimX, &computeX, &computeDimY, &computeY);
  //heatReadHit.setPFName(SescConf->getCharPtr("", "reportFile",0));
  //heatReadHit.setPlotFunctions(&isBorder, &computeDimX, &computeX, &computeDimY, &computeY);
  //heatWriteHit.setPFName(SescConf->getCharPtr("", "reportFile",0));
  //heatWriteHit.setPlotFunctions(&isBorder, &computeDimX, &computeX, &computeDimY, &computeY);
  //heatBlockMiss.setPFName(SescConf->getCharPtr("", "reportFile",0));
  //heatBlockMiss.setPlotFunctions(&isBorder, &computeDimX, &computeX, &computeDimY, &computeY);
  //heatBlockUsage.setPFName(SescConf->getCharPtr("", "reportFile",0));
  //heatBlockUsage.setPlotFunctions(&isBorder, &computeDimX, &computeX, &computeDimY, &computeY);

  //tracSkipRatio.setPFName(SescConf->getCharPtr("", "reportFile",0));
  //tracSkipRatio.setPlotLabels("Execution Time (x1024 Cycles)", "Skip Ratio of Blocks");
  tracAccessTime.setPFName(SescConf->getCharPtr("", "reportFile",0));
  tracAccessTime.setPlotLabels("Execution Time (x1024 Cycles)", "Average Access Time (cycles)");

  // display conf
  printf("WideIO [dispatch: %lld, softPage: %lld, memSize: %lld, hbmSize: %lld, rowSize: %lld, blkSize: %lld, numVaults: %lld, numRanks: %lld, numBanks: %lld, numRows: %lld]\n", dispatch, softPage, memSize, hbmSize, rowSize, blkSize, numVaults, numRanks, numBanks, numRows);
  //printf("WideIO[Vaults:%ld, Ranks:%ld, Banks:%ld, rowSize:%ld, hbmSize:%ld, Rows:%ld, blkSize:%ld, tagSize:%ld, Blks:%ld]\n", numVaults, numRanks, numBanks, rowSize, hbmSize, numRows, blkSize, tagSize, numBlks);

  // set the lower level
  lower_level = current->declareMemoryObj(section, "lowerLevel");
  if(lower_level) {
    addLowerLevel(lower_level);
  }
}
/* }}} */

void WideIO::doReq(MemRequest *mreq)
  /* request reaches the memory controller {{{1 */
{
#if 0
#define BLKBITS 6
#define BLKSIZE (1<<BLKBITS)
#define BLKWORD (BLKSIZE>>3)
  char *MsgActionStr[9] = {
        "ma_setInvalid",
        "ma_setValid",
        "ma_setDirty",
        "ma_setShared",
        "ma_setExclusive",
        "ma_MMU",
        "ma_VPCWU",
        "ma_MAX",
        NULL
  };
  uint64_t *data = NULL;//(uint64_t *) mreq->getData();
  //if(mreq->getAction() == ma_setExclusive) {
    printf("doReq\t%s\t0x%llx ", MsgActionStr[mreq->getAction()], mreq->getAddr());
    if(data) {
      for(int i=BLKWORD-1; i >= 0; --i) {
        printf("%016lx", data[i]);
      }
    }
    printf("\n"); fflush(stdout);
  //}
#endif

  if(mreq->getAction() == ma_setDirty) {
  	  addRequest(mreq, false);
  }
  else {
  	  addRequest(mreq, true);
  }
}
/* }}} */

void WideIO::doReqAck(MemRequest *lreq)
  /* push up {{{1 */
{
    WideIOReference *mref = (WideIOReference *) lreq->getMRef();
    if((mref != NULL) && (mref->getState() != UNKNOWN)) {
        mref->addLog("ack with %p", lreq);
        if(mref->getState() == SKIPPED) {
            mref->sendUp();
            pendingList.remove(mref);
            delete mref;
        }
        else if(mref->getState() == DOFETCH) {
            free(lreq);
            if(mref->incFchCount()) {
                mref->sendUp();
                pendingList.remove(mref);
                vaults[mref->getVaultID()]->retReference(mref);
                //mref->printState();
                //mref->printLog();
                if(mref->isFolded()) {
                    //printf("return to %ld", mref->getVaultID());
                    for(long i = 1; i < 2; i++) {
                        long v = (mref->getVaultID() + i*numVaults/2) % numVaults;
                        //printf(" %ld", v);
                        WideIOReference *temp = new WideIOReference();
                        //memcpy(temp, mref, sizeof(WideIOReference));
                        temp->setMReq(NULL);
                        temp->setMAddr(mref->getMAddr());
                        temp->setVaultID(v);
                        temp->setRankID(mref->getRankID());
                        temp->setBankID(mref->getBankID());
                        temp->setRowID(mref->getRowID());
                        temp->setColID(mref->getColID());
                        temp->setRead(mref->isRead());
                        temp->setBlkSize(blkSize);
                        temp->setNumGrain(mref->getNumGrain());
                        temp->setSetID(mref->getSetID());
                        temp->setTagID(mref->getTagID());
                        //temp->setReadPending();
                        //temp->setWritePending();
                        //temp->setBurstPending();
                        temp->setFolded(true);
                        temp->setReplica(true);
                        temp->setTagIdeal(false);
                        temp->addLog("replica");
                        temp->setState(DOFETCH);
                        vaults[v]->retReference(temp);
                        //temp->printState();
                        //temp->printLog();
                    }
                    //printf("\n");
                }
            }
        }
        else {
            MSG("WideIO: an acknowledge with unknown state returned from lower level!\n");
        }
    }
}
/* }}} */

void WideIO::doDisp(MemRequest *mreq)
  /* push down {{{1 */
{
#if 0
#define BLKBITS 6
#define BLKSIZE (1<<BLKBITS)
#define BLKWORD (BLKSIZE>>3)
  char *MsgActionStr[9] = {
        "ma_setInvalid",
        "ma_setValid",
        "ma_setDirty",
        "ma_setShared",
        "ma_setExclusive",
        "ma_MMU",
        "ma_VPCWU",
        "ma_MAX",
        NULL
  };
  uint64_t *data = NULL; //(uint64_t *) mreq->getData();
  //if(mreq->getAction() == ma_setExclusive) {
    printf("doDisp\t%s\t0x%llx ", MsgActionStr[mreq->getAction()], mreq->getAddr());
    if(data) {
      for(int i=BLKWORD-1; i >= 0; --i) {
        printf("%016lx", data[i]);
      }
    }
    printf("\n"); fflush(stdout);
  //}
#endif

  //MSG("\nWideIO::doDisp for Addr %llx", mreq->getAddr());
  if(mreq->getAction() == ma_setDirty) {
  	  addRequest(mreq, false);
  }
  else {
      mreq->ack();
  }
}
/* }}} */

void WideIO::doSetState(MemRequest *mreq)
  /* push up {{{1 */
{
  MSG("\nWideIO::doSetState for Addr %llx", mreq->getAddr());
  I(0);
}
/* }}} */

void WideIO::doSetStateAck(MemRequest *mreq)
  /* push up {{{1 */
{
  MSG("\nWideIO::doSetStateAck for Addr %llx", mreq->getAddr());
//  I(0);
}
/* }}} */

bool WideIO::isBusy(AddrType addr) const
/* always can accept writes {{{1 */
{
  return false;
}
/* }}} */

void WideIO::tryPrefetch(AddrType addr, bool doStats)
  /* try to prefetch to openpage {{{1 */
{
  MSG("\nWideIO::tryPrefetch for Addr %llx", addr);
  // FIXME:
}
/* }}} */

TimeDelta_t WideIO::ffread(AddrType addr)
  /* fast forward reads {{{1 */
{
  MSG("\nWideIO::ffread for Addr %llx", addr);
  return 0; //delay + RowAccessLatency;
}
/* }}} */

TimeDelta_t WideIO::ffwrite(AddrType addr)
  /* fast forward writes {{{1 */
{
  MSG("\nWideIO::ffwrite for Addr %llx", addr);
  return 0; //delay + RowAccessLatency;
}
/* }}} */


// adding memory requests
void WideIO::addRequest(MemRequest *mreq, bool read)
{
    AddrType maddr = mreq->getAddr() & (memSize-1);
    AddrType vaultID, rankID, bankID, rowID, colID; //, tagID, blkID;

    freqBlockUsage.sample(true, maddr>>blkSizeLog2); // for Alpha
    freqBlockAccess.sample(false, maddr>>blkSizeLog2, read? 1: 2); // for Gamma TODO:  this sample must be taken when the access processed with RedCache

    // map DRAM fields [rowID/vaultID/rankID/bankID/colID]
    colID   = (maddr &  (rowSize - 1));
    bankID  = (maddr >>  rowSizeLog2)&(numBanks - 1);
    rankID  = (maddr >> (rowSizeLog2 + numBanksLog2)) & (numRanks - 1);
    vaultID = (maddr >> (rowSizeLog2 + numBanksLog2   +  numRanksLog2)) & (numVaults - 1);
    rowID   = (maddr >> (rowSizeLog2 + numBanksLog2   +  numRanksLog2 +    numVaultsLog2)) & (numRows - 1);

    WideIOReference *mref = new WideIOReference();
    mref->setMReq(mreq);
    mref->setMAddr(maddr);
    mref->setVaultID(vaultID);
    mref->setRankID(rankID);
    mref->setBankID(bankID);
    mref->setRowID(rowID);
    mref->setColID(colID);
    mref->setRead(read);
    mref->addLog("received");


    // override dispatch mode by software
    //printf("addRequest %lx\n", mreq->getSMode());
    if(mreq->getSMode() & SIG_CAMPAD) {
        printf("ERROR: unsupported mode (%ld)!\n", mreq->getSMode());
        exit(0);
    }
    if(mreq->getSMode() & SIG_RAMPAD) {
        //printf("RAMPAD SIGNAL RECEIVED HERE\n");
        dispatch = 3;
    }

    if(firstaddreqcall == 0)
    {
      printf("First Req Received  %d\n", receivedQueue.size());
      KMEANSREQRECEIVED = true;
      firstaddreqcall = 1;
    }
    WideIODelayedQ.push(mref);
    countReceived.inc();
    // push request into receivedQueue
    /*
    if(dispatch == 0) { // normal cache
        mref->setFolded(false);
        mref->setTagIdeal(false);
        mref->setState(READTAG);
        receivedQueue.push(mref);
        countReceived.inc();
    }
    else if(dispatch == 1) { // loopback
        mref->setFolded(false);
        mref->setTagIdeal(false);
        mref->setState(LOOPBAK);
        receivedQueue.push(mref);
        countReceived.inc();
    }
    else if(dispatch == 2) { // bypass
        mref->setFolded(false);
        mref->setTagIdeal(false);
        mref->setState(SKIPPED);
        receivedQueue.push(mref);
        countReceived.inc();
    }
    else if(dispatch == 3) { // scratchpad
        mref->setFolded(false);
        mref->setTagIdeal(false);
        mref->setState(SCRATCH);
        receivedQueue.push(mref);
        countReceived.inc();
    }
    else if(dispatch == 4) { // ideal cache
        mref->setFolded(false);
        mref->setTagIdeal(true);
        mref->setState(READTAG);
        receivedQueue.push(mref);
        countReceived.inc();
    }
    else if(dispatch == 5) { // replicated cache
        long cntVlt, cntRow, tmpRow;
        long minVlt, maxRow;
        cntVlt = 1000000;
        cntRow = tmpRow = 0;
        minVlt = maxRow = vaultID;
        // find a lightly loaded
        //printf("send to");
        for(long i = 0; i < 2; i++) {
            long v = (vaultID + i*numVaults/2) % numVaults;
            //printf(" [%ld]%ld", i, v);
            if(vaults[v]->isRefreshing(rankID) == false) {
                ReqCount rc = vaults[v]->getReqCount(rankID, bankID, rowID);
                if(cntRow < rc.rowCount) {
                    cntRow = rc.rowCount;
                    maxRow = v;
                }
                if(cntVlt > rc.vaultCount) {
                    cntVlt = rc.vaultCount;
                    tmpRow = rc.rowCount;
                    minVlt = v;
                }
            }
        }
        if(cntRow > tmpRow) {
            mref->setVaultID(maxRow);
        }
        else {
            mref->setVaultID(minVlt);
        }
        //printf(" (%ld)\n", mref->getVaultID());

        mref->setFolded(true);
        mref->setTagIdeal(false);
        mref->addLog("original");
        mref->setState(READTAG);
        receivedQueue.push(mref);
        countReceived.inc();

        //for(long v = 0; v < numVaults; v++) {
        //    if(v != vaultID) {
        //        WideIOReference *temp = new WideIOReference();
        //        temp->setMReq(mreq);
        //        temp->setMAddr(maddr);
        //        temp->setVaultID(v);
        //        temp->setRankID(rankID);
        //        temp->setBankID(bankID);
        //        temp->setRowID(rowID);
        //        temp->setColID(colID);
        //        temp->setRead(read);
        //        temp->setFolded(true);
        //        temp->setTagIdeal(false);
        //        temp->addLog("replica");
        //        temp->setState(READTAG);
        //        mref->addSibling(temp);
        //        receivedQueue.push(temp);
        //        countReceived.inc();
        //    }
        //}
    }
    else if(dispatch == 6) { // folder cache
        long index;
        bool found, trigger;
        AddrType mpage = maddr>>softPageLog2;

        // search folders
        index = 0;
        found = trigger = false;
        for(long i = 0; !found && (i < folders.size()); ++i) {
            if(folders[i].mpage == mpage) {
                found = true;
                index = i;
            }
        }

        // send request to destination(s)
        if(false) {//(!found) {
            mref->setFolded(false);
            mref->setTagIdeal(false);
            mref->setState(SKIPPED);
            receivedQueue.push(mref);
            countReceived.inc();
        }
        else {
            bool folded = found; // TODO: compute based on folder counts
            mref->setFolded(folded);
            mref->setTagIdeal(false);
            mref->addLog("original");
            mref->setState(READTAG);
            receivedQueue.push(mref);
            countReceived.inc();
            for(long i = foldOffset; folded && (i < numVaults); i += foldOffset) {
                long v = (vaultID + i) % numVaults;
                WideIOReference *temp = new WideIOReference();
                temp->setMReq(mreq);
                temp->setMAddr(maddr);
                temp->setVaultID(v);
                temp->setRankID(rankID);
                temp->setBankID(bankID);
                temp->setRowID(rowID);
                temp->setColID(colID);
                temp->setRead(read);
                temp->setFolded(true);
                temp->setTagIdeal(false);
                temp->addLog("replica");
                temp->setState(READTAG);
                mref->addSibling(temp);
                receivedQueue.push(temp);
                countReceived.inc();
            }
        }

        // scout for foldable data
        index = 0;
        found = trigger = false;
        for(long i = 0; !found && (i < scouts.size()); ++i) {
            if(scouts[i].mpage == mpage) {
                scouts[i].count++;
                if(scouts[i].count >= maxScout) {
                    trigger = true;
                }
                found = true;
                index = i;
            }
        }

        // update scouts and folders
        if(trigger) {
            scouts.erase(scouts.begin() + index);
            found = false;
            for(long i = 0; !found && (i < folders.size()); ++i) {
                if(folders[i].mpage == mpage) {
                    found = true;
                    index = i;
                }
            }
            if(found) {
                folders[index].count++;
                printf("WideIO: folder incremented for %llx.\n", mpage);
            }
            else {
                Folder f;
                f.mpage  = mpage;
                f.count  = 1;
                folders.push_back(f);
                printf("WideIO: folder added for %llx.\n", mpage);
            }
        }
        else {
            if(found) {
                for(long i = 0; i < scouts.size(); ++i) {
                    scouts[i].recent = (i == index);
                }
            }
            else {
                if(scouts.size() == numScouts) {
                    for(long i = 0; i < scouts.size(); ++i) {
                        if(!scouts[i].recent) {
                            scouts.erase(scouts.begin() + i);
                            break;
                        }
                    }
                }
                for(long i = 0; i < scouts.size(); ++i) {
                    scouts[i].recent = false;
                }
                Scout s;
                s.mpage  = mpage;
                s.count  = 1;
                s.recent = true;
                scouts.push_back(s);
            }
        }
    }
    else {
        printf("ERROR: unknown dispatch mode '%d' for WideIO!\n", dispatch);
        exit(0);
    }*/
    // schedule a clock edge
    if(!clockScheduled) {
        clockScheduled = true;
        CallbackBase *cb = ManageWideIOCB::create(this);
        EventScheduler::schedule((TimeDelta_t) 1, cb);
    }
}


// WideIO cycle by cycle management
void WideIO::manageWideIO(void)
{
  //printf("Clock Cycle\n");
  finishWideIO();
  //printf("Clock Cycle 1\n");
  completeMRef();
  //printf("Clock Cycle 2\n");
  doWriteBacks();
  //printf("Clock Cycle 3\n");
  doFetchBlock();
  //printf("Clock Cycle 4\n");
  doSkipBlocks();
  //printf("Clock Cycle 5\n");
  dispatchRefs();
  //printf("Clock Cycle 6\n");
  //if(KMEANSLASTREQRECVD == false)
  //{
  doKMeans();
  //}


  //if(KMEANSLASTREQRECVD == true)
  //{
    //printf("Clock after all requests pushed %lf %lf\n", countServiced.getDouble(), countReceived.getDouble()); fflush(stdout);
  //}

  for(int c = 0; c < numVaults; ++c) {
    vaults[c]->clock();
  }
  //printf("Clocking per vault complete\n");
  // sample trackers
  if((globalClock & 0x3FFFF) == 0) {
    //tracSkipRatio.sample(true, globalClock, countSkipped.getDouble()/countReceived.getDouble());
    tracHitRatio.sample(true, globalClock>>10, countHit.getDouble() / (countHit.getDouble() + countMiss.getDouble()));
    tracAccessTime.sample(true, globalClock>>10, avgAccessTime.getDouble());
  }

  // schedule a clock edge
  //if(globalClock > 2698340) printf("@%10lld : %ld\t%ld\n", globalClock, countServiced.getDouble(), countReceived.getDouble());
  clockScheduled = false;
  if(countServiced.getDouble() < countReceived.getDouble()) {
    //printf("Scheduling next clock done\n");
    clockScheduled = true;
    CallbackBase *cb = ManageWideIOCB::create(this);
    EventScheduler::schedule((TimeDelta_t) 1, cb);
  }


}


// process skipped references
void WideIO::doSkipBlocks(void)
{
  for(int i=0; i < WideIOSkippedQ.size(); ++i) {
    WideIOReference *mref = WideIOSkippedQ.front();
    MemRequest *mreq = mref->getMReq();
    mreq->setMRef((void *)mref);
    router->scheduleReq(mreq, 1);
    mref->addLog("descend %p", mreq);
    mref->setState(SKIPPED);
    pendingList.append(mref);
    WideIOSkippedQ.pop();
    countSkipped.inc();
  }
}


// complete WideIO references
void WideIO::completeMRef(void)
{
  for(int i=0; i < WideIOCompleteQ.size(); ++i) {
    WideIOReference *mref = WideIOCompleteQ.front();
    mref->sendUp();

    if(mref->getNumMatch()) {
        countHit.inc();
    }
    else {
        countMiss.inc();
    }
    countRead.add(mref->getNumRead(), true);
    countWrite.add(mref->getNumWrite(), true);
    countLoad.add(mref->getNumLoad(), true);
    countUpdate.add(mref->getNumUpdate(), true);
    countEvict.add(mref->getNumEvict(), true);
    countInstall.add(mref->getNumInstall(), true);
    //if(mref->isCanceled()) {
    //    mref->printState();
    //    mref->printLog();
    //    exit(0);
    //}

    delete mref;
    WideIOCompleteQ.pop();
  }
}


// process write back requests
void WideIO::doWriteBacks(void)
{
  for(int i=0; i < WideIOWriteBackQ.size(); ++i) {
    WideIOWriteBack wb = WideIOWriteBackQ.front();
    router->sendDirtyDisp(wb.maddr, true, 1);
    WideIOWriteBackQ.pop();
    countWriteBack.inc();
  }
}


// process fetch references
void WideIO::doFetchBlock(void)
{
  for(int i = 0; i < WideIOFetchedQ.size(); ++i) {
    WideIOReference *mref = WideIOFetchedQ.front();
    for(long j=0; j < mref->getNumGrain(); ++j) {
        MemRequest *temp = MemRequest::createReqRead(this, true, mref->getGrainAddr(j));
        temp->setMRef((void *)mref);
        router->scheduleReq(temp, 1);
        mref->addLog("fetch 0x%llx with %p", mref->getGrainAddr(j), temp);
    }
    pendingList.append(mref);
    WideIOFetchedQ.pop();
    countSentLower.inc();
  }
}

//send data to logic layer for processing kmeans_acc
void WideIO::doKMeans(void)
{
  //printf("Enters KMeans Loop\n");
  AddrType Addr = 1423*64;
  KMeans_start_addr = Addr;
  if((!KMEANS_FIRSTCALL) && (WideIODelayedQ.size()))
  {
    //printf("Enters Initial Request Push");
    currservaddr = Addr;
    for(int i = 0; i < 1; i ++)
    {
      //MemRequest *mreq = MemRequest::createReqRead(this, true, Addr + 64*i); //blockwise access
      //printf("Request Created\n");
      AddrType maddr = Addr + 64*i;
      AddrType vaultID, rankID, bankID, rowID, colID; //, tagID, blkID;

      // map DRAM fields [rowID/vaultID/rankID/bankID/colID]
      colID   = (maddr &  (rowSize - 1));
      bankID  = (maddr >>  rowSizeLog2)&(numBanks - 1);
      rankID  = (maddr >> (rowSizeLog2 + numBanksLog2)) & (numRanks - 1);
      vaultID = (maddr >> (rowSizeLog2 + numBanksLog2   +  numRanksLog2)) & (numVaults - 1);
      rowID   = (maddr >> (rowSizeLog2 + numBanksLog2   +  numRanksLog2 +    numVaultsLog2)) & (numRows - 1);

      WideIOReference *mref = new WideIOReference();
      mref->setMReq(NULL);
      mref->setMAddr(maddr);
      mref->setVaultID(vaultID);
      mref->setRankID(rankID);
      mref->setBankID(bankID);
      mref->setRowID(rowID);
      mref->setColID(colID);
      mref->setRead(true);
      mref->setFolded(false);
      mref->setTagIdeal(false);
      mref->setState(SCRATCH);
      mref->addLog("received");
      receivedQueue.push(mref);
      countReceived.inc();
      PushedReqKMeans = PushedReqKMeans + 1;
      currservaddr = maddr;
    }


    KMEANS_FIRSTCALL = true;
  }

  //printf("Here after loop print\n");
  while(receivedQueue.size() < 1 && (!KMEANSLASTREQRECVD))
  {
    //print("Size Check ReceivedQ %d\n", receivedQueue.size());
    //currservaddr = currservaddr + 64;
    //printf("Loop2 check\n");
    if(currservaddr >= KMeans_start_addr + NO_OF_POINTS*NO_OF_DIMENSIONS*8)
    {
      //printf("Reset Happens\n");
      currservaddr = KMeans_start_addr;
    }
    if(PushedReqKMeans == (NO_OF_POINTS*NO_OF_DIMENSIONS/8)*NUM_ITERATIONS)
    {
      //printf("LastReqReached\n");
      KMEANSLASTREQRECVD = true;
      KMEANSREQRECEIVED = false;
    }

      //MemRequest *mreq = MemRequest::createReqRead(this, true, Addr + currservaddr); //blockwise access
      AddrType maddr = (Addr + currservaddr) & (memSize-1);
      AddrType vaultID, rankID, bankID, rowID, colID; //, tagID, blkID;

      //freqBlockUsage.sample(true, maddr>>blkSizeLog2); // for Alpha
      //freqBlockAccess.sample(false, maddr>>blkSizeLog2, read? 1: 2); // for Gamma TODO:  this sample must be taken when the access processed with RedCache

      // map DRAM fields [rowID/vaultID/rankID/bankID/colID]
      colID   = (maddr &  (rowSize - 1));
      bankID  = (maddr >>  rowSizeLog2)&(numBanks - 1);
      rankID  = (maddr >> (rowSizeLog2 + numBanksLog2)) & (numRanks - 1);
      vaultID = (maddr >> (rowSizeLog2 + numBanksLog2   +  numRanksLog2)) & (numVaults - 1);
      rowID   = (maddr >> (rowSizeLog2 + numBanksLog2   +  numRanksLog2 +    numVaultsLog2)) & (numRows - 1);

      WideIOReference *mref = new WideIOReference();
      mref->setMReq(NULL);
      mref->setMAddr(maddr);
      mref->setVaultID(vaultID);
      mref->setRankID(rankID);
      mref->setBankID(bankID);
      mref->setRowID(rowID);
      mref->setColID(colID);
      mref->setRead(true);
      mref->setFolded(false);
      mref->setTagIdeal(false);
      mref->setState(SCRATCH);
      mref->addLog("received");
      receivedQueue.push(mref);
      countReceived.inc();
      PushedReqKMeans = PushedReqKMeans + 1;
      currservaddr = currservaddr + 64;
  }

  if(KMEANSLASTREQRECVD)
  {
    printf("Pushes Requests from Delayed!\n");
    while(WideIODelayedQ.size() > 0)
    {
      printf("DelayedQueue Size %d\n", WideIODelayedQ.size());
      WideIOReference * mref = WideIODelayedQ.front();
      mref->setFolded(false);
      mref->setTagIdeal(false);
      mref->setState(READTAG);
      receivedQueue.push(mref);

      WideIODelayedQ.pop();
    }
  }
}
// process serviced request
void WideIO::finishWideIO(void)
{
  while(WideIOServicedQ.size()) {
    //printf("YES FINISHING REQ KMEANS"); fflush(stdout);
    ServicedRequest sreq = WideIOServicedQ.front();
    /*if(sreq.mreq == NULL)
    {
      printf("YES FINISHING REQ KMEANS 1"); fflush(stdout);
      WideIOServicedQ.pop();
      countServiced.inc();
      return;
    }*/
    I(sreq.mreq);
    if(sreq.mreq->isDisp()) {
      sreq.mreq->ack();  // Fixed doDisp Acknowledge -- LNB 5/28/2014
    }
    else {
      I(sreq.mreq->isReq());

      if (sreq.mreq->getAction() == ma_setValid || sreq.mreq->getAction() == ma_setExclusive) {
        sreq.mreq->convert2ReqAck(ma_setExclusive);
      }
      else {
        sreq.mreq->convert2ReqAck(ma_setDirty);
      }

      avgAccessTime.sample(sreq.time, true);
      router->scheduleReqAck(sreq.mreq, 1);  //  Fixed doReq acknowledge -- LNB 5/28/2014
    }

    WideIOServicedQ.pop();
    countServiced.inc();

  }

  while(KMeansCompleteQ.size())
  {
    KMeansCompleteQ.pop();
    countServiced.inc();
  }
}


// dispatch reference
void WideIO::dispatchRefs(void)
{
  WideIOReference *mref;
  bool notFull = true;

  while(receivedQueue.size() && notFull) {
    mref = receivedQueue.front();
    switch(mref->getState()) {
    case LOOPBAK:
      mref->sendUp();
      delete mref;
      receivedQueue.pop();
      break;

    case SKIPPED:
      WideIOSkippedQ.push(mref);
      receivedQueue.pop();
      break;

    case SCRATCH:
    case READTAG:
      if(notFull = !vaults[mref->getVaultID()]->isFull()) {
        vaults[mref->getVaultID()]->addReference(mref);
        receivedQueue.pop();
      }
      break;
    }
  }
}