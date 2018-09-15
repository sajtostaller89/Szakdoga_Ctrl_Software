################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := G:/Programok/ti - Szakdoga/ccsv8/tools/compiler/ti-cgt-c2000_18.1.2.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./F2806x_CodeStartBranch.obj" \
"./F2806x_GlobalVariableDefs.obj" \
"./F2806x_usDelay.obj" \
"./hal.obj" \
"./main.obj" \
"./resolver.obj" \
"../f28069M_RAM.cmd" \
"../cmd/F2806x_Headers_nonBIOS.cmd" \
"../include/IQmath_fpu32.lib" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include cmd/subdir_vars.mk
-include include/subdir_vars.mk
-include subdir_rules.mk
-include cmd/subdir_rules.mk
-include include/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
TIDA-01527.out \

EXE_OUTPUTS__QUOTED += \
"TIDA-01527.out" \

BIN_OUTPUTS += \
TIDA-01527.hex \

BIN_OUTPUTS__QUOTED += \
"TIDA-01527.hex" \


# All Target
all:
	@$(MAKE) --no-print-directory pre-build
	@$(MAKE) --no-print-directory main-build

# Main-build Target
main-build: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "TIDA-01527.out"

# Tool invocations
TIDA-01527.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"G:/Programok/ti - Szakdoga/ccsv8/tools/compiler/ti-cgt-c2000_18.1.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 --advice:performance=all -g --diag_warning=225 --diag_wrap=off --display_error_number -z -m"TIDA-01527.map" --stack_size=0x300 --warn_sections -i"G:/Programok/ti - Szakdoga/ccsv8/tools/compiler/ti-cgt-c2000_18.1.2.LTS/lib" -i"G:/Programok/ti - Szakdoga/ccsv8/tools/compiler/ti-cgt-c2000_18.1.2.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="TIDA-01527_linkInfo.xml" --rom_model -o "TIDA-01527.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

TIDA-01527.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: C2000 Hex Utility'
	"G:/Programok/ti - Szakdoga/ccsv8/tools/compiler/ti-cgt-c2000_18.1.2.LTS/bin/hex2000"  -o "TIDA-01527.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "F2806x_CodeStartBranch.obj" "F2806x_GlobalVariableDefs.obj" "F2806x_usDelay.obj" "hal.obj" "main.obj" "resolver.obj" 
	-$(RM) "F2806x_GlobalVariableDefs.d" "hal.d" "main.d" "resolver.d" 
	-$(RM) "F2806x_CodeStartBranch.d" "F2806x_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

pre-build:
	-C:\_Apps\astyle\bin\AStyle --style=allman --indent=spaces=4 --indent-switches --pad-oper --pad-header --unpad-paren --convert-tabs --break-blocks --delete-empty-lines --add-one-line-brackets --recursive "C:/_Work/100 Tools/CCS/TIDA-01527\*.c"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

