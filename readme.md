# serial-joystick-linux-module

![Badge](https://img.shields.io/badge/License-GPL-red)
![Badge](https://img.shields.io/badge/Linux%20Kernel-Module-informational)
![Badge](https://img.shields.io/badge/Status-Experimental-yellow)

## Descripción

**serial-joystick-linux-module** es un módulo del kernel de Linux diseñado para interactuar con un dispositivo joystick conectado a través de un puerto serial (`/dev/ttyS0`). El objetivo del módulo es interpretar datos seriales y exponer una interfaz básica de lectura a través de un dispositivo de carácter.

Aunque el módulo actualmente no está completamente funcional, establece la estructura básica para:

* Abrir un puerto serial al cargar el módulo
* Crear un dispositivo de carácter accesible desde el espacio de usuario
* Leer desde el dispositivo de manera no bloqueante
* Manejar apertura y cierre del dispositivo desde procesos de usuario

## Uso

> ⚠️ Este módulo no está terminado. No se espera funcionalidad completa aún.

### Compilación

Asegúrate de tener instalado el entorno necesario para compilar módulos del kernel (headers y build tools):

```bash
make
```

### Carga del módulo

```bash
sudo insmod serial-joystick-linux-module.ko
```

Verifica en los logs del kernel:

```bash
dmesg | tail
```

### Interacción

Una vez cargado, el módulo crea un dispositivo de carácter con un número de mayor asignado dinámicamente. Puedes crear un nodo de dispositivo manualmente:

```bash
sudo mknod /dev/lkm_example c <major_num> 0
```

> Reemplaza `<major_num>` con el número reportado por `dmesg`.

### Descarga del módulo

```bash
sudo rmmod serial-joystick-linux-module
```

## Estado del desarrollo

* [x] Registro del dispositivo de carácter
* [x] Lectura bloqueante
* [x] Manejo básico del puerto serial
* [ ] Interpretación de datos tipo joystick
* [ ] Comunicación con usuariospace
* [ ] Tests de integración

## Autor

Creado por [senchpimy](https://github.com/senchpimy)

