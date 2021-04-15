################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../xdbd/XdbdAdminJob.o \
../xdbd/XdbdAnyJob.o \
../xdbd/XdbdCommon.o \
../xdbd/XdbdController.o \
../xdbd/XdbdLocalClient.o \
../xdbd/XdbdMessageQueue.o \
../xdbd/XdbdMqClient.o \
../xdbd/XdbdNetworkClient.o \
../xdbd/XdbdShmSegment.o \
../xdbd/XdbdSqlConnectJob.o \
../xdbd/XdbdSqlCreateStatementJob.o \
../xdbd/XdbdSqlDeleteJob.o \
../xdbd/XdbdSqlDeleteStatementJob.o \
../xdbd/XdbdSqlDisconnectJob.o \
../xdbd/XdbdSqlExecuteDirectJob.o \
../xdbd/XdbdSqlExecuteJob.o \
../xdbd/XdbdSqlInsertJob.o \
../xdbd/XdbdSqlJob.o \
../xdbd/XdbdSqlPrepareJob.o \
../xdbd/XdbdSqlSelectJob.o \
../xdbd/XdbdSqlUpdateJob.o \
../xdbd/XdbdWorker.o \
../xdbd/XdbdWorkingClient.o \
../xdbd/XdbdWorkingThread.o \
../xdbd/xdbd-main.o 

CPP_SRCS += \
../xdbd/XdbdAdminJob.cpp \
../xdbd/XdbdAnyJob.cpp \
../xdbd/XdbdCommon.cpp \
../xdbd/XdbdController.cpp \
../xdbd/XdbdLocalClient.cpp \
../xdbd/XdbdMessageQueue.cpp \
../xdbd/XdbdMqClient.cpp \
../xdbd/XdbdNetworkClient.cpp \
../xdbd/XdbdShmSegment.cpp \
../xdbd/XdbdSqlConnectJob.cpp \
../xdbd/XdbdSqlCreateStatementJob.cpp \
../xdbd/XdbdSqlDeleteJob.cpp \
../xdbd/XdbdSqlDeleteStatementJob.cpp \
../xdbd/XdbdSqlDisconnectJob.cpp \
../xdbd/XdbdSqlExecuteDirectJob.cpp \
../xdbd/XdbdSqlExecuteJob.cpp \
../xdbd/XdbdSqlInsertJob.cpp \
../xdbd/XdbdSqlJob.cpp \
../xdbd/XdbdSqlPrepareJob.cpp \
../xdbd/XdbdSqlSelectJob.cpp \
../xdbd/XdbdSqlUpdateJob.cpp \
../xdbd/XdbdWorker.cpp \
../xdbd/XdbdWorkingClient.cpp \
../xdbd/XdbdWorkingThread.cpp \
../xdbd/xdbd-main.cpp 

OBJS += \
./xdbd/XdbdAdminJob.o \
./xdbd/XdbdAnyJob.o \
./xdbd/XdbdCommon.o \
./xdbd/XdbdController.o \
./xdbd/XdbdLocalClient.o \
./xdbd/XdbdMessageQueue.o \
./xdbd/XdbdMqClient.o \
./xdbd/XdbdNetworkClient.o \
./xdbd/XdbdShmSegment.o \
./xdbd/XdbdSqlConnectJob.o \
./xdbd/XdbdSqlCreateStatementJob.o \
./xdbd/XdbdSqlDeleteJob.o \
./xdbd/XdbdSqlDeleteStatementJob.o \
./xdbd/XdbdSqlDisconnectJob.o \
./xdbd/XdbdSqlExecuteDirectJob.o \
./xdbd/XdbdSqlExecuteJob.o \
./xdbd/XdbdSqlInsertJob.o \
./xdbd/XdbdSqlJob.o \
./xdbd/XdbdSqlPrepareJob.o \
./xdbd/XdbdSqlSelectJob.o \
./xdbd/XdbdSqlUpdateJob.o \
./xdbd/XdbdWorker.o \
./xdbd/XdbdWorkingClient.o \
./xdbd/XdbdWorkingThread.o \
./xdbd/xdbd-main.o 

CPP_DEPS += \
./xdbd/XdbdAdminJob.d \
./xdbd/XdbdAnyJob.d \
./xdbd/XdbdCommon.d \
./xdbd/XdbdController.d \
./xdbd/XdbdLocalClient.d \
./xdbd/XdbdMessageQueue.d \
./xdbd/XdbdMqClient.d \
./xdbd/XdbdNetworkClient.d \
./xdbd/XdbdShmSegment.d \
./xdbd/XdbdSqlConnectJob.d \
./xdbd/XdbdSqlCreateStatementJob.d \
./xdbd/XdbdSqlDeleteJob.d \
./xdbd/XdbdSqlDeleteStatementJob.d \
./xdbd/XdbdSqlDisconnectJob.d \
./xdbd/XdbdSqlExecuteDirectJob.d \
./xdbd/XdbdSqlExecuteJob.d \
./xdbd/XdbdSqlInsertJob.d \
./xdbd/XdbdSqlJob.d \
./xdbd/XdbdSqlPrepareJob.d \
./xdbd/XdbdSqlSelectJob.d \
./xdbd/XdbdSqlUpdateJob.d \
./xdbd/XdbdWorker.d \
./xdbd/XdbdWorkingClient.d \
./xdbd/XdbdWorkingThread.d \
./xdbd/xdbd-main.d 


# Each subdirectory must supply rules for building sources it contributes
xdbd/%.o: ../xdbd/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"/home/miran/Eclipse/workspace/cdcp.cs6114bx.009/ftpmpx" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbdsqlparser" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbdapi" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbdmsg" -I"/home/miran/Eclipse/workspace/xdbd-xml.stable/xdbd-include" -I/usr/include/libxml2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


