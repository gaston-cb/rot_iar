# rot_iar
Sistema de posicionamiento para antena de sala de control del IAR. 

## Compilacion 
El proyecto utiliza gcc-arm-none-eabi con c/c++ SDK de raspberry pi pico 
Posicionarse con terminal en el directorio

Crear carpeta build
mkdir build 
cd build 
cmake .. 
make 

Crea los archivos binarios dentro de la carpeta build/main/* 
-   *uf2
-   *hex 
-   *elf -> debugger  
