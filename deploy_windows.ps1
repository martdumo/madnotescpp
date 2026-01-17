# Script de Despliegue (Corregido para VS2022)
# Ajusta esta ruta a tu instalaci?n de Qt:
$QT_BIN_PATH = "C:\Qt\6.10.1\msvc2019_64\bin" 

# Ruta de compilaci?n de Visual Studio (Ninja)
$BUILD_DIR = "out\build\x64-Release"
$DEPLOY_DIR = "MadNotes_Portable"

Write-Host ">>> INICIANDO DESPLIEGUE..." -ForegroundColor Cyan

# 1. Limpieza
if (Test-Path $DEPLOY_DIR) { Remove-Item -Recurse -Force $DEPLOY_DIR }
New-Item -ItemType Directory -Force -Path $DEPLOY_DIR | Out-Null

# 2. Verificar Ejecutable
$EXE_SOURCE = "$BUILD_DIR\MadNotesCpp.exe"
if (-not (Test-Path $EXE_SOURCE)) {
    Write-Error "ERROR: No se encuentra $EXE_SOURCE"
    Write-Host "ASEGURATE DE HABER COMPILADO EN 'x64-Release' EN VISUAL STUDIO." -ForegroundColor Yellow
    exit
}

# 3. Copiar Ejecutable
Copy-Item $EXE_SOURCE -Destination $DEPLOY_DIR
Write-Host "Ejecutable copiado."

# 4. Ejecutar windeployqt
Write-Host "Empaquetando DLLs..."
$WINDEPLOY = "$QT_BIN_PATH\windeployqt.exe"

if (Test-Path $WINDEPLOY) {
    & $WINDEPLOY --no-translations --compiler-runtime --dir $DEPLOY_DIR "$DEPLOY_DIR\MadNotesCpp.exe"
} else {
    # Intentar usar el del PATH global si la ruta falla
    windeployqt --no-translations --compiler-runtime --dir $DEPLOY_DIR "$DEPLOY_DIR\MadNotesCpp.exe"
}

Write-Host "LISTO. Carpeta generada: $DEPLOY_DIR" -ForegroundColor Green