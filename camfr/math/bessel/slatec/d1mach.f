      double precision function d1mach (i)
c
c     Machine constants for SLATEC, from the Fortran intrinsics:
c       d1mach(1) = smallest positive normalised number (tiny)
c       d1mach(2) = largest number (huge)
c       d1mach(3) = smallest relative spacing, epsilon/radix
c       d1mach(4) = largest relative spacing, epsilon
c       d1mach(5) = log10(radix)
c
c     Modified for camfr3: this used to call machard from machar.c
c     (W. J. Cody, MACHAR, ACM TOMS 14, 1988), which computed the same
c     values at run time; the intrinsics give bit-identical results
c     (PORTING_JOURNAL.md, entry 58).
c
      integer i
      if (i .eq. 1) then
        d1mach = tiny(1d0)
      else if (i .eq. 2) then
        d1mach = huge(1d0)
      else if (i .eq. 3) then
        d1mach = epsilon(1d0) / radix(1d0)
      else if (i .eq. 4) then
        d1mach = epsilon(1d0)
      else if (i .eq. 5) then
        d1mach = log10(dble(radix(1d0)))
      else
        write(*,1999) i
 1999   format(' d1mach - i out of bounds', i10)
        stop
      endif
      end
