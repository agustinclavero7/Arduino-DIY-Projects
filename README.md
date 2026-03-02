Este repositorio contiene código, esquemas y modelos de distintos proyectos que se me fueron ocurriendo en arduino. Para divertirme, aprender, hacer regalos y experimentar.

## Optimización de memoria para `04.MusicBox` (Arduino Nano)

Si el sketch no entra en un Nano y querés conservar animaciones, aplicá este orden de prioridades:

1. **Mover todos los textos a flash (`F("...")`)** para bajar uso de SRAM.
2. **Evitar `String`** en funciones de pantalla y usar `const __FlashStringHelper*`.
3. **Quitar `Serial` de producción** cuando no haga falta depurar.
4. **Revisar el tamaño de frames** (ideal: 32x32/128 bytes por frame) y evitar duplicados.
5. **Dividir assets grandes** en headers para poder medir y recortar bloques de bitmap con facilidad.

6. **Reutilizar frames repetidos**: guardar solo los bitmaps únicos y una secuencia de índices para reproducir la animación.

En este repo, el archivo `04.MusicBox/04.MusicBox.ino` ya incorpora estas optimizaciones (incluida la deduplicación de frames) para reducir memoria sin eliminar la animación principal.
