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
#include "XAMIO.h"
#include "../../../apps-ananth/esesc/simusignal.h"
#include <map>
#include <fstream>
//#include <iostream>
//#include "stdlib.h"
//#include <queue>
//#include <cmath>


// XAMIO clock, scaled wrt globalClock
//Time_t globalClock;
std::queue<ServicedRequest> XAMIOServicedQ;
std::queue<XAMIOReference *> XAMIOFetchedQ;
std::queue<XAMIOReference *> XAMIOSkippedQ;
std::queue<XAMIOWriteBack> XAMIOWriteBackQ;
std::queue<XAMIOReference *> XAMIOCompleteQ;
std::map<unsigned long int, int> EnduranceMap;


// memory reference constructor
XAMIOReference::XAMIOReference()
  : mreq(NULL)
  , sentUp(false)
{
  birthTime = globalClock;
  climbTime = globalClock;
  deathTime = globalClock;
}


// send a response up
void XAMIOReference::sendUp() {
    if((sentUp != true) && ((mreq != NULL) || isKeyReset() || isMaskReset())) {
        climbTime = globalClock;
        deathTime = globalClock;
        ServicedRequest sreq;
        sreq.time = climbTime - birthTime;
        sreq.mreq = mreq;
        XAMIOServicedQ.push(sreq);
        sentUp    = true;
        addLog("climb");
    }
    else {
        deathTime = globalClock;
    }
}


// set constructor
XAMIOSet::XAMIOSet(const char *name, AddrType numRows, AddrType numCols)
{
  this->numRows = numRows;
  this->numCols = numCols;
  this->numSubs = numCols/numRows;

  // internal organization
  if(numRows != (sizeof(DataType)*8)) {
      printf("ERROR: numRows must be %d for now!\n", sizeof(DataType)<<3);
      exit(0);
  }
  content = (DataType *)malloc(numCols*sizeof(DataType));
}

// Write a block
void XAMIOSet::write(XAMMode mode, AddrType index, DataType *data)
{
    if((mode == COLINP) && (index < numRows)) {
        for(int i = 0; i < numSubs; ++i) {
            content[i*numRows + index] = data[i];
        }
    }
    else if((mode == ROWINP) && (index < numRows)) {
        int c = 0;
        DataType temp = 1LL<<index;
        for(int i = 0; i < numSubs; ++i) {
            DataType word = data[i];
            for(int j = 0; j < numRows; ++j) {
                if(word & 1LL) {
                    content[c] |= temp;
                }
                else {
                    content[c] &= ~temp;
                }
                word = word >> 1;
                c++;
            }
        }
    }
    else {
        printf("ERROR: Invalid write operation for (%s, %d)!\n", XAMModeStr[mode], index);
        exit(0);
    }
}

// Read a block
void XAMIOSet::read(XAMMode mode, AddrType index, DataType *output, DataType *mask, DataType *key)
{
    int c = 0;
    DataType temp;
    for(int i = 0; i < numSubs; ++i) {
        temp = 1;
        output[i] = 0;
        for(int j = 0; j < numRows; ++j) {
            if(mode == SEARCH) {
                //printf("%d:\t%016llx\t%016llx\t%016llx\n", c, content[c], mask[i], key[i]);
                if((mask[i] !=  0) && (content[c] & mask[i]) == (key[i] & mask[i])) {
                    output[i] |= temp;
                }
            }
            else if((mode == INDXED) && (index < numRows)) {
                if(content[c] & (1LL<<index)) {
                    output[i] |= temp;
                }
            }
            else {
                printf("ERROR: Invalid write operation for (%s, %d)!\n", XAMModeStr[mode], index);
                exit(0);
            }
            temp = temp << 1;
            c++;
        }
    }
}


// superset constructor
XAMIOSuper::XAMIOSuper(const char *name, AddrType numSets, AddrType numRows, AddrType numCols)
  : mode(UNINIT)
  , maskPending(true)
  , keyPending(true)
{
  this->numSets = numSets;

  if(numRows>>3 != sizeof(DataType)) {
      printf("ERROR: numRows must be %d for now!\n", sizeof(DataType)<<3);
      exit(0);
  }

  keyBlock  = (DataType *)malloc(numCols>>3);
  maskBlock = (DataType *)malloc(numCols>>3);

  // internal organization
  sets = *(new std::vector<XAMIOSet *>(numSets));
  for(int s = 0; s < numSets; ++s) {
    char temp[256]; sprintf(temp, "%s:%d", name, s);
    sets[s] = new XAMIOSet(temp, numRows, numCols);
  }
}

// Activate a row
void XAMIOSuper::activate()
{
    mode = (mode == ROWINP)? COLINP: ROWINP;
    //printf("%s\n", XAMModeStr[mode]);
}

// Write a block
void XAMIOSuper::write(XAMMode bankMode, AddrType setID, AddrType index, DataType *data)
{
    if((bankMode == SEARCH) && (mode == ROWINP)) {
        if(index == 0) {
            keyPending = false;
            //printf("keyBlock: ");
            for(int i = 0; i < 8; ++i) {
                keyBlock[i] = data[i];
                //printf("%016llx ", keyBlock[i]);
            }
            //printf("\n");
        }
        else {
            maskPending = false;
            //printf("keyBlock: ");
            for(int i = 0; i < 8; ++i) {
                maskBlock[i] = data[i];
                //printf("%016llx ", maskBlock[i]);
            }
            //printf("\n");
        }
    }
    else {
        sets[setID]->write(mode, index, data);
    }
}

