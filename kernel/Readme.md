# Pannel issues

When booting from fel mode, the panel allways works. However, with chainload boot, the driver does not work at start. The reason is unknown, however restarting the driver helps.
The script in this folder solves the problem.

## Howto install

The sh file goes here:
```
sudo install -m 755 panel_reload.sh /usr/local/sbin/panel_reload.sh
```
The systemd file goes here:
```
/etc/systemd/system/panel-reload.service
```

To configure:

```
sudo systemctl daemon-reload
sudo systemctl enable panel-reload.service
sudo reboot
```

It is dirty but enough hours have been spent on this. An expert may fix this quickly.
