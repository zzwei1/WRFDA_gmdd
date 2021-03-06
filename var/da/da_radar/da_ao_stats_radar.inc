subroutine da_ao_stats_radar (stats_unit, iv, re)

   !-----------------------------------------------------------------------
   ! Purpose: TBD
   !-----------------------------------------------------------------------

   implicit none

   integer,        intent (in)    :: stats_unit    ! Output unit for stats.
   type (iv_type), intent (inout) :: iv            ! iv
   type (y_type),  intent (in)    :: re            ! A - O

   type (stats_radar_type) :: stats
   integer                 :: nrv, nrf, nrrn, nrsn, nrgr,nrqv
   integer                 :: n, k

   if (trace_use) call da_trace_entry("da_ao_stats_radar")

   nrv = 0
   nrf = 0
   nrrn= 0
   nrsn= 0
   nrgr= 0
   nrqv= 0

   stats%maximum%rv = maxmin_type (missing_r, 0, 0)
   stats%maximum%rf = maxmin_type (missing_r, 0, 0)
   stats%minimum%rv = maxmin_type(-missing_r, 0, 0)
   stats%minimum%rf = maxmin_type(-missing_r, 0, 0)

   stats%maximum%rrn = maxmin_type (missing_r, 0, 0)
   stats%maximum%rsn = maxmin_type (missing_r, 0, 0)
   stats%maximum%rgr = maxmin_type (missing_r, 0, 0)
   stats%maximum%rcl = maxmin_type (missing_r, 0, 0)
   stats%maximum%rci = maxmin_type (missing_r, 0, 0)
   stats%maximum%rqv = maxmin_type (missing_r, 0, 0)

   stats%minimum%rrn = maxmin_type(-missing_r, 0, 0)
   stats%minimum%rsn = maxmin_type(-missing_r, 0, 0)
   stats%minimum%rgr = maxmin_type(-missing_r, 0, 0)
   stats%minimum%rcl = maxmin_type(-missing_r, 0, 0)
   stats%minimum%rci = maxmin_type(-missing_r, 0, 0)
   stats%minimum%rqv = maxmin_type(-missing_r, 0, 0)

   stats%average = residual_radar1_type(0.0, 0.0, 0.0, 0.0, 0.0,0.0,0.0,0.0)
   stats%rms_err = stats%average

   do n=1, iv%info(radar)%nlocal
      if (iv%info(radar)%proc_domain(1,n)) then
         do k=1, iv%info(radar)%levels(n)
            if (use_radar_rv) then
               call da_stats_calculate (n, k, iv%radar(n)%rv(k)%qc, & 
                  re%radar(n)%rv(k), nrv, & 
                  stats%minimum%rv, stats%maximum%rv, &
                  stats%average%rv, stats%rms_err%rv)
            end if

            if (use_radar_rf.or.(use_radar_rhv.and.radar_rf_opt>=1)) then
               call da_stats_calculate (n, k, iv%radar(n)%rf(k)%qc, & 
                  re%radar(n)%rf(k), nrf, & 
                  stats%minimum%rf, stats%maximum%rf, &
                  stats%average%rf, stats%rms_err%rf)
            end if

            if (.not. use_radar_rf .and. use_radar_rhv.and.radar_rf_opt==1) then
               call da_stats_calculate (n, k, iv%radar(n)%rrn(k)%qc, &
                  re%radar(n)%rrn(k), nrrn, &
                  stats%minimum%rrn, stats%maximum%rrn, &
                  stats%average%rrn, stats%rms_err%rrn)
              call da_stats_calculate (n, k, iv%radar(n)%rsn(k)%qc, &
                  re%radar(n)%rsn(k), nrsn, &
                  stats%minimum%rsn, stats%maximum%rsn, &
                  stats%average%rsn, stats%rms_err%rsn)
               call da_stats_calculate (n, k, iv%radar(n)%rgr(k)%qc, &
                  re%radar(n)%rgr(k), nrgr, &
                  stats%minimum%rgr, stats%maximum%rgr, &
                  stats%average%rgr, stats%rms_err%rgr)
            end if

            if (use_radar_rqv.and.radar_rf_opt==1) then
               call da_stats_calculate (n, k, iv%radar(n)%rqv(k)%qc, &
                  re%radar(n)%rqv(k), nrqv, &
                  stats%minimum%rqv, stats%maximum%rqv, &
                  stats%average%rqv, stats%rms_err%rqv)
            end if
         end do
      end if    
   end do
   ! Do inter-processor communication to gather statistics.
   if (use_radar_rv) then
      call da_proc_sum_int (nrv)
      call da_proc_stats_combine(stats%average%rv, stats%rms_err%rv, &
         stats%minimum%rv%value, stats%maximum%rv%value, &
         stats%minimum%rv%n, stats%maximum%rv%n, &
         stats%minimum%rv%l, stats%maximum%rv%l)
   end if

   if (use_radar_rf.or.(use_radar_rhv.and.radar_rf_opt>=1)) then
      call da_proc_sum_int (nrf)
      call da_proc_stats_combine(stats%average%rf, stats%rms_err%rf, &
          stats%minimum%rf%value, stats%maximum%rf%value, &
          stats%minimum%rf%n, stats%maximum%rf%n, &
          stats%minimum%rf%l, stats%maximum%rf%l)
   end if

   if (.not.use_radar_rf .and. use_radar_rhv.and.radar_rf_opt==1) then
      call da_proc_sum_int (nrrn)
      call da_proc_stats_combine(stats%average%rrn, stats%rms_err%rrn, &
          stats%minimum%rrn%value, stats%maximum%rrn%value, &
          stats%minimum%rrn%n, stats%maximum%rrn%n, &
          stats%minimum%rrn%l, stats%maximum%rrn%l)
      call da_proc_sum_int (nrsn)
      call da_proc_stats_combine(stats%average%rsn, stats%rms_err%rsn, &
          stats%minimum%rsn%value, stats%maximum%rsn%value, &
          stats%minimum%rsn%n, stats%maximum%rsn%n, &
          stats%minimum%rsn%l, stats%maximum%rsn%l)
      call da_proc_sum_int (nrgr)
      call da_proc_stats_combine(stats%average%rgr, stats%rms_err%rgr, &
          stats%minimum%rgr%value, stats%maximum%rgr%value, &
          stats%minimum%rgr%n, stats%maximum%rgr%n, &
          stats%minimum%rgr%l, stats%maximum%rgr%l)
   end if

   if (use_radar_rqv.and.radar_rf_opt==1) then
      call da_proc_sum_int (nrqv)
      call da_proc_stats_combine(stats%average%rqv, stats%rms_err%rqv, &
          stats%minimum%rqv%value, stats%maximum%rqv%value, &
          stats%minimum%rqv%n, stats%maximum%rqv%n, &
          stats%minimum%rqv%l, stats%maximum%rqv%l)
   end if
   iv%nstats(radar) = nrv + nrf + nrrn + nrsn + nrgr + nrqv

   if (rootproc) then
      if (nrv /= 0 .or. nrf /= 0 .or. nrrn /= 0 .or. nrsn /= 0 .or. nrgr /= 0 .or. nrqv /= 0) then 
         write(unit=stats_unit, fmt='(/a/)') ' Diagnostics of AO for radar'
         call da_print_stats_radar(stats_unit, nrv, nrf, nrrn, nrsn, nrgr, nrqv, stats)
      end if
   end if

   if (trace_use) call da_trace_exit("da_ao_stats_radar")

end subroutine da_ao_stats_radar


