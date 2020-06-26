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

@echo off

:: Get the source path
set /p path_to_source=Enter the source path:
:: a method for removing the surrounding quotes when spaces are used
::  in the path of a passed directory
set path_to_source=%path_to_source:"=%
:: and the types of files to format
set cpp=*.cpp
set cc=*.cc
set c=*.c
set h=*.h
set hh=*.hh
set hh=*.hpp

echo.
echo formating files of type:
echo %path_to_source%\%cpp%
echo %path_to_source%\%cc%
echo %path_to_source%\%c%
echo %path_to_source%\%h%
echo %path_to_source%\%hh%
echo.

astyle --options=style.txt "%path_to_source%\%cpp%"
astyle --options=style.txt "%path_to_source%\%cc%"
astyle --options=style.txt "%path_to_source%\%c%"
astyle --options=style.txt "%path_to_source%\%hh%"
astyle --options=style.txt "%path_to_source%\%h%"
astyle --options=style.txt "%path_to_source%\%hpp%"


pause