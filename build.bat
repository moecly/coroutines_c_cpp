@if "%1"=="cl" (
    @goto clean
)

@if "%1"=="mk" (
    @goto make
)

@if "%1"=="" (
    @goto make
)

@if "%1"=="re" (
    @goto rebuild
)

@goto end

:clean
@make -j18 clean
@goto end

:make
@make -j18 all
@goto end

:rebuild
@make -j18 clean 
@make -j18 all
@goto end

:end
