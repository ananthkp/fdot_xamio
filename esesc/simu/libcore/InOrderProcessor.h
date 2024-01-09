// Contributed by Jose Renau
//
// The ESESC/BSD License
//
// Copyright (c) 2005-2013, Regents of the University of California and 
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

#ifndef INORDERPROCESSOR_H_
#define INORDERPROCESSOR_H_

#include "nanassert.h"

#include "GProcessor.h"
#include "Pipeline.h"
#include "FetchEngine.h"
#include "LSQ.h"

#ifndef SMTFetch_S
#define SMTFetch_S
struct SMTFetch {
  FetchEngine *fe;
  Time_t      smt_lastTime;
  int         smt_cnt;
  int         smt_active;
  int         smt_turn;

  SMTFetch() {
    fe = 0;
    smt_lastTime = 0;
    smt_cnt      = 1;
    smt_active   = 1;
    smt_turn     = 0;
  };

  bool update(bool space) {
    if (smt_lastTime != globalClock) {
      smt_lastTime = globalClock;
      smt_active   = smt_cnt;
      smt_cnt      = 1;
    }else{
      smt_cnt++;
    }
    I(smt_active>0);
    
    smt_turn--;
    if (smt_turn<0 && space) {
      if (smt_cnt == smt_active)
        smt_turn = 0;
      else
        smt_turn = smt_active;
      return true;
    }
    
    return false;
  };
};
#endif
class InOrderProcessor : public GProcessor {
private:
  const int32_t RetireDelay;

  FetchEngine *ifid;
  PipeQueue   pipeQ;
  int32_t     spaceInInstQueue;

  LSQNone     lsq;
  bool        busy;
  bool        lastrob_getStatsFlag;

  SMTFetch *sf;

  //DInst *RAT[LREG_MAX];
  DInst **RAT;

  void fetch(FlowID fid);
protected:
  ClusterManager clusterManager;
  // BEGIN VIRTUAL FUNCTIONS of GProcessor

  bool advance_clock(FlowID fid);
  void retire();

  StallCause addInst(DInst *dinst);
  // END VIRTUAL FUNCTIONS of GProcessor

public:
  InOrderProcessor(GMemorySystem *gm, CPU_t i);
  virtual ~InOrderProcessor();

  void       executing(DInst *dinst);
  void       executed(DInst *dinst);
  LSQ *getLSQ() { return &lsq; }
  void replay(DInst *dinst);
  bool isFlushing() {
    I(0);
    return false;
  }
  bool isReplayRecovering() {
    I(0);
    return false;
  }
    Time_t getReplayID()
  {
    I(0);
    return false;
  }

};


#endif /* INORDERPROCESSOR_H_ */