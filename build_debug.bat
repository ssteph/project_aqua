pushd %~dp0
CALL setup_path.bat
CALL build.bat -debug
popd