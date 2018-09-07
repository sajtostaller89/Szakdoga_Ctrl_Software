################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"G:/Programok/ti - Szakdoga/ccsv8/tools/compiler/ti-cgt-c2000_18.1.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 --include_path="G:/GitHub/Szakdoga_Ctrl_Software/projecto/ADC_PWM_TIMER" --include_path="G:/Programok/ti - Szakdoga/ccsv8/tools/compiler/ti-cgt-c2000_18.1.2.LTS/include" --include_path="G:/GitHub/Szakdoga_Ctrl_Software/projecto/ADC_PWM_TIMER/Include" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="main.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


