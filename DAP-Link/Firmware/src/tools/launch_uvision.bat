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
:: Launches uVision with the python environment needed to build DAPLink
::
:: git and python are expected to be in PATH. Project will fail to build otherwise
:: 

setlocal

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
set uv4exe=%UV4%

@rem Select an existing virtualenv as first parameter
@if not [%1]==[] (
	echo Using virtualenv %1
	@if exist %1 call %1\Scripts\activate && goto :env_folder_ok
) else ( 
	@if exist env goto :env_ok
)


echo Creating python virtual environment && virtualenv env
:env_ok
call env\Scripts\activate
:env_folder_ok

@echo Doing pip install
@REM use project requirements if not specified
if [%2]==[] pip install -r requirements.txt
@REM use custom requirements if specified
if not [%2]==[] pip install -r %2

start %uv4exe%
exit /B 0

:error_nomdk
@echo Error: Keil MDK not installed or not found. If you installed it to a 
@echo non-default location, you need to set environment variable UV4 to 
@echo the path of the executable
@exit /B 1

