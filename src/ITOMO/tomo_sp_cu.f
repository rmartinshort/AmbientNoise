c--------------------MAIN TOMOGRAPHY-------------------------
c Author: M.P. Barmin  Date: 03/02/01
c Tomography program: itomo_ra_sp_cu_dec_2 (v1.4.1)
c Rejection via formulas: delta < n*lambda +
c geographical or geocentrical setting + strict type definition+
c resolution analysis
c--------------------MAIN TOMOGRAPHY-------------------------
      IMPLICIT NONE
      include "tomo.h"
      include "line.h"
c-------------------INITIATION------------------------------------S
      CALL INIT
      if(INDEX.NE.0) goto 98   
c-------------------INITIATION------------------------------------E
c---------------------INPUT DATA READING AND CHECKING-------------S
      CALL READER
      if(INDEX.NE.0) goto 98   
      if(lrejd) then
      CALL REJEDEL
      N=NOUT
      endif
      if(lreje)then
      CALL REJECTOR
      N=NOUT
      endif
c--------------------INPUT MODEL READING AND CHECKING-------------E       
c------------PRELIMINARY PROCEDURES-------------------------------S
      CALL MODEL
      if(INDEX.NE.0) goto 98
c------------PRELIMINARY PROCEDURES-------------------------------E
c-----------------INVERSION PROCEDURES-----------------S
      CALL SWTSPH
      if(KOD.GT.0.AND.KOD.LT.32) goto 20
      if(lsymb) then
      print*,'to convert velocity in % of deviation from average value'
      CALL PERC_MAP(outfile,NXY)
      endif
      close(8)
      goto 99                            
c-----------------INVERSION PROCEDURES-----------------E
   20 print *,'RUN IS INTERRUPTED; KOD=',KOD 
      STOP
   99 print *,'COMPUTATIONS FINISHED'
      STOP
   98 print *,'RUN IS INTERRUPTED; INDEX=',INDEX
      STOP
      end               
