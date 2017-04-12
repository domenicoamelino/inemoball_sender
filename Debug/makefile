################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/iNemo_Led/subdir.mk
-include src/iNEMO_M1_SensorDrivers/Source/src/subdir.mk
-include src/iNEMO_AHRS_v121/src/subdir.mk
-include src/STM32_USB-FS-Device_Lib/Source/Libraries/STM32_USB-FS-Device_Driver/src/subdir.mk
-include src/STM32F1_VirtualCom/Source/src/subdir.mk
-include src/LSM303DLHC/src/subdir.mk
-include src/L3Gx/src/subdir.mk
-include src/subdir.mk
-include libs/misc/src/subdir.mk
-include libs/StdPeriph/src/subdir.mk
-include libs/CMSIS/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
iNemo_SmartBall.hex \

SECONDARY_SIZE += \
iNemo_SmartBall.siz \


# All Target
all: iNemo_SmartBall.elf secondary-outputs

# Tool invocations
iNemo_SmartBall.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -T mem.ld -T libs.ld -T sections.ld -nostartfiles -Xlinker --gc-sections -L"../ldscripts" -Wl,-Map,"iNemo_SmartBall.map" -o "iNemo_SmartBall.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

iNemo_SmartBall.hex: iNemo_SmartBall.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "iNemo_SmartBall.elf"  "iNemo_SmartBall.hex"
	@echo 'Finished building: $@'
	@echo ' '

iNemo_SmartBall.siz: iNemo_SmartBall.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "iNemo_SmartBall.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(SECONDARY_FLASH)$(S_UPPER_DEPS) iNemo_SmartBall.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
