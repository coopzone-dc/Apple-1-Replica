msbasic for Apple 1 replica.

You will need the cc65 C compiler installed in order to compile this version of basic.

First download the full Github repop for MSBasic from:

https://github.com/mist64/msbasic

once you have it replace / add the files from this folder to the root folder of the download.

The make.sh file will have been replaced with the one from here, it only builds cz6502.bin, run make.sh to compile.

The defines_cz6502 is currently based on the OSI defines, but I have used others as well, for example the commodore (cbm2) one.

You need to set the CONFIG_SMALL := 1 or the image will not fit it 8k

Search for Ben Eater MS-BASIC for lots of info!

The addition files are:

bios.s contains I/O fixes to accomodate the use of CR/LF in the arduido terminal emulator

wozmon.s A copy of the original wozmon, tweaked to remove duplicate labels and fit in with the 8k basic

Be carful not to add too much extra code, only arounf 50-60 bytes free space available. 

Have fun.
