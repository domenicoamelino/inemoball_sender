################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/FreeRTOS/Source/croutine.c \
../src/FreeRTOS/Source/event_groups.c \
../src/FreeRTOS/Source/list.c \
../src/FreeRTOS/Source/queue.c \
../src/FreeRTOS/Source/tasks.c \
../src/FreeRTOS/Source/timers.c 

OBJS += \
./src/FreeRTOS/Source/croutine.o \
./src/FreeRTOS/Source/event_groups.o \
./src/FreeRTOS/Source/list.o \
./src/FreeRTOS/Source/queue.o \
./src/FreeRTOS/Source/tasks.o \
./src/FreeRTOS/Source/timers.o 

C_DEPS += \
./src/FreeRTOS/Source/croutine.d \
./src/FreeRTOS/Source/event_groups.d \
./src/FreeRTOS/Source/list.d \
./src/FreeRTOS/Source/queue.d \
./src/FreeRTOS/Source/tasks.d \
./src/FreeRTOS/Source/timers.d 


# Each subdirectory must supply rules for building sources it contributes
src/FreeRTOS/Source/%.o: ../src/FreeRTOS/Source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -DDEBUG -I"../libs/misc/include" -I"../include" -I"../libs/CMSIS/include" -I"../libs/StdPeriph/include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


