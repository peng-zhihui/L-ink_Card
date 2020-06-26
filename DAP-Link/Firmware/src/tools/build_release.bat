::
:: DAPLink Interface Firmware
:: Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
:: SPDX-License-Identifier: Apache-2.0
::
:: Licensed under the Apache License, Version 2.0 (the "License"); you may
:: not use this file except in compliance with the License.
:: You may obtain a copy of the License at
::
:: http://www.apache.org/licenses/LICENSE-2.0
::
:: Unless required by applicable law or agreed to in writing, software
:: distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
:: WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
:: See the License for the specific language governing permissions and
:: limitations under the License.
::
setlocal enabledelayedexpansion
@rem Script assumes working directory is workspace root. Force it.
cd %~dp0..\

@rem See if we can find uVision. This logic is consistent with progen
@if [%UV4%]==[] (
	@echo UV4 variable is not set, trying to autodetect..
	if EXIST c:\keil\uv4\uv4.exe (
		set UV4=c:\keil\uv4\uv4.exe
	) else if EXIST c:\keil_v5\uv4\uv4.exe (
		set UV4=c:\keil_v5\uv4\uv4.exe
	) else goto error_nomdk
)
@echo USING UV4=%UV4%

@set project_tool=%1
@set env_folder=%2
@set requirements_file=%3


@rem Select an existing virtualenv as first parameter
@if not [%env_folder%]==[] (
	echo Using virtualenv %env_folder%
	@if exist %env_folder% call %env_folder%\Scripts\activate && goto :env_folder_ok
) else ( 
	echo Checking env folder
	@if exist env goto :env_ok
)


echo Creating python virtual environment && virtualenv env
:env_ok
call env\Scripts\activate
:env_folder_ok

@echo Doing pip install
@REM use project requirements if not specified
if [%requirements_file%]==[] pip install -r requirements.txt
@REM use custom requirements if specified
if not [%requirements_file%]==[] pip install -r %requirements_file%

@if [%project_tool%]==[mbedcli] (
	@REM setup mbed dependencies
	echo Building for mbed cli

	@if [%ARM_PATH%]==[] (
		@echo ARM_PATH variable is not set, trying to autodetect..
		if EXIST C:\KEIL_V4\ARM\ARMCC\ (
			set ARM_PATH=C:\KEIL_V4\ARM\ARMCC
		) else if EXIST C:\KEIL_V5\ARM\ARMCC\ (
			set ARM_PATH=C:\KEIL_V5\ARM\ARMCC
		) else goto error_armpath
	)
	@echo USING ARM_PATH=!ARM_PATH!

	mbed deploy
	mbed config root .
	mbed config ARM_PATH !ARM_PATH!
	python tools/mbedcli_compile.py --clean --release
	@if !errorlevel! neq 0 exit /B !errorlevel!
	python tools/copy_release_files.py --project-tool mbedcli
	exit /B !errorlevel!
) else ( 

	@REM build bootloader images first (In the future this should be done with a pattern like *_bl)
	echo Building for uvision release
	progen generate -t uvision -b -p kl26z_bl
	progen generate -t uvision -b -p k20dx_bl
	progen generate -t uvision -b -p sam3u2c_bl
	@REM build but continue if there are errors
	progen generate -t uvision -b
	@if !errorlevel! neq 0 exit /B !errorlevel!
	python tools/copy_release_files.py --project-tool uvision
	exit /B !errorlevel!
)

:error_nomdk
@echo Error: Keil MDK not installed or not found. If you installed it to a 
@echo non-default location, you need to set environment variable UV4 to 
@echo the path of the executable
@exit /B 1

:error_armpath
@echo Error: Need to set environment variable ARM_PATH to 
@echo the path of ARM compiler
@exit /B 1
