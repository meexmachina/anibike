################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../AniBike2.c \
../AniBike2_DataFlash.c \
../AniBike2_FileSystem.c \
../AniBike2_TransferMode.c \
../AniBike2_Usart_Comm.c 

S_UPPER_SRCS += \
../AniBike2_Processing.S 

OBJS += \
./AniBike2.o \
./AniBike2_DataFlash.o \
./AniBike2_FileSystem.o \
./AniBike2_Processing.o \
./AniBike2_TransferMode.o \
./AniBike2_Usart_Comm.o 

C_DEPS += \
./AniBike2.d \
./AniBike2_DataFlash.d \
./AniBike2_FileSystem.d \
./AniBike2_TransferMode.d \
./AniBike2_Usart_Comm.d 

S_UPPER_DEPS += \
./AniBike2_Processing.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O3 -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=atmega325p -DF_CPU=19660800UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.S
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Assembler'
	avr-gcc -x assembler-with-cpp -W -g2 -gstabs -mmcu=atmega325p -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


