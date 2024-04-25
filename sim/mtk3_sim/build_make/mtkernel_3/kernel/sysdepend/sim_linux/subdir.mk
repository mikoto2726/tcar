################################################################################
# micro T-Kernel 3.00.03  makefile
################################################################################

OBJS += \
./mtkernel_3/kernel/sysdepend/sim_linux/cpu_cntl.o \
./mtkernel_3/kernel/sysdepend/sim_linux/devinit.o \
./mtkernel_3/kernel/sysdepend/sim_linux/hw_setting.o \
./mtkernel_3/kernel/sysdepend/sim_linux/interrupt.o \
./mtkernel_3/kernel/sysdepend/sim_linux/power_save.o \
./mtkernel_3/kernel/sysdepend/sim_linux/sys_timer.o 

C_DEPS += \
./mtkernel_3/kernel/sysdepend/sim_linux/cpu_cntl.d \
./mtkernel_3/kernel/sysdepend/sim_linux/devinit.d \
./mtkernel_3/kernel/sysdepend/sim_linux/hw_setting.d \
./mtkernel_3/kernel/sysdepend/sim_linux/interrupt.d \
./mtkernel_3/kernel/sysdepend/sim_linux/power_save.d \
./mtkernel_3/kernel/sysdepend/sim_linux/sys_timer.d 


mtkernel_3/kernel/sysdepend/sim_linux/%.o: ../kernel/sysdepend/sim_linux/%.c
	@echo 'Building file: $<'
	$(GCC) $(CFLAGS) -D$(TARGET) $(INCPATH) -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
