
#ifndef DRAM_H
#define DRAM_H

// Bank state 
enum STATE {
    IDLE = 0,
    ACTIVE
};

// DDRx Power state
enum POWERSTATE {
    sIDD2PF = 0,   // CKE low, all banks precharged, fast-exit mode
    sIDD2PS,       // CKE low, all banks precharged, slow-exit mode
    sIDD3P,        // CKE low, at least one bank active, fast-exit mode
    sIDD2N,        // CKE high, all banks precharged
    sIDD3N         // CKE high, at least one bank active
};

// Exit mode register
enum EMR {
    SLOW = 0,      // Slow-exit mode
    FAST           // Fast-exit mode
};

// Serviced request
typedef struct _SERVICEDREQUEST {
  Time_t time;
  MemRequest *mreq;
} ServicedRequest;

#endif // DRAM_H
