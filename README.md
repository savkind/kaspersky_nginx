Созданные IPC-каналы:

* `netinit.NetInit` > `kl.VfsRamFs`

* `netinit.NetInit` > `kl.VfsNet`

* `nginx.Nginx` > `kl.VfsRamFs`

* `nginx.Nginx` > `kl.VfsNet`

* `kl.VfsNet` > `kl.drivers.BSP`

* `kl.VfsNet` > `kl.drivers.GPIO`

* `nginx.Nginx` > `kl.drivers.I2C`	

* `kl.rump.Dhcpcd` > `kl.VfsRamFs`

* `kl.rump.Dhcpcd` > `kl.VfsNet`

* `kl.VfsRamFs` > `kl.drivers.RAMDisk`



Инструкция по установке проекта:

1. Проверить наличие SDK KasperskyOS

2. Заполнить SDK_PREFIX: export SDK_PREFIX=/opt/KasperskyOS-Community-Edition-<version>

3. Запустить скрипт sudo -E /etc/nginx-kos/kos/nginx/cross-build.sh

4. Запустить скрипт sudo -E /etc/nginx-kos/kos/nginx/install.sh

5. Запустить скрипт sudo -E /etc/nginx-kos/kos/nginx/example/cross-build.sh с изменением последней строки на $SCRIPT_DIR/ && make kos-image

6. Подключить SD-карту к ПК

7. Запустить скрипт opt/KasperskyOS-Community-Edition-<version>/examples/rpi4_prepare_fs_image_rev1.5.sh

8. Запустить рекомендованный скрипт, после выполнения предыдущего

9. Скопировать образ kos-image в раздел карты boot 

10. Вставить SD-карту в распберри



Так же можно использовать приложенный образ kos-image и перебросить на карту.

P.S модуль ngx_http_smarthome_module уже вшит в nginx
