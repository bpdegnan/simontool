@setlocal
@echo off
rem svnstrip.bat
rem Given a string possibly containing a number, print the first integer.
rem 123test456 -> 123
rem Note that special characters may not be properly handled.  (e.g. , ;)
rem http://stackoverflow.com/questions/6120623/how-to-extract-number-from-string-in-batch
set input=%1
if ("%input%") == ("") goto :eof
call :firstnum input output
if not ("%output%") == ("") echo %output%&&goto end_success
goto end_fail

:end_success
endlocal
@exit /b 0

:end_fail
endlocal
@exit /b 1

:firstnum
SETLOCAL ENABLEDELAYEDEXPANSION
call set "string=%%%~1%%"
set /a index = 0
set return_number=
goto firstnum_loop

:firstnum_loop
if ("!string:~%index%,1!") == ("") goto firstnum_end
set test_char=!string:~%index%,1!
call :isdigit test_char is_digit
rem Found a digit?  Add it to the return.
if ("%is_digit%") == ("true") set return_number=%return_number%%test_char%
rem Found a not-digit?  If we found a digit before, end.
if ("%is_digit%") == ("false") if not ("%return_number%") == ("") goto firstnum_end
set /a index = %index% + 1
goto firstnum_loop

:firstnum_end
( ENDLOCAL & REM RETURN VALUES
    IF "%~2" NEQ "" SET "%~2=%return_number%"
)
goto :eof

:isdigit
SETLOCAL ENABLEDELAYEDEXPANSION
set NUMBERS=1234567890
set found_number=false
call set "string=%%%~1%%"
REM If the passed string does not have a single character, return immediately with false.
if ("%string:~0,1%") == ("") goto isdigit_end
if not ("%string:~1,1%") == ("") goto isdigit_end
set /a index=0
goto isdigit_loop

:isdigit_loop
if ("!NUMBERS:~%index%,1!") == ("") goto isdigit_end
set test_char=!NUMBERS:~%index%,1!
if ("%test_char%") == ("%string%") set found_number=true&&goto isdigit_end
set /a index = %index% + 1
goto isdigit_loop

:isdigit_end
( ENDLOCAL & REM RETURN VALUES
    IF "%~2" NEQ "" SET "%~2=%found_number%"
)
goto :eof
