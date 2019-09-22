# M-OS
M-OS is single task operating system for Raspberry Pi Zero. 

**Features**

* Command line interface
* Simple file system
* Text editor
* BASIC interpreter

**Command line interface**

MOS don't using graphic or USB keyboard, all communication is 
over serial (UART). You need to connect pin 14 and 15 to USB 
over UART module and operate with putty. Also you can power 
RPi with UART module over pin 5V just like on circuit diagram. 
Command line interface supports few commands: basic (for start 
BASIC interpreter), edit (for start text editor), ls (for print 
all files in memory), and other commands for manipulate with files. 
Type 'help' to view all supported commands.

![MOS screenshot](https://raw.githubusercontent.com/marin9/M_OS/master/screenshot/s1.png)

**File system**

MOS is using simple file system on external flash memory. 
Flash memory is splitted into 16 blocks of 8 kB, so every 
file can be max 8 kB and it can contain up to 16 files 
with memory capacity 128 kB. File system is not supporting 
directories, only files.

**Text editor**

You can run simple text editor by typing 'edit' to create or 
modify text file. Editor supports only four commands: move 
cursor up (Ctrl+U), move cursor down (Ctrl+D), save file 
to flash memory (Ctrl+S) and exit from  editor (ESC).

![Text editor screenshot](https://raw.githubusercontent.com/marin9/M_OS/master/screenshot/s2.png)


**BASIC interpreter**

This simple version of BASIC interpreter allows you to create 
and run your programs. It supports over 20 commands for input/output,
read/write file, control GPIO pins and others. To view all supported
commands and grammar, type 'basic grammar'.
For example: blink LED program

![Blink program screenshot](https://raw.githubusercontent.com/marin9/M_OS/master/screenshot/s3.png)


**Circuit:**

* Raspberry Pi Zero
* Serial: CP2102 USB 2.0 to TTL UART module 6 pin serial converter
* Flash Memory: 25LC1024 (128 kB SPI memory)

![Schematic screenshot](https://raw.githubusercontent.com/marin9/M_OS/master/screenshot/s5.png)

