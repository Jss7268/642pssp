# Byte for Byte Attacks with SSP and P-SSP
See https://github.com/zhilongwang/PolymorphicCanaries for information about Polymorphic canaries

## SSP
To run against SSP use two process tabs with Ubuntu:
### Process 1
```
gcc vulnerable_server.c -o vuln -fstack-protector-all  -z execstack -no-pie
./vuln 8080
```

### Process 2
```
gcc attack.c -o attack
./attack 8080
```
This should allow us to brute force the canary value in a few seconds. The canary hex value will be displayed at the end of running. If running Process 2 multiple times, the same canary value will be displayed as it never changes in Process 1 using SSP.

## P-SSP
To run against P-SSP use two process tabs with Ubuntu:
### Process 1
```
cd PolymorphicCanaries/Runtime_Environment
make
cd ../GCC_PLUGIN
make
cd ../../
gcc -fplugin=../PolymorphicCanaries/GCC_PLUGIN/PolymorphicCanaries.so vulnerable_server.c -o vuln_pssp -fstack-protector-all  -z execstack -no-pie
LD_PRELOAD=../PolymorphicCanaries/Runtime_Environment/LIBPolymorphicCanaries.so ./vuln_pssp 1234
```

### Process 2
```
gcc attack.c -o attack
./attack 1234
```

This will output: `could not get canary byte @ idx=0`. Idx shows the index where we were unable to brute force the canary value.
Because P-SSP changes the canary value after forking, we cannot guarantee we will guess the canary bytes. Therefore its possible for idx to be greater than 0, but the chance is small, and the chance is EXTREMELY small to guess the canary with this method.

Since P-SSP randomizes the shadow canary, we cannot get the canary value with this byte-for-byte approach.
