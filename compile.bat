@echo off
cl main.c /Fe:darwin > NUL 2>&1
cl comp.c /Fe:dcomp > NUL 2>&1
del main.obj
del comp.obj
@echo on
echo Compilation Complete