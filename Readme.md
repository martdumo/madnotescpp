$readme = @"
# 🧳 MadNotesCpp "Pro Session Edition"

![C++](https://img.shields.io/badge/C++-17-blue?style=for-the-badge&logo=c%2B%2B) ![Qt](https://img.shields.io/badge/Qt-6.x-41CD52?style=for-the-badge&logo=qt) ![Status](https://img.shields.io/badge/Status-Beta-orange?style=for-the-badge)

**MadNotesCpp** es un editor de Markdown/HTML nativo diseñado para la velocidad. Esta versión se centra en la **Persistencia de Sesión**: el programa recuerda exactamente dónde estabas (carpeta y archivo) al cerrar, eliminando la fricción de "abrir y buscar".

---

## 🌟 Nuevas Características (v2.0)

### 1. Persistencia de Sesión Inteligente (`QSettings`)
Olvídate de configurar un "Archivo Home" manual.
- **Auto-Save de Estado:** Al cerrar la ventana, se guarda la ruta del Vault actual y el archivo que estabas editando en el Registro del Sistema.
- **Restauración Instantánea:** Al abrir la app, apareces exactamente donde lo dejaste.
- **Fallback:** Si es la primera vez, busca automáticamente `Home.html` o inicia en blanco.

### 2. Arquitectura de Zoom Global
Hemos movido la lógica de Zoom del editor (`SmartTextEdit`) a la ventana principal (`MainWindow`).
- **Shortcuts Globales:** `Ctrl +` y `Ctrl -` están registrados a nivel de aplicación, intentando capturar el evento sin importar qué panel tenga el foco.
- **Implementación:** Utiliza la API nativa de `viewport()->scale()` de Qt (aunque sigue siendo un área de mejora activa).

### 3. File Guard (Protección de Memoria)
- **Umbral de Seguridad:** Si intentas abrir un archivo > 5MB, el sistema intercepta la carga y solicita confirmación para evitar bloqueos de la UI (Main Thread freeze).

---

## 🛠️ Controles y Atajos

| Acción | Atajo | Descripción |
| :--- | :--- | :--- |
| **Nuevo** | `Ctrl + N` | Crear nota nueva en el directorio actual. |
| **Guardar** | `Ctrl + S` | Guardado atómico (SafeSave). |
| **Zoom In** | `Ctrl + +` | Aumentar escala de vista (Experimental). |
| **Zoom Out** | `Ctrl + -` | Reducir escala de vista (Experimental). |
| **Reset Zoom**| `Ctrl + 0` | Volver a tamaño original. |
| **Imagen** | `Click Derecho` | Menú contextual para redimensionar (25%, 50%, Full). |

---

## 🐛 Known Issues (Sinceridad Técnica)

Este proyecto sigue una metodología de desarrollo acelerada ("Vibe Coding"). Aunque la arquitectura Core es sólida, hay detalles de UI en pulido:

1.  **Inconsistencia del Zoom:** En algunas configuraciones de Windows/DPI, el `QTextEdit` ignora los comandos de escalado o pelean contra el CSS interno del HTML pegado.
2.  **Pegado Web:** Se prioriza la velocidad. Las imágenes copiadas directamente (Bitmap) funcionan perfecto, pero el HTML con referencias remotas complejas puede no renderizarse si no se descargan localmente.

---

## 🏗️ Estructura del Proyecto

- **src/ui/MainWindow:** Orquestador de Sesión, Menús y Acciones Globales.
- **src/editor/SmartTextEdit:** Manejo de eventos de Mouse (Links, Context Menu).
- **src/core/VaultManager:** I/O, Búsqueda Full-Text y Caché.
- **src/core/DocumentArchitect:** Buffer Dual y Guardado Seguro.

## ✍️ Autor & Metodología

Desarrollado mediante **Iteración Asistida por IA**.
El código refleja una evolución desde scripts básicos de Python hacia una arquitectura C++ modular, priorizando la funcionalidad sobre la perfección académica.

*License: MIT*
@"
$readme | Out-File -Encoding utf8 "README.md"
Write-Host "✅ README.md actualizado con la realidad del proyecto."