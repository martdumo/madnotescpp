# 🧳 MadNotesCpp "Pro Edition"

![C++](https://img.shields.io/badge/C++-17-blue?style=for-the-badge&logo=c%2B%2B) ![Qt](https://img.shields.io/badge/Qt-6.x-41CD52?style=for-the-badge&logo=qt) ![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey?style=for-the-badge) ![Build](https://img.shields.io/badge/Build-CMake-064F8C?style=for-the-badge&logo=cmake)

**MadNotesCpp** es un gestor de notas y bóveda personal (Personal Vault) diseñado bajo la premisa de **Alto Rendimiento** y **Fidelidad Visual**.

A diferencia de las soluciones basadas en Electron (como Obsidian), este editor ha sido desarrollado nativamente en C++ para ofrecer una latencia cercana a cero, un consumo de recursos mínimo y una experiencia de escritura fluida en cualquier sistema operativo.

---

## 🌟 Características Principales

### 🚀 Flujo de Trabajo "Wiki"
- **Wiki-Links Activos:** Escribe \`[[Concepto]]\` y el sistema lo detectará. Haz clic para navegar o crear la nota automáticamente.
- **Navegación Histórica:** Botones Atrás/Adelante reales para no perderte en tu grafo de conocimiento.

### ⚡ Motor Nativo
- **Smart Paste:** Pega imágenes desde el portapapeles o la web; se guardan localmente y se optimizan automáticamente.
- **Búsqueda Full-Text:** Encuentra cualquier texto dentro de tus notas en milisegundos gracias al indexado en memoria.
- **Persistencia de Sesión:** Cierra la app y vuélvela a abrir; estarás exactamente en el mismo archivo y carpeta.

### 🛡️ Seguridad
- **SafeSave:** Escritura atómica (`QSaveFile`) que previene la corrupción de datos.
- **File Guard:** Alerta preventiva al intentar abrir archivos masivos (>5MB).

---

## 🛠️ Guía de Compilación (Build)

MadNotesCpp utiliza **CMake**, lo que garantiza una compilación idéntica en Windows y Linux.

### 🐧 Linux (Arch / Manjaro)

El código "vuela" en Linux. Sigue estos pasos para compilarlo nativamente:

1.  **Instalar Dependencias:**
    \`\`\`bash
    sudo pacman -S --needed base-devel cmake qt6-base qt6-5compat qt6-svg git
    \`\`\`

2.  **Compilar:**
    \`\`\`bash
    # Clonar repo
    git clone https://github.com/martdumo/madnotescpp.git
    cd madnotescpp

    # Configurar y Construir
    mkdir build && cd build
    cmake ..
    make -j$(nproc)
    \`\`\`

3.  **Ejecutar:**
    \`\`\`bash
    ./MadNotesCpp
    \`\`\`

### 🪟 Windows 11 (Visual Studio 2022)

1.  **Requisitos:** Tener instalado Visual Studio con el workload "Desarrollo para el escritorio con C++" y **Qt 6** (vía instalador online o vcpkg).
2.  **Abrir Proyecto:** Abre la carpeta raíz en Visual Studio. CMake debería autodetectarse.
3.  **Configurar:** Selecciona \`x64-Release\` en la barra superior.
4.  **Compilar:** Presiona \`Ctrl + Shift + B\`.
5.  **Ejecutable:** Lo encontrarás en \`out/build/x64-Release/MadNotesCpp.exe\`.

---

## 📦 Despliegue (Release)

Para generar un instalador o una versión portable en Windows:

1.  Asegúrate de compilar en **Release**.
2.  Ejecuta el script de automatización incluido:
    \`\`\`powershell
    .\deploy_windows.ps1
    \`\`\`
3.  Esto generará una carpeta \`MadNotes_Portable\` con todas las DLLs necesarias.
4.  (Opcional) Usa el script \`setup_script.iss\` con **Inno Setup** para crear el instalador \`.exe\`.

---

## 🤖 Metodología: "Vibecoding"

Este proyecto fue desarrollado utilizando una arquitectura asistida por IA (Vibe Architect), siguiendo principios estrictos:
- **Modularidad:** Componentes desacoplados (`VaultManager`, `DocumentArchitect`).
- **Iteración Atómica:** Ciclos de desarrollo cortos con verificación de compilación constante.
- **Sinceridad Técnica:** Priorización de funcionalidad robusta sobre complejidad innecesaria.

## ✍️ Autor

Desarrollado por **Martín Dumont** (`@martdumo`).

*License: MIT*