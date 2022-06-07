!-----------------------------------------------------------------------------
! Program code for CS 415/515 Parallel Programming, Portland State University.
!-----------------------------------------------------------------------------

program arrayop

  ! Array operations

  integer, parameter :: n = 8, x = 1
  integer :: front_idx = 1, back_idx = 1
  integer:: A(n) = (/ (i,i=1,n) /) ! A = (1,2,3,4,5,6,7,8)
  integer :: odd(n/2), even(n/2), front(n/2), back(n/2), reverse(n), shuffle(n)

  odd = A(1:n:2)
  even = A(2:n:2)
  write(*, '(A,4I3)') 'odd =', odd
  write(*, '(A,4I3)') 'even =', even

  front = A(1:n/2)
  back = A(n/2+1:n)
  write(*, '(A,4I3)') 'front =', front
  write(*, '(A,4I3)') 'back =', back

  reverse = A(n:1:-1)
  write(*, '(A,4I3)') 'reverse =', reverse


  do i = 1,n
        if (mod(i,2) > 0) then
                shuffle(i) = front(front_idx)
                front_idx = front_idx + 1

        else if (mod(i,2) .eq. 0) then
                shuffle(i) = back(back_idx)
                back_idx = back_idx + 1
        end if
  end do
  write(*, '(A,8I3)') 'shuffle =', shuffle


end program arrayop