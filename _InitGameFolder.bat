@echo off

set /p isDel="Will delete unnecessary folder(regenerated as needed), are you sure? (Y/N):"

IF %isDel%==Y (
 goto delete

) ELSE IF %isDel%==y (
 goto delete

) ELSE (
 echo;
 echo Canceled delete.
 timeout /t -1
 goto :EOF
)

:delete
 echo;
echo Start deleting.

set dir=Teleport

echo;
rd /s /q .vs
echo .vs Completed.
echo;
rd /s /q Binaries
echo Binaries Completed.
echo;
rd /s /q Build
echo Build Completed.
echo;
rd /s /q DerivedDataCache
echo DerivedDataCache Completed.
echo;
rd /s /q Intermediate
echo Intermediate Completed.
echo;
rd /s /q Saved
echo Saved Completed.
echo;
del %dir%.sln
echo %dir%.sln Completed.

echo;
echo Delete completed.
timeout /t -1
