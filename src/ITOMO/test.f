      real*8 s ,s2
      do i=1,20
      s=1.0d0*i-10.0d0
      s2=DEXP(s)
      write(*,*)s s2
      enddo
      end
