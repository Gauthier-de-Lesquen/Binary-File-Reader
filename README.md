# Binary-File-Reader
*a binary file reader as xxd or hexdump*

## How to use it ?
To use the Binary File Reader you must type :
```[name you chose by compiling] [arguments] [file] ```
### the arguments can be:
```-h``` or ```--help```                         to get help \
```-x``` or ```--hexadecimal```                  to display the bytes in hexadecimal notation \
```-b``` or ```--binary```                       to display the bytes in binary notation \
```-d``` or ```--decimal```                      to display the bytes in decimal notation \
```-u [number]``` or ```--until [number]```      to display all the bytes before the byte at the specified index \
```-ul [number]``` or ```-until-line [number]``` to display all the bytes before the specified line \`
```-f (--from) [number] -t (--to) [number]```    to display all the bytes between the two specified indices \
```-ha``` or ```--hide-ascii```                  to display the binary content without the ASCII equivalents of the bytes \
