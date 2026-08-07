@echo off
setlocal

rem === Configuration ===
set APP_NAME=BilliardViewer
set MAIN_JAR=billiard-viewer.jar
set MAIN_CLASS=billiards.viewer.Main
set ICON_PATH=icon.ico
set BUILD_DIR=build
set INPUT_DIR=%BUILD_DIR%\libs
set DIST_DIR=dist
set RUNTIME_IMAGE=%BUILD_DIR%\custom-runtime
set DYLIB_TEMP_DIR=%BUILD_DIR%\javafx-natives

rem === Step 1: Auto-detect JAVA_HOME ===
rem Assumes JDK 17 is on the system PATH
for /f "tokens=1,* delims==" %%a in ('java -XshowSettings:properties -version 2^>^&1 ^| findstr "java.home"') do set "JAVA_HOME_RAW=%%b"
set "JAVA_HOME=%JAVA_HOME_RAW:~1%"
echo [INFO] Detected JAVA_HOME: %JAVA_HOME%

rem === Step 2: Ensure main JAR is present ===
echo [INFO] Ensuring main JAR is present...
call gradlew.bat jar -q
copy /y "%INPUT_DIR%\%MAIN_JAR%" "%BUILD_DIR%\%MAIN_JAR%"

rem === Step 3: Resolve JavaFX runtime JARs from Gradle ===
echo [INFO] Resolving JavaFX runtime JARs...
call gradlew.bat copyRuntimeLibs -q

rem === Step 4: jlink with JavaFX modules ===
echo [INFO] Creating custom Java runtime with jlink...
for /f "delims=" %%i in ('gradlew.bat -q runtimeClasspathAsPath') do set RUNTIME_CP=%%i
rmdir /s /q "%RUNTIME_IMAGE%" 2>nul
"%JAVA_HOME%\bin\jlink" ^
  --module-path "%JAVA_HOME%\jmods;%RUNTIME_CP%" ^
  --add-modules java.base,java.logging,java.desktop,java.management,javafx.controls,javafx.fxml,javafx.graphics,java.sql,jdk.crypto.ec,java.security.jgss ^
  --output "%RUNTIME_IMAGE%" ^
  --strip-debug ^
  --compress=2 ^
  --no-header-files ^
  --no-man-pages

rem === Step 5: Copy native DLL dependencies ===
echo [INFO] Copying MSYS2 native DLL dependencies to input\backend\shared...
copy /y "C:\msys64\ucrt64\bin\libgmp-10.dll" "%INPUT_DIR%\backend\shared\" 2>nul
copy /y "C:\msys64\ucrt64\bin\libmpfr-6.dll" "%INPUT_DIR%\backend\shared\" 2>nul
copy /y "C:\msys64\ucrt64\bin\libmpfi-0.dll" "%INPUT_DIR%\backend\shared\" 2>nul
copy /y "C:\msys64\ucrt64\bin\libtbb12.dll" "%INPUT_DIR%\backend\shared\" 2>nul
copy /y "C:\msys64\ucrt64\bin\libsqlite3-0.dll" "%INPUT_DIR%\backend\shared\" 2>nul
copy /y "C:\msys64\ucrt64\bin\libboost_thread-mt.dll" "%INPUT_DIR%\backend\shared\" 2>nul
copy /y "C:\msys64\ucrt64\bin\libwinpthread-1.dll" "%INPUT_DIR%\backend\shared\" 2>nul
copy /y "C:\msys64\ucrt64\bin\libstdc++-6.dll" "%INPUT_DIR%\backend\shared\" 2>nul
copy /y "C:\msys64\ucrt64\bin\libgcc_s_seh-1.dll" "%INPUT_DIR%\backend\shared\" 2>nul

rem === Step 6: Package with jpackage ===
rem arthur 06/08/2026 [give the installed Windows launcher the same documented JVM memory defaults as Gradle run]
rem Required for JNA in Java 22+
echo [INFO] Running jpackage...
"%JAVA_HOME%\bin\jpackage" ^
  --type app-image ^
  --name "%APP_NAME%" ^
  --input "%INPUT_DIR%" ^
  --main-jar "%MAIN_JAR%" ^
  --main-class "%MAIN_CLASS%" ^
  --runtime-image "%RUNTIME_IMAGE%" ^
  --dest "%DIST_DIR%" ^
  --win-console ^
  --java-options "-server" ^
  --java-options "-Xss2m" ^
  --java-options "-Xms2g" ^
  --java-options "-Xmx6g" ^
  --java-options "-XX:MaxDirectMemorySize=2g" ^
  --java-options "-XX:+UnlockDiagnosticVMOptions" ^
  --java-options "-Djna.library.path=$APPDIR\backend\shared" ^
  --java-options "--enable-native-access=ALL-UNNAMED,javafx.graphics" ^
  --java-options "--add-modules=javafx.controls,javafx.fxml,java.sql" ^
  --app-version 2.1

echo [SUCCESS] Installer created at %DIST_DIR%
endlocal
