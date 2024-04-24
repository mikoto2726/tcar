################################################################################
# micro T-Kernel 3.00.03  makefile
################################################################################

OBJS += \
./mtkernel_3/device/pmd/pmd.o 

C_DEPS += \
./mtkernel_3/device/pmd/pmd.d 

mtkernel_3/device/pmd/%.o: ../device/pmd/%.c
	@echo 'Building file: $<'
	$(GCC) $(CFLAGS) -D$(TARGET) $(INCPATH) -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
