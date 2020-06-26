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

@set PROJECT_DIR=%1
@set KEIL_ARM=%2
@set TOOLS=..\..\..\tools

@REM add fromelf to PATH
@set PATH=%KEIL_ARM%\ARMCC\bin;%path%

@python --version 2> nul
@if %errorlevel% neq 0 echo Error: python not in PATH. If you are manually building the project, make sure to launch uVision from the python venv && exit /B %errorlevel%

fromelf --bin %PROJECT_DIR%.axf -o %PROJECT_DIR%.bin
@if %errorlevel% neq 0 exit /b %errorlevel%

fromelf --i32 %PROJECT_DIR%.axf -o %PROJECT_DIR%.hex
@if %errorlevel% neq 0 exit /b %errorlevel%

python %TOOLS%\post_build_script.py %PROJECT_DIR%.hex %PROJECT_DIR%_crc
@if %errorlevel% neq 0 exit /b %errorlevel%

@exit 0
