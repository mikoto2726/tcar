################################################################################
# micro T-Kernel 3.00.03  makefile
################################################################################

OBJS += \
./mtkernel_3/device/i2c/sysdepend/sim_linux/i2c_sim.o 

C_DEPS += \
./mtkernel_3/device/i2c/sysdepend/sim_linux/i2c_sim.d 


# Each subdirectory must supply rules for building sources it contributes
mtkernel_3/device/i2c/sysdepend/sim_linux/%.o: ../device/i2c/sysdepend/sim_linux/%.c
	@echo 'Building file: $<'
	$(GCC) $(CFLAGS) -D$(TARGET) $(INCPATH) -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
