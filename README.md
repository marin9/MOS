# M-OS
M-OS is single task operating system for Raspberry Pi Zero. 


**Features:**
* Command line interface
* Simple file system
* Text editor
* BASIC interpreter


**Command line interface:**

![MOS screenshot](https://raw.githubusercontent.com/marin9/M_OS/master/screenshot/s1.png)


**File system:**
MOS is using simple file system on external flash memory. 
Flash memory is splitted into 16 blocks of 8 kB, so every 
file can be max 8 kB and it can contains up to 16 files 
with memory capacity 128 kB.


**Text editor:**
You can run simple text editor typing 'edit' to create or 
modify file. Editor supports only four commands: move cursor 
up (Ctrl+U), move cursor down (Ctrl+D), save file to flash 
memory (Ctrl+S) and exit from  editor (ESC).

![Text editor screenshot](https://raw.githubusercontent.com/marin9/M_OS/master/screenshot/s2.png)


**Circuit:**
* Raspberry Pi Zero
* Serial: CP2102 USB 2.0 to TTL UART module 6 pin serial converter
* Flash Memory: 25LC1024 (128 MB SPI memory)

![Schematic screenshot](https://raw.githubusercontent.com/marin9/M_OS/master/screenshot/s5.png)

