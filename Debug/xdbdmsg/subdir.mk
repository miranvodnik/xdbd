################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../xdbdmsg/XdbdMessages_xdr.o 

C_SRCS += \
../xdbdmsg/XdbdMessages_clnt.c \
../xdbdmsg/XdbdMessages_svc.c \
../xdbdmsg/XdbdMessages_xdr.c 

OBJS += \
./xdbdmsg/XdbdMessages_clnt.o \
./xdbdmsg/XdbdMessages_svc.o \
./xdbdmsg/XdbdMessages_xdr.o 

C_DEPS += \
./xdbdmsg/XdbdMessages_clnt.d \
./xdbdmsg/XdbdMessages_svc.d \
./xdbdmsg/XdbdMessages_xdr.d 


# Each subdirectory must supply rules for building sources it contributes
xdbdmsg/%.o: ../xdbdmsg/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