// Read a block
void XAMIOSuper::read(XAMMode mode, AddrType setID, AddrType index, DataType *output)
{
    sets[setID]->read(mode, index, output, maskBlock, keyBlock);
}


// bank constructor
XAMIOBank::XAMIOBank(const char *name, AddrType numSupers, AddrType numSets, AddrType numRows, AddrType numCols)
  : mode(UNINIT)
  , lastPrepare(0)
  , lastActivate(0)
  , lastWrite(0)
  , lastRead(0)
{
  this->numSupers = numSupers;

  // internal organization
  supers = *(new std::vector<XAMIOSuper *>(numSupers));
  for(int s = 0; s < numSupers; ++s) {
    char temp[256]; sprintf(temp, "%s:%d", name, s);
    supers[s] = new XAMIOSuper(temp, numSets, numRows, numCols);
  }
}

// Prepare a bank
void XAMIOBank::prepare()
{
    lastPrepare = globalClock;
    mode = (mode == INDXED)? SEARCH: INDXED;
    //printf("%s\n", XAMModeStr[mode]);
}

// Activate a row
void XAMIOBank::activate(AddrType superID)
{
    lastActivate = globalClock;
    supers[superID]->activate();
}

// Write a block
void XAMIOBank::write(AddrType superID, AddrType setID, AddrType index, DataType *data)
{
    lastWrite = globalClock;
    supers[superID]->write(mode, setID, index, data);
}

// Read a block
void XAMIOBank::read(AddrType superID, AddrType setID, AddrType index, DataType *output)
{
    lastRead = globalClock;
    supers[superID]->read(mode, setID, index, output);
}


// XAMIO Vault Controller
XAMIOVault::XAMIOVault(const char *section, const char *name, XCtrlMode mode, AddrType numBanks, AddrType numSupers, AddrType numSets, AddrType numRows, AddrType numCols)
  : lastActivate(0)
  , lastActive_1(0)
  , lastActive_2(0)
  , lastWrite(0)
  , lastRead(0)
  , numPrepares("%s:Prepares", name)
  , numActivates("%s:Activates", name)
{
  this->mode = mode;
  this->numBanks = numBanks;

  // check user defined parameters
  bool pass = true;
  pass = pass && SescConf->isInt(section, "schQueueSize");
  pass = pass && SescConf->isInt(section, "tCCD");
  pass = pass && SescConf->isInt(section, "tWTR");
  pass = pass && SescConf->isInt(section, "tRTP");
  pass = pass && SescConf->isInt(section, "tRRD");
  pass = pass && SescConf->isInt(section, "tRAS");
  pass = pass && SescConf->isInt(section, "tRCD");
  pass = pass && SescConf->isInt(section, "tFAW");
  pass = pass && SescConf->isInt(section, "tOST");
  pass = pass && SescConf->isInt(section, "tCAS");
  pass = pass && SescConf->isInt(section, "tCWD");
  pass = pass && SescConf->isInt(section, "tWR");
  pass = pass && SescConf->isInt(section, "tRP");
  pass = pass && SescConf->isInt(section, "tRC");
  pass = pass && SescConf->isInt(section, "tBL");
  if(!pass) { exit(0); }

  // read in user-defined parameters
  schQueueSize = SescConf->getInt(section, "schQueueSize");
  tCCD         = SescConf->getInt(section, "tCCD");
  tWTR         = SescConf->getInt(section, "tWTR");
  tRTP         = SescConf->getInt(section, "tRTP");
  tRRD         = SescConf->getInt(section, "tRRD");
  tRAS         = SescConf->getInt(section, "tRAS");
  tRCD         = SescConf->getInt(section, "tRCD");
  tFAW         = SescConf->getInt(section, "tFAW");
  tOST         = SescConf->getInt(section, "tOST");
  tCAS         = SescConf->getInt(section, "tCAS");
  tCWD         = SescConf->getInt(section, "tCWD");
  tWR          = SescConf->getInt(section, "tWR");
  tRP          = SescConf->getInt(section, "tRP");
  tRC          = SescConf->getInt(section, "tRC");
  tBL          = SescConf->getInt(section, "tBL");
  //printf("schQueueSize: %lx\n", schQueueSize); fflush(stdout);

  // internal organization
  banks = *(new std::vector<XAMIOBank *>(numBanks));
  for(int b = 0; b < numBanks; ++b) {
    char temp[256]; sprintf(temp, "%s:%d", name, b);
    banks[b] = new XAMIOBank(temp, numSupers, numSets, numRows, numCols);
  }
}

// adding memory reference to XAMIO vault
bool XAMIOVault::isFull()
{
    //printf("SchQueue Size in isFull %zu\n", schQueue.size());
    //return (schQueue.size() >= schQueueSize);
    return false;
}

