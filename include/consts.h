#ifndef MNA_CONSTS
#define MNA_CONSTS

//#define DEBUG

static const int MAX_NAME = 11;
static const int MAX_NODES = 50;
static const int MAX_ELEMS = 50;
static const int MAX_PARAMS = 8;
static const double TOLG = 1e-9;

static double ZERO_SOLUTION[MAX_NODES+1] = {
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0 // MAX_NODES + 1
};

#endif
