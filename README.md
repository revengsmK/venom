## VeNoM

A [Blumentals Program Protector](https://www.blumentals.net/protector/) v4.x anti protection toolkit.   
Reverse engineering proof-of-concept code.

### Screenshot & demo

![venom](img/venomscr.png) 


### Usage

There are three options you can use to bypass protected programs.

1. _get password_ - retrieves password for chosen protected program.   

2. _run program_ - simply runs protected program without asking for password.   

3. _unprotect programs_ - this option manipulates Program Protector main executable (__protect.exe__), allowing you to unprotect programs from the list without entering password. 

### How to build

Install either GCC or MSVC compiler and set PATH environment variable correctly to point to your chosen compiler, then run **build.bat** script. The script prefers GCC, but if it is not available it will try to build using MSVC.

This toolkit is intended to be compiled as a 32-bit binary, so make sure you have set your compiler to output 32-bit image.

### Tested / Affected versions

Following Program Protector versions are tested / affected:
 
**4.8, 4.11, 4.12, 4.13**


### Note on antivirus

Your antivirus software may flag this toolkit as a malware or potentially unwanted program (PUP).   
Please ignore this, and add it to your antivirus exclusion list.

### Support

[![bitcoin](https://img.shields.io/badge/donate-bitcoin-EF8E19)](bitcoin:bc1qjwedefp3xruv7qak72l89hzx5mx0c0ftq8qem7)

If you would like to see more projects like this one, please consider supporting me :)  

Donate bitcoin to the following address:

![btcqrcode](img/bcqrcode.png)

```
bc1qjwedefp3xruv7qak72l89hzx5mx0c0ftq8qem7
```