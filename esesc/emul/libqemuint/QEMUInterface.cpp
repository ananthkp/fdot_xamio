/*
   ESESC: Super ESCalar simulator
   Copyright (C) 2005 University California, Santa Cruz.

   Contributed by Gabriel Southern
                  Jose Renau
                  Sushant Kondguli

This file is part of ESESC.

ESESC is free software; you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation;
either version 2, or (at your option) any later version.

ESESC is    distributed in the  hope that  it will  be  useful, but  WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should  have received a copy of  the GNU General  Public License along with
ESESC; see the file COPYING.  If not, write to the  Free Software Foundation, 59
Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <pthread.h>
#include <strings.h>
#include <string.h>

#include "InstOpcode.h"
#include "Instruction.h"
#include "Snippets.h"

#include "QEMUInterface.h"
#include "QEMUReader.h"
#include "EmuSampler.h"

#include "../../../apps-ananth/esesc/simusignal.h"

EmuSampler *qsamplerlist[128];
//EmuSampler *qsampler = 0;
bool* globalFlowStatus = 0;
typedef struct _Signal {
    uint64_t arg1;  // mode
    uint64_t arg2;  // begin
    uint64_t arg3;  // end
    struct _Signal *next;
} Signal;
Signal *head = NULL;

extern "C" void QEMUReader_goto_sleep(void *env);
extern "C" void QEMUReader_wakeup_from_sleep(void *env);
extern "C" int qemuesesc_main(int argc, char **argv, char **envp);

extern "C" void *qemuesesc_main_bootstrap(void *threadargs) {

  static bool qemuStarted = false;
  if (!qemuStarted) {
    qemuStarted = true;
    QEMUArgs *qdata = (struct QEMUArgs *) threadargs;

    int    qargc = qdata->qargc;
    char **qargv = qdata->qargv;

    MSG("Starting qemu with");
    for(int i = 0; i < qargc; i++)
      MSG("arg[%d] is: %s",i,qargv[i]);

    qemuesesc_main(qargc,qargv,NULL);

    MSG("qemu done");

    QEMUReader_finish(0);

    pthread_exit(0);
  }else{
    MSG("QEMU already started! Ignoring the new start.");
  }

  return 0;
}

extern "C" uint32_t QEMUReader_getFid(FlowID last_fid)
{
  return qsamplerlist[last_fid]->getFid(last_fid);
}

extern "C" uint64_t QEMUReader_get_time() 
{
  return qsamplerlist[0]->getTime();
}

///MnB:~ Memory Data
extern "C" uint64_t QEMUReader_queue_inst(uint64_t pc, uint64_t addr, uint16_t fid, uint16_t op, uint16_t src1, uint16_t src2, uint16_t dest, void *dummy) {
//extern "C" uint64_t QEMUReader_queue_inst(uint64_t pc, uint64_t addr, uint16_t fid, uint16_t op, uint16_t src1, uint16_t src2, uint16_t dest, void *dummy, void *oldData, void *newData) {
///:.
  I(fid<128); // qsampler statically sized to 128 at most

  //I(qsamplerlist[fid]->isActive(fid) || EmuSampler::isTerminated());

  //MSG("pc=%llx addr=%llx op=%d cpu=%d",pc,addr,op,fid);
///MnB:~ Memory Data
  // iLALU_LD(10), iSALU_LL(12), iSALU_ST(11), and iSALU_SC(13)
  if((op == 10) || (op == 12) || (op == 11) || (op == 13)) {
    // search the signal list
    bool found = false;
    Signal *item, *temp = head;
    while((found != true) && (temp != NULL)) {
        found = ((addr >= temp->arg2) && (addr < temp->arg3));
        item = temp;
        temp = temp->next;
    }
    if(found) {
      //printf("QEMUReader_queue_inst in found\t%lx\t%lx\n", addr, item->arg1);
      //printf("%d:\tQEMUReader_queue_inst(%lx, %lx, %x, %hx, %hx, %hx, %hx, %p)\n", op, pc, addr, fid, op, src1, src2, dest, dummy);
      if(item->arg1 & SIG_NOSIMU) {
          return 0;
      }
      //if(item->arg1) printf("QEMUReader_queue_inst %lx\t%lx\n", addr, item->arg1);
      return qsamplerlist[fid]->queue(pc,addr,fid,op,src1, src2, dest, LREG_InvalidOutput, dummy, item->arg1);
    }
  }
  return qsamplerlist[fid]->queue(pc,addr,fid,op,src1, src2, dest, LREG_InvalidOutput, dummy, 0);
  //return qsamplerlist[fid]->queue(pc,addr,fid,op,src1, src2, dest, LREG_InvalidOutput, dummy, oldData, newData);
///:.
}
//ANATH HASHING START
#define getblock(p, i) (p[i])
#define BIG_CONSTANT(x) (x)
#define ROTL32(x,y) rotl32(x,y)

static uint64_t rotl32 ( uint32_t x, int8_t r )
{
  return (x << r) | (x >> (32 - r));
}

static  uint32_t fmix32 ( uint32_t h )
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

void MurmurHash3_x86_32 ( const void * key, int len,
                          uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 4;
  int i;

  uint32_t h1 = seed;

  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;

  //----------
  // body

  const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

  for(i = -nblocks; i; i++)
  {
    uint32_t k1 = getblock(blocks,i);

    k1 *= c1;
    k1 = ROTL32(k1,15);
    k1 *= c2;
    
    h1 ^= k1;
    h1 = ROTL32(h1,13); 
    h1 = h1*5+0xe6546b64;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

  uint32_t k1 = 0;

  switch(len & 3)
  {
  case 3: k1 ^= tail[2] << 16;
  case 2: k1 ^= tail[1] << 8;
  case 1: k1 ^= tail[0];
          k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len;

  h1 = fmix32(h1);

  *(uint32_t*)out = h1;
}
//ANANTH HASHING END
int getnextline(char* output, int max_len, char* file)
{
  int i=0;
  while(i<max_len-1)
  {
    if( file[i] == '\0')
    {
      if(i==0)
        return -1;
      else
        return i;
    }
    if( file[i] == '\r')
      return (i+2);

    if( file[i] == '\n' )
      return (i+1);

    output[i] = file[i];
    i++;
  }
  file+=i;
  return i;
}
//ANANTH SMATCH END
extern "C" uint64_t QEMUReader_signal_simu(uint32_t fid, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{

  if(arg1 & SIG_DOQEMU)
  {
    int i;
    long long ret = 0;
    //printf("VALUE CHECK %lld\n", *((long long *)((void *)arg3)));
    for(i = 0; i < 64; i ++)
    {
      //printf("VALUES %lld\t %lld\n", ((long long *)((void *)arg2))[i], arg3);
      if(((long long *)((void *)arg2))[i] == arg3) {
        ret |= 1LL<<i;
      }
    }
    return ret;
  }


  else if(arg1 & SIG_DOSMAT)
  {
    int i = 0;
    printf("Enters DOSMAT in QEMU\n");
    int key_len = 0;
    int max_rec_length = 1024;
    char* cur_word = (char *)malloc(1024);
    char* key_file = (char *)arg2;
    bzero(cur_word, max_rec_length);
    uint64_t total_blocks = 0;
    while(key_len = getnextline(cur_word, max_rec_length, key_file)>=0)
    //for(i = 0; i < 10; i ++)
    {
      key_len = getnextline(cur_word, max_rec_length, key_file);
      //printf("Word is %s\n", cur_word);
      total_blocks = total_blocks + (key_len/8 + 1);
      //printf("Before key_file %p\n", key_file);
      key_file  = key_file + key_len;
      //printf("After key_file %p\n", key_file);
      bzero(cur_word, max_rec_length);
    }
    return total_blocks;
  }
  else if(arg1 & SIG_DOHASH)
  {
    int * key_hash = (int *) &arg2;

    size_t idx;
    //size_t i;
    size_t sz;

    uint32_t *h = (uint32_t *)malloc(sizeof(uint32_t));
    sz = 1ULL << arg3;

    MurmurHash3_x86_32(key_hash, 4, 2, h);
    idx = *h & (sz - 1);

    printf("RETURNS IN DO_HASH\n");
    return idx;
  }


  // search the signal list
  bool found = false;
  Signal *item, *temp = head;
  while((found != true) && (temp != NULL)) {
      found = (arg2 == temp->arg2);
      item = temp;
      temp = temp->next;
  }

  // update the signal list
  if(found) {
    MSG("UPDATE\tQEMUReader_signal_simu(%d, %lx, %lx, %lx)", fid, arg1, arg2, arg3);
    item->arg1 = arg1;
  }
  else {
    MSG("NEW\tQEMUReader_signal_simu(%d, %lx, %lx, %lx)", fid, arg1, arg2, arg3);
    temp = (Signal *)malloc(sizeof(Signal));
    temp->arg1 = arg1;
    temp->arg2 = arg2;
    temp->arg3 = arg3;
    temp->next = head;
    head = temp;
  }
  
//  qsamplerlist[fid]->signalSimu(fid, arg1, arg2, arg3);
}

extern "C" void QEMUReader_finish(uint32_t fid)
{
  MSG("QEMUReader_finish(%d)",fid);
  qsamplerlist[fid]->stop();
  qsamplerlist[fid]->pauseThread(fid);
  qsamplerlist[fid]->terminate();
}

extern "C" void QEMUReader_finish_thread(uint32_t fid)
{
  MSG("QEMUReader_finish_thread(%d)",fid);
  qsamplerlist[fid]->stop();
  qsamplerlist[fid]->pauseThread(fid);
}

extern "C" void QEMUReader_start_roi(uint32_t fid)
{
  qsamplerlist[fid]->start_roi();
}

extern "C" void QEMUReader_syscall(uint32_t num, uint64_t usecs, uint32_t fid)
{
  //MSG("syscall %d - %lx - %d",num,usecs,fid);
  //qsamplerlist[fid]->syscall(num, usecs, fid);
}

#if 1
extern "C" FlowID QEMUReader_resumeThreadGPU(FlowID uid) {
  return(qsamplerlist[uid]->resumeThread(uid));
}

extern "C" FlowID QEMUReader_cpu_start(uint32_t cpuid) { 
  qsamplerlist[0]->setFid(cpuid);
  MSG("cpu_start %d",cpuid);
  return(qsamplerlist[cpuid]->resumeThread(cpuid, cpuid));
}
extern "C" FlowID QEMUReader_cpu_stop(uint32_t cpuid) { 
  MSG("cpu_stop %d",cpuid);
  qsamplerlist[cpuid]->pauseThread(cpuid);
}

extern "C" FlowID QEMUReader_resumeThread(FlowID uid, FlowID last_fid) { 
  uint32_t fid = qsamplerlist[0]->getFid(last_fid); 
  MSG("resume %d -> %d",last_fid,fid);
  return(qsamplerlist[fid]->resumeThread(uid, fid));
}
extern "C" void QEMUReader_pauseThread(FlowID fid) {
  qsamplerlist[fid]->pauseThread(fid);
}

extern "C" void QEMUReader_setFlowCmd(bool* flowStatus) {

}
#endif

extern "C" int32_t QEMUReader_setnoStats(FlowID fid){
  return 0;
}


