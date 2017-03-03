#define NSTATION 250
#define NEVENTS 500
#define COMPONENT 3


typedef struct event
{
  float lat, lon;
  int yy, mm, dd, h, m, s, ms, jday;
  double t0;
  char name[40];
}
EVENT;

typedef struct station
{
  float lat, lon;
  char name[10];
}
STATION;

typedef struct record
{
  char fname[150];
  char ft_fname[150];
  char resp_fname[150];
  char chan[7];
  double t0;
  float dt;
  long n;
}
RECORD;

/* introduced a third dimension to the record array, rec, for three components with the convention 0-N, 1-E, 2-Z. */
typedef struct sac_dbase
{
  EVENT ev[NEVENTS];
  STATION st[NSTATION];
  RECORD rec[NEVENTS][NSTATION][COMPONENT];
  int nev, nst;
}
SAC_DB;

/*typedef struct sac_dbase3
{
  EVENT ev[NEVENTS];
  STATION st[NSTATION];
  RECORD rz[NEVENTS][NSTATION][COMPONENT];
  RECORD rn[NEVENTS][NSTATION][COMPONENT];
  RECORD re[NEVENTS][NSTATION][COMPONENT];
  int nev, nst;
}
SAC_DB3; */
