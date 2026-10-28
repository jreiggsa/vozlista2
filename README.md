# Voz Lista - plugin vocal con Assist (VST3 + Standalone)

Cadena: HPF -> low shelf (anti-proximidad) -> mud 300 Hz -> box 550 Hz -> compresor
        -> presencia 4.5k -> aire 11k -> saturacion -> de-esser relativo -> mix/salida.

El knob ASSIST mezcla de "sin procesar" al 100% del objetivo del MODO elegido.
Modos: Limpio, Pop, Rap/Trap, Rock, Podcast, Dinamico a Estudio.
32 presets (ruedita de presets + flechas < >).

## Compilar en Windows (sin saber C++)
1. Instala Visual Studio 2022 Community (workload "Desarrollo para el escritorio con C++")
   y Git y CMake (https://cmake.org/download/).
2. Abre "x64 Native Tools Command Prompt for VS 2022" en esta carpeta y ejecuta:

       cmake -B build -G "Visual Studio 17 2022"
       cmake --build build --config Release

   (la primera vez descarga JUCE, tarda unos minutos)
3. El .vst3 queda en:  build\VozLista_artefacts\Release\VST3\Voz Lista.vst3
   y se copia solo a C:\Program Files\Common Files\VST3 (ejecuta la consola como admin
   si no lo copia). El .exe esta en ...\Standalone\Voz Lista.exe

JUCE tiene licencia AGPL/comercial: para uso personal esta bien; si lo vendes necesitas licencia.
