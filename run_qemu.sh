#!/bin/bash
echo "[-] You can abort via Ctrl-a followed by x ..."
sleep 1
qemu-system-arm -M versatilepb -nographic -m 128 -kernel image.bin
