################################################################################
# micro T-Kernel 3.00.03  makefile
################################################################################

OBJS += \
./mtkernel_3/lib/libtk/sysdepend/cpu/sim_linux/int_sim.o 

C_DEPS += \
./mtkernel_3/lib/libtk/sysdepend/cpu/sim_linux/int_sim.d 

mtkernel_3/lib/libtk/sysdepend/cpu/sim_linux/%.o: ../lib/libtk/sysdepend/cpu/sim_linux/%.c
	@echo 'Building file: $<'
	$(GCC) $(CFLAGS) -D$(TARGET) $(INCPATH) -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


