# libomapi

Open Movement C-based API "OMAPI", plus bindings for .NET, Java, Node NAPI, Node FFI.


## Ports

```
# Mac OS
/dev/tty.usbserial-*
## ioreg -p IOUSB -l -b | grep -E "@|PortNum|USB Serial Number"
```

```
# Ubuntu 16
/dev/serial/by-id/usb-Newcastle_University_AX3_Composite_Device_1.7_CWA17_22529-if01

/dev/sdb1
/media/$USER/AX317_?????/
```

```
# Raspian
/dev/serial/by-id/usb-Newcastle_University_AX3_Composite_Device_1.7_CWA17_22529-if01
mount /dev/sda1 /mnt/usb
```


---

<!--

gcc -o test -I./include -Dtest_main=main ./examples/test.c -L. -lomapi -ludev -lpthread

-->