// Can a prepare to given bank issue now ?
bool XAMIOVault::canIssuePrepare(AddrType bankID)
{
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

// Can an activate to given bank/superset issue now ?
bool XAMIOVault::canIssueActivate(AddrType bankID)
{
    // Enforce tFAW
    if(globalClock < lastActive_2 + tFAW) {
        return false;
    }

    // Enforce tRP
    if(globalClock < banks[bankID]->getLastPrepare() + tRP) {
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

// Can a write to given bank/superset issue now ?
bool XAMIOVault::canIssueWrite(AddrType bankID)
{
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

// Can a read to given bank/superset issue now ?
bool XAMIOVault::canIssueRead(AddrType bankID)
{
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

// Prepare a bank
void XAMIOVault::prepare(AddrType bankID)
{
    numPrepares.inc();
    banks[bankID]->prepare();
}

// Activate a row
void XAMIOVault::activate(AddrType bankID, AddrType superID)
{
    // Timing
  numActivates.inc();
    lastActive_2 = lastActive_1;
    lastActive_1 = lastActivate;
    lastActivate = globalClock;

    banks[bankID]->activate(superID);
}

// Write a block
void XAMIOVault::write(AddrType bankID, AddrType superID, AddrType setID, AddrType index, DataType *data)
{
    lastWrite = globalClock;
    banks[bankID]->write(superID, setID, index, data);
}

// Read a block
void XAMIOVault::read(AddrType bankID, AddrType superID, AddrType setID, AddrType index, DataType *output)
{
    lastRead = globalClock;
    banks[bankID]->read(superID, setID, index, output);
}


// Performing Data Burst
void XAMIOVault::performBurst()
{
    int index;
    XAMIOReference *mref = NULL;

    // Find the oldest ready to complete burst
    for(int i=0; (i < schQueue.size()) && (mref == NULL); ++i) {
        if(schQueue[i]->needsBurst()) {
            //printf("** %ld > (%ld + %ld)) + %ld\n", globalClock, lastWrite, tCWD, tBL); //schQueue[i]->printLog();
            if(globalClock > ((schQueue[i]->getBurstType() == XAMOUT)? (lastRead + tCAS): (lastWrite + tCWD)) + tBL) {
                mref = schQueue[i];
                index = i;
            }
        }
    }

    // Complete data burst
    if(mref != NULL) {
        mref->addLog("burst");
        //if(mref->getBurstType() == UPDATE) {
        //  tagBuffer[mref->getRankID()][mref->getBankID()].clear();
        //  tagBufferUpdated = true;
        //}
        //else if(mref->getBurstType() == LOAD) {
        //  tagBuffer[mref->getRankID()][mref->getBankID()].setTagRow(mref->getTagRow(), mref->getOpenRowID());
        //  tagBufferUpdated = true;
        //  mref->setState((mref->getState() == READTAG)? DOCHECK: FINDTAG);
        //  mref->addLog("tagBuffer[%ld][%ld]: rowID <- %ld", mref->getRankID(), mref->getBankID(), mref->getOpenRowID());
        //}
        //else {
        //  if(mref->getState() == WRITEBK) {
        //    AddrType waddr = mref->getMAddr() >> (blkSizeLog2 + grnSizeLog2);
        //    waddr <<= (blkSizeLog2 + grnSizeLog2);
        //    do {
        //        WideIOWriteBack wb;
        //        wb.maddr = waddr;
        //        wb.mdata = NULL; //mref->getMReq()->getData();
        //        WideIOWriteBackQ.push(wb);
        //        mref->addLog("writeback 0x%lx", waddr);
        //        waddr += blkSize;
        //    }
        //    while(!mref->incWbkCount());
        //    mref->setState(INSTALL);
        //  }
        //  if(!mref->isSentUp()) {
        //    mref->sendUp();
        //  }
        if(!mref->needsRead() && !mref->needsWrite()) {
            XAMIOCompleteQ.push(mref);
            schQueue.erase(schQueue.begin() + index);
        }
        //}
        mref->setBurstPending(false, UNINIT);
    }
}


// Send a clock signal to the XAMIO vault controller
void XAMIOVault::clock()
{
/*
    DataType block[8] = {0x0123456789abcdefLL, 0x0123456789abcdefLL, 0x0123456789abcdefLL, 0x0123456789abcdefLL, 0x0123456789abcdefLL, 0x0123456789abcdefLL, 0x0123456789abcdefLL, 0x0123456789abcdefLL};
    DataType key[8] = {0x0123456789abcdefLL, 0x0123456789abcdefLL, 0x0123456789abcdefLL, 0x0123456789abcdefLL, 0x0123456789abcdefLL, 0x0123456789abcdefLL, 0x0123456789abcdefLL, 0x0123456789abcdefLL};
    DataType mask[8] = {0xffffffffffffffffLL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL, 0x0000000000000000LL};
    DataType output[8];
    prepare(0);
    prepare(0);
    activate(0, 0);
    activate(0, 0);
    write(0, 0, 0, 19, block);
    write(0, 0, 0, 20, block);
    write(0, 0, 0, 21, block);
    write(0, 0, 0, 22, block);
    activate(0, 0);
    write(0, 0, 0, 0, key);
    write(0, 0, 0, 1, mask);
    prepare(0);
    for(int i=0; i<64; ++i) {
      read(0, 0, 0, i, output);
      printf("%3d: ", i); for(int i = 0; i < 8; ++i) { printf("%016llx ", output[i]); } printf("\n");
    }
    prepare(0);
    activate(0, 0);
    read(0, 0, 0, 0, output);
    for(int i = 0; i < 8; ++i) { printf("%016llx ", output[i]); } printf("\n");
    //prepare(0);
    //activate(0, 0);
    exit(0);
*/
    switch(mode) {
    case CACHE:
        updateCACHEQueue();
        scheduleCACHE();
        break;

    case SPRAM:
        performBurst();
        updateSPRAMQueue();
        scheduleSPRAM();
        break;

    case SPCAM:
        performBurst();
        updateSPCAMQueue();
        scheduleSPCAM();
        break;

    default:
        printf("ERROR: Invalid vault mode (%x)!\n", mode);
        exit(0);
    }

    //// Process the entries of retQueue
    //receiveReturned();
    //
    //// replace tag in the tag buffer
    //performReplace();
    //
    //// Complete a pending tag check if needed
    //performTagCheck();
    //
    //// Complete a pending data burst if needed
    //performBurst();
    //
    //// Update queue and power states
    //updateQueueState();
    //updatePowerState();
    //
    //// Include installs in the schedule
    //scheduleInstall();
    //
    //// Schedule a XAMIO command if needed
    //switch(schAlgorithm) {
    //  case 0: // FCFS
    //      scheduleFCFS();
    //      break;
    //  case 1: // FRFCFS
    //      scheduleFRFCFS();
    //      break;
    //  case 2: // TEST
    //      scheduleTEST();
    //      break;
    //  default:
    //      printf("ERROR: Illegal ID for XAMIO command scheduler!\n");
    //      exit(0);
    //}
}

// adding memory reference to XAMIO vault
void XAMIOVault::addReference(XAMIOReference *mref)
{
    // one time setting for reference
    //printf("Enters addReference %d\n", mode); fflush(stdout);
    switch(mode) {
    case CACHE:
        //printf("CACHE ADD REFERENCE");
        break;

    case SPRAM:
        //printf("SPRAM ADD REFERENCE BEFORE\n"); fflush(stdout);
        mref->setReadPending(mref->isRead());
        mref->setWritePending(!mref->isRead());
        mref->setBurstPending(false, UNINIT);
        //printf("SPRAM ADD REFERENCE\n");fflush(stdout);
        break;

    case SPCAM:
        if(mref->isKeyReset() || mref->isMaskReset()) {
            mref->setReadPending(false);
            mref->setWritePending(false);
            //printf("SPCAM ADD REFERENCE 1\n"); fflush(stdout);
        }
        else {
            //printf("SPCAM ADD REFERENCE 2 1\n"); fflush(stdout);
            mref->setReadPending(mref->isRead());
            mref->setWritePending(!mref->isRead());
            //printf("SPCAM ADD REFERENCE 2 2\n"); fflush(stdout);
        }
        mref->setBurstPending(false, UNINIT);
        break;
    }

    // add the memory reference to the scheduling queue
    //printf("Pushing Back into SchQueue\n"); fflush(stdout);
    schQueue.push_back(mref);
}


// update state of the scheduling queue in cache mode
void XAMIOVault::updateCACHEQueue()
{
}


// update state of the scheduling queue in scratchpad RAM mode
void XAMIOVault::updateSPRAMQueue()
{
    for(int i=0; i < schQueue.size(); ++i) {
        XAMIOReference *mref = schQueue[i];
        AddrType bankID      = mref->getBankID();
        AddrType superID     = mref->getSuperID();
        XAMMode bankMode     = getBankMode(bankID);
        XAMMode superMode    = getSuperMode(bankID, superID);

        mref->setPreparePending(bankMode != INDXED);
        mref->setActivatePending(superMode != ROWINP);
    }
}


// update state of the scheduling queue in scratchpad CAM mode
void XAMIOVault::updateSPCAMQueue()
{
    for(int i=0; i < schQueue.size(); ++i) {
        XAMIOReference *mref = schQueue[i];
        AddrType bankID      = mref->getBankID();
        AddrType superID     = mref->getSuperID();
        XAMMode bankMode     = getBankMode(bankID);
        XAMMode superMode    = getSuperMode(bankID, superID);

        mref->setPreparePending(bankMode != (mref->isSearch()? SEARCH: INDXED));
        if(mref->isSearch() && (needsKey(bankID, superID) || needsMask(bankID, superID))) {
            mref->setKeyPending(needsKey(bankID, superID));
            mref->setMaskPending(needsMask(bankID, superID));
            mref->setActivatePending(superMode != ROWINP);
        }
        else {
            mref->setKeyPending(false);
            mref->setMaskPending(false);
            mref->setActivatePending(superMode != (mref->isColumnWrite()? COLINP: ROWINP));
        }
    }
}


// scheduling a cache transaction
void XAMIOVault::scheduleCACHE()
{
    if(schQueue.empty()) {
        return;
    }

    XAMIOReference *mref = schQueue[0];
}


// scheduling a scratchpad RAM transaction
void XAMIOVault::scheduleSPRAM()
{
    if(schQueue.empty()) {
        return;
    }

    XAMIOReference *mref = schQueue[0];

    if(mref->needsPrepare()) {
        if(canIssuePrepare(mref->getBankID())) {
            prepare(mref->getBankID());
            mref->addLog("prepare %ld", mref->getBankID());
        }
        return;
    }

    if(mref->needsActivate()) {
        if(canIssueActivate(mref->getBankID())) {
            activate(mref->getBankID(), mref->getSuperID());
            mref->addLog("activate %ld/%ld", mref->getBankID(), mref->getSuperID());
        }
        return;
    }

    if(mref->needsRead()) {
        if(canIssueRead(mref->getBankID())) {
            DataType block[8];
            read(mref->getBankID(), mref->getSuperID(), mref->getSetID(), mref->getRowID(), block);
            mref->setReadPending(false);
            mref->setBurstPending(true, XAMOUT);
            mref->addLog("read %ld/%ld/%ld/%ld/data", mref->getBankID(), mref->getSuperID(), mref->getSetID(), mref->getRowID());
        }
        return;
    }

    if(mref->needsWrite()) {
        if(canIssueWrite(mref->getBankID())) {
            DataType block[8];
            write(mref->getBankID(), mref->getSuperID(), mref->getSetID(), mref->getRowID(), block);
            mref->setWritePending(false);
            mref->setBurstPending(true, XAMINP);
            mref->addLog("write %ld/%ld/%ld/%ld/data", mref->getBankID(), mref->getSuperID(), mref->getSetID(), mref->getRowID());
        }
        return;
    }
}


// scheduling a scratchpad CAM transaction
void XAMIOVault::scheduleSPCAM()
{
    if(schQueue.empty()) {
        return;
    }

    XAMIOReference *mref = schQueue[0];

    if(mref->isKeyReset()) {
        resetKey();
        mref->addLog("reset key");
        XAMIOCompleteQ.push(mref);
        schQueue.erase(schQueue.begin());
        return;
    }

    if(mref->isMaskReset()) {
        resetMask();
        mref->addLog("reset mask");
        XAMIOCompleteQ.push(mref);
        schQueue.erase(schQueue.begin());
        return;
    }

    if(mref->needsPrepare()) {
        if(canIssuePrepare(mref->getBankID())) {
            prepare(mref->getBankID());
            mref->addLog("prepare %ld", mref->getBankID());
        }
        return;
    }

    if(mref->needsActivate()) {
        if(canIssueActivate(mref->getBankID())) {
            activate(mref->getBankID(), mref->getSuperID());
            mref->addLog("activate %ld/%ld", mref->getBankID(), mref->getSuperID());
        }
        return;
    }

    if(mref->needsKey()) {
        if(canIssueWrite(mref->getBankID())) {
            DataType key[8];
            write(mref->getBankID(), mref->getSuperID(), mref->getSetID(), 0, key);
            mref->setBurstPending(true, XAMINP);
            mref->addLog("write %ld/%ld/%ld/%ld/key", mref->getBankID(), mref->getSuperID(), mref->getSetID(), mref->getRowID());
        }
        return;
    }

    if(mref->needsMask()) {
        if(canIssueWrite(mref->getBankID())) {
            DataType mask[8];
            write(mref->getBankID(), mref->getSuperID(), mref->getSetID(), 1, mask);
            mref->setBurstPending(true, XAMINP);
            mref->addLog("write %ld/%ld/%ld/%ld/mask", mref->getBankID(), mref->getSuperID(), mref->getSetID(), mref->getRowID());
        }
        return;
    }

    if(mref->needsRead()) {
        if(canIssueRead(mref->getBankID())) {
            DataType block[8];
            read(mref->getBankID(), mref->getSuperID(), mref->getSetID(), mref->getRowID(), block);
            if(mref->isSearch() || (mref->decReadCount() == 0)) {
                mref->setReadPending(false);
            }
            mref->setBurstPending(true, XAMOUT);
            mref->addLog("read %ld/%ld/%ld/%ld/data", mref->getBankID(), mref->getSuperID(), mref->getSetID(), mref->getRowID());
        }
        return;
    }

    if(mref->needsWrite()) {
        if(canIssueWrite(mref->getBankID())) {
            DataType block[8];
            write(mref->getBankID(), mref->getSuperID(), mref->getSetID(), mref->getRowID(), block);
            mref->setWritePending(false);
            mref->setBurstPending(true, XAMINP);
            mref->addLog("write %ld/%ld/%ld/%ld/data", mref->getBankID(), mref->getSuperID(), mref->getSetID(), mref->getRowID());
        }
        return;
    }
}



// XAMIO Interface
XAMIO::XAMIO(MemorySystem* current, const char *section, const char *name)
  /* constructor {{{1 */
  : MemObj(section, name)
  , clockScheduled(false)
  , countReceived("%s:received", name)
  , countServiced("%s:serviced", name)
  , countSentLower("%s:sentlower", name)
  , countWriteBack("%s:writeback", name)
  , countSkipped("%s:skipped", name)
  , avgAccessTime("%s:avgAccessTime", name)
  , tracAccessTime("%s:tracAccessTime", name)
  , freqBlockUsage("%s:freqBlockUsage", name)
{
  MemObj *lower_level = NULL;

  // check config parameters
  bool pass = true;
  pass = pass && SescConf->isPower2(section, "memSize", 0);
  pass = pass && SescConf->isPower2(section, "numCVaults", 0);
  pass = pass && SescConf->isPower2(section, "numRVaults", 0);
  pass = pass && SescConf->isPower2(section, "numSVaults", 0);
  pass = pass && SescConf->isPower2(section, "numBanks", 0);
  pass = pass && SescConf->isPower2(section, "numSupers", 0);
  pass = pass && SescConf->isPower2(section, "numSets", 0);
  pass = pass && SescConf->isPower2(section, "numRows", 0);
  pass = pass && SescConf->isPower2(section, "rowSize", 0);
  pass = pass && SescConf->isPower2(section, "tagSize", 0);
  if(!pass) { exit(0); }

  // read in config parameters
  memSize      = SescConf->getInt(section, "memSize") * 1024LL * 1024LL; // Memory Size (MB)
  numCVaults   = SescConf->getInt(section, "numCVaults");
  numRVaults   = SescConf->getInt(section, "numRVaults");
  numSVaults   = SescConf->getInt(section, "numSVaults");
  numBanks     = SescConf->getInt(section, "numBanks");
  numSupers    = SescConf->getInt(section, "numSupers");
  numSets      = SescConf->getInt(section, "numSets");
  numRows      = SescConf->getInt(section, "numRows");
  rowSize      = SescConf->getInt(section, "rowSize");
  tagSize      = SescConf->getInt(section, "tagSize");
  numTags      = numRows/tagSize;
  numVaults    = numCVaults + numRVaults + numSVaults;
  xamSize      = numVaults*numBanks*numSupers*numSets*numRows*rowSize/8;
  numTBanks    = numBanks/((numRows/tagSize)*(rowSize/numRows));
  numDBanks    = numBanks - numTBanks;

  // intermediate parameters
  memSizeLog2      = log2(memSize);
  xamSizeLog2      = log2(xamSize);
  numCVaultsLog2   = log2(numCVaults);
  numRVaultsLog2   = log2(numRVaults);
  numSVaultsLog2   = log2(numSVaults);
  numBanksLog2     = log2(numBanks);
  numSupersLog2    = log2(numSupers);
  numSetsLog2      = log2(numSets);
  numRowsLog2      = log2(numRows);
  rowSizeLog2      = log2(rowSize);
  numTagsLog2      = log2(numTags);
  numTBanksLog2    = log2(numTBanks);

  // internal organization
  vaults = *(new std::vector<XAMIOVault *>(numVaults));
  for(int v = 0; v < numVaults; ++v) {
    char temp[256]; sprintf(temp, "%s:%d", name, v);
    if(v < numCVaults) {
      vaults[v] = new XAMIOVault(section, temp, CACHE, numBanks, numSupers, numSets, numRows, rowSize);
    }
    else if(v < (numCVaults + numRVaults)) {
      vaults[v] = new XAMIOVault(section, temp, SPRAM, numBanks, numSupers, numSets, numRows, rowSize);
    }
    else {
      vaults[v] = new XAMIOVault(section, temp, SPCAM, numBanks, numSupers, numSets, numRows, rowSize);
    }
  }

  freqBlockUsage.setPFName(SescConf->getCharPtr("", "reportFile",0));
  freqBlockUsage.setPlotLabels("Block Usage", "Frequency");
  freqBlockUsage.setBucketSize(64);
  // display conf
  printf("XAMIO [Size:%ld, Vaults:%ld/%ld/%ld, Banks:%ld, Supersets:%ld, Sets:%ld, Rows:%ld, Columns:%ld]\n", xamSize, numCVaults, numRVaults, numSVaults, numBanks, numSupers, numSets, numRows, rowSize);

  // set the lower level
  lower_level = current->declareMemoryObj(section, "lowerLevel");
  if(lower_level) {
    addLowerLevel(lower_level);
  }
}
/* }}} */

void XAMIO::doReq(MemRequest *mreq)
  /* request reaches the memory controller {{{1 */
{
  if(mreq->getAction() == ma_setDirty) {
    addRequest(mreq, false);
  }
  else {
    addRequest(mreq, true);
  }
}
/* }}} */

void XAMIO::doReqAck(MemRequest *lreq)
  /* push up {{{1 */
{
  XAMIOReference *mref = (XAMIOReference *) lreq->getMRef();
  if((mref != NULL)) {
      mref->addLog("ack with %p", lreq);
      mref->sendUp();
      pendingList.remove(mref);
      delete mref;
    }
}
/* }}} */

void XAMIO::doDisp(MemRequest *mreq)
  /* push down {{{1 */
{
  //MSG("\nXAMIO::doDisp for Addr %llx", mreq->getAddr());
  if(mreq->getAction() == ma_setDirty) {
  	  addRequest(mreq, false);
  }
  else {
      mreq->ack();
  }
}
/* }}} */

void XAMIO::doSetState(MemRequest *mreq)
  /* push up {{{1 */
{
  MSG("\nXAMIO::doSetState for Addr %llx", mreq->getAddr());
  I(0);
}
/* }}} */

void XAMIO::doSetStateAck(MemRequest *mreq)
  /* push up {{{1 */
{
  MSG("\nXAMIO::doSetStateAck for Addr %llx", mreq->getAddr());
//  I(0);
}
/* }}} */

bool XAMIO::isBusy(AddrType addr) const
/* always can accept writes {{{1 */
{
  return false;
}
/* }}} */

void XAMIO::tryPrefetch(AddrType addr, bool doStats)
  /* try to prefetch to openpage {{{1 */
{
  MSG("\nXAMIO::tryPrefetch for Addr %llx", addr);
  // FIXME:
}
/* }}} */

TimeDelta_t XAMIO::ffread(AddrType addr)
  /* fast forward reads {{{1 */
{
  MSG("\nXAMIO::ffread for Addr %llx", addr);
  return 0; //delay + RowAccessLatency;
}
/* }}} */

TimeDelta_t XAMIO::ffwrite(AddrType addr)
  /* fast forward writes {{{1 */
{
  MSG("\nXAMIO::ffwrite for Addr %llx", addr);
  return 0; //delay + RowAccessLatency;
}
/* }}} */


// adding memory requests
void XAMIO::addRequest(MemRequest *mreq, bool read)
{
  bool skip = false;
  //if(mreq->getSMode()) printf("%s\t%llx\t%lx\n", read? "READ": "WRITE", mreq->getAddr(), mreq->getSMode());
  AddrType smode = mreq->getSMode();
  AddrType maddr = mreq->getAddr() & (memSize-1);
  AddrType vaultID, bankID, superID, setID, rowID, colID, tagID, addrTag;

  if(smode & SIG_CAMPAD) {
      if(numSVaults == 0) {
          printf("ERROR: No scratchpad CAM defined for the system!\n");
          exit(0);
      }
      if(!read)
      {
        /*std::map<unsigned long int, int>::iterator itr = EnduranceMap.find(maddr);
        if(itr == EnduranceMap.end())
        {
          EnduranceMap.insert(std::pair<unsigned long int, int>(maddr, 1));
        }
        else
        {
          itr->second++;
        }*/
        freqBlockUsage.sample(true, maddr);
      }
      AddrType numBytes = numRows >> 3;
      AddrType numBytesLog2 = numRowsLog2 - 3;
      // compute SP-CAM fields [vaultID:bankID:superID:setID:colID:rowID]
      rowID   = (maddr & (numBytes - 1));
      colID   = (maddr >> numBytesLog2) & (rowSize - 1);
      setID   = (maddr >> (numBytesLog2 + rowSizeLog2)) & (numSets - 1);
      superID = (maddr >> (numBytesLog2 + rowSizeLog2 + numSetsLog2)) & (numSupers - 1);
      bankID  = (maddr >> (numBytesLog2 + rowSizeLog2 + numSetsLog2 + numSupersLog2)) & (numBanks - 1);
      vaultID = (maddr >> (numBytesLog2 + rowSizeLog2 + numSetsLog2 + numSupersLog2 + numBanksLog2)) & (numSVaults - 1);
      vaultID = vaultID + numCVaults + numRVaults;
      //printf("CAM Scratchpad\t%lx\t%02lx:%02lx:%02lx:%02lx:%02lx:%02lx\n", maddr, vaultID, bankID, superID, setID, colID, rowID);
  }
  else if(smode & SIG_RAMPAD) {
      if(numRVaults == 0) {
          printf("ERROR: No scratchpad RAM defined for the system!\n");
          exit(0);
      }
      if(!read)
      {
        /*std::map<unsigned long int, int>::iterator itr = EnduranceMap.find(maddr);
        if(itr == EnduranceMap.end())
        {
          EnduranceMap.insert(std::pair<unsigned long int, int>(maddr, 1));
        }
        else
        {
          itr->second++;
        }*/
        freqBlockUsage.sample(true, maddr);
      }
      AddrType numBytes = rowSize >> 3;
      AddrType numBytesLog2 = rowSizeLog2 - 3;
      // compute SP-RAM fields [vaultID:bankID:superID:setID:rowID:colID]
      colID   = (maddr & (numBytes - 1));
      rowID   = (maddr >> numBytesLog2) & (numRows - 1);
      setID   = (maddr >> (numBytesLog2 + numRowsLog2)) & (numSets - 1);
      superID = (maddr >> (numBytesLog2 + numRowsLog2 + numSetsLog2)) & (numSupers - 1);
      bankID  = (maddr >> (numBytesLog2 + numRowsLog2 + numSetsLog2 + numSupersLog2)) & (numBanks - 1);
      vaultID = (maddr >> (numBytesLog2 + numRowsLog2 + numSetsLog2 + numSupersLog2 + numBanksLog2)) & (numRVaults - 1);
      vaultID = vaultID + numCVaults;
      //printf("RAM Scratchpad\t%lx\t%02lx:%02lx:%02lx:%02lx:%02lx:%02lx\n", maddr, vaultID, bankID, superID, setID, rowID, colID);
  }
  else {
      skip = (numCVaults == 0);
      // compute compute cache fields [addrTag:bankID:vaultID:superID:setID:tagID]
      tagID   = (maddr & (numTags - 1));
      setID   = (maddr >> numTagsLog2) & (numSets - 1);
      superID = (maddr >> (numTagsLog2 + numSetsLog2)) & (numSupers - 1);
      vaultID = (maddr >> (numTagsLog2 + numSetsLog2 + numSupersLog2)) & (numCVaults - 1);
      bankID  = (maddr >> (numTagsLog2 + numSetsLog2 + numSupersLog2 + numCVaultsLog2)) & (numTBanks - 1);
      bankID  = bankID + numDBanks;
      addrTag = (maddr >> (numTagsLog2 + numSetsLog2 + numSupersLog2 + numCVaultsLog2 + numTBanksLog2));
      //printf("CACHE\t%lx\t%02lx:%02lx:%02lx:%02lx:%02lx:%02lx\n", maddr, vaultID, bankID, superID, setID, rowID, colID);
  }

  if((smode & SIG_CAMPAD) && (smode & (SIG_KEYMEM | SIG_MSKMEM))) {
      for(int v=numCVaults+numRVaults; v < numVaults; ++v) {
          XAMIOReference *mref = new XAMIOReference();
          mref->setMReq((v==numCVaults+numRVaults)? mreq: NULL);
          mref->setSMode(smode);
          mref->setMAddr(maddr);
          mref->setVaultID(v);
          mref->setBankID(0);
          mref->setSuperID(0);
          mref->setSetID(0);
          mref->setRowID(0);
          mref->setColID(0);
          mref->setTagID(0);
          mref->setAddrTag(0);
          mref->setRead(read);
          mref->setSearch(false);
          mref->setKeyReset(smode & SIG_KEYMEM);
          mref->setMaskReset(smode & SIG_MSKMEM);
          mref->setReadCount(0);
          mref->setColumnWrite(false);
          mref->addLog("received");
          receivedQueue.push(mref);
          countReceived.inc();
      }
  }
  else {
      XAMIOReference *mref = new XAMIOReference();
      mref->setMReq(mreq);
      mref->setSMode(smode);
      mref->setMAddr(maddr);
      mref->setVaultID(vaultID);
      mref->setBankID(bankID);
      mref->setSuperID(superID);
      mref->setSetID(setID);
      mref->setRowID(rowID);
      mref->setColID(colID);
      mref->setTagID(tagID);
      mref->setAddrTag(addrTag);
      mref->setRead(read);
      mref->setSearch(smode & SIG_SEARCH);
      mref->setKeyReset(false);
      mref->setMaskReset(false);
      mref->setReadCount(numRows);
      mref->setColumnWrite(smode & SIG_COLUMN);
      mref->addLog("received");
      receivedQueue.push(mref);
      countReceived.inc();

      if(skip) {
        XAMIOSkippedQ.push(mref);
        receivedQueue.pop();
      }
  }

  // schedule a clock edge
  if(!clockScheduled) {
      clockScheduled = true;
      CallbackBase *cb = ManageXAMIOCB::create(this);
      EventScheduler::schedule((TimeDelta_t) 1, cb);
  }
}


// XAMIO cycle by cycle management
void XAMIO::manageXAMIO(void)
{
  finishXAMIO();
  completeMRef();
  doWriteBacks();
  doFetchBlock();
  doSkipBlocks();
  dispatchRefs();

  for(int v = 0; v < numVaults; ++v) {
    vaults[v]->clock();
  }

  // sample trackers
  if((globalClock & 0x3FFFF) == 0) {
  }

  // schedule a clock edge
  clockScheduled = false;
  if(countServiced.getDouble() < countReceived.getDouble()) {
    clockScheduled = true;
    CallbackBase *cb = ManageXAMIOCB::create(this);
    EventScheduler::schedule((TimeDelta_t) 1, cb);
  }
}


// process skipped references
void XAMIO::doSkipBlocks(void)
{
  for(int i=0; i < XAMIOSkippedQ.size(); ++i) {
    XAMIOReference *mref = XAMIOSkippedQ.front();
    MemRequest *mreq = mref->getMReq();
    mreq->setMRef((void *)mref);
    router->scheduleReq(mreq, 1);
    mref->addLog("descend %p", mreq);
    //mref->setState(SKIPPED);
    pendingList.append(mref);
    XAMIOSkippedQ.pop();
    countSkipped.inc();
  }
}


// complete XAMIO references
void XAMIO::completeMRef(void)
{
  for(int i=0; i < XAMIOCompleteQ.size(); ++i) {
    XAMIOReference *mref = XAMIOCompleteQ.front();
    mref->sendUp();
    //
    //if(mref->getNumMatch()) {
    //    countHit.inc();
    //}
    //else {
    //    countMiss.inc();
    //}
    //countRead.add(mref->getNumRead(), true);
    //countWrite.add(mref->getNumWrite(), true);
    //countLoad.add(mref->getNumLoad(), true);
    //countUpdate.add(mref->getNumUpdate(), true);
    //countEvict.add(mref->getNumEvict(), true);
    //countInstall.add(mref->getNumInstall(), true);
    ////if(mref->isCanceled()) {
    ////    mref->printState();
    ////    mref->printLog();
    ////    exit(0);
    ////}
    //
    delete mref;
    XAMIOCompleteQ.pop();
  }
}


// process write back requests
void XAMIO::doWriteBacks(void)
{
  for(int i=0; i < XAMIOWriteBackQ.size(); ++i) {
    //XAMIOWriteBack wb = XAMIOWriteBackQ.front();
    //router->sendDirtyDisp(wb.maddr, true, 1);
    //XAMIOWriteBackQ.pop();
    //countWriteBack.inc();
  }
}


// process fetch references
void XAMIO::doFetchBlock(void)
{
  for(int i = 0; i < XAMIOFetchedQ.size(); ++i) {
    //XAMIOReference *mref = XAMIOFetchedQ.front();
    //for(long j=0; j < mref->getNumGrain(); ++j) {
    //    MemRequest *temp = MemRequest::createReqRead(this, true, mref->getGrainAddr(j));
    //    temp->setMRef((void *)mref);
    //    router->scheduleReq(temp, 1);
    //    mref->addLog("fetch 0x%llx with %p", mref->getGrainAddr(j), temp);
    //}
    //pendingList.append(mref);
    //XAMIOFetchedQ.pop();
    //countSentLower.inc();
  }
}


// process serviced request
void XAMIO::finishXAMIO(void)
{
    while(XAMIOServicedQ.size()) {
        ServicedRequest sreq = XAMIOServicedQ.front();

        I(sreq.mreq);

        if(sreq.mreq) {
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
        }

        /*std::ofstream endurancefile;
        endurancefile.open("/uusoc/scratch/res/arch/students/ananth/fdot/endurancecheck.txt", std::ofstream::out | std::ofstream::trunc);

        for( std::map<unsigned long int, int>::iterator itr = EnduranceMap.begin(); itr != EnduranceMap.end(); itr ++)
        {
          endurancefile << itr->first << "\t" <<itr->second << "\n";
        }*/

        XAMIOServicedQ.pop();
        countServiced.inc();
    }
}


// dispatch reference
void XAMIO::dispatchRefs(void)
{
  //printf("Enters\n");
    XAMIOReference *mref;
    bool notFull = true;

    while(receivedQueue.size() && notFull) {
        mref = receivedQueue.front();
        /*if(mref->getMReq()->getSMode() & SIG_COLUMN)
        {
        printf("1 Channel ID %d BankID %d superID %d setID %d colID %d rowID %d\n", mref->getVaultID(), mref->getBankID(), mref->getSuperID(), mref->getSetID(), mref->getColID(), mref->getRowID()) ;
        }
        if(mref->getMReq()->getSMode() & SIG_SEARCH)
        {
        printf("2 Channel ID %d BankID %d superID %d setID %d colID %d rowID %d\n", mref->getVaultID(), mref->getBankID(), mref->getSuperID(), mref->getSetID(), mref->getColID(), mref->getRowID()) ;
        }
        if(mref->getMReq()->getSMode() & SIG_RAMPAD)
        {
        printf("3 Channel ID %d BankID %d superID %d setID %d colID %d rowID %d\n", mref->getVaultID(), mref->getBankID(), mref->getSuperID(), mref->getSetID(), mref->getColID(), mref->getRowID()) ;
        }*/
        if(notFull = !vaults[mref->getVaultID()]->isFull()) {
            vaults[mref->getVaultID()]->addReference(mref);
            //printf("POPS\n");
            receivedQueue.pop();
        }
    }
  //printf("Exits");
}
