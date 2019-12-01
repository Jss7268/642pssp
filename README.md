# Byte for Byte Attacks with SSP and P-SSP
See https://github.com/zhilongwang/PolymorphicCanaries

To run against SSP use two process tabs with Ubuntu:
## Process 1
```
gcc vulnerable_server.c -o vuln -fstack-protector-all  -z execstack -no-pie
./vuln 8080
```

## Process 2
```
gcc attack.c -o attack
./attack 8080
```


To run against P-SSP use two process tabs with Ubuntu:
## Process 1
```
cd PolymorphicCanaries/Runtime_Environment
make
cd ../GCC_PLUGIN
make
cd ../../
gcc -fplugin=../PolymorphicCanaries/GCC_PLUGIN/PolymorphicCanaries.so vulnerable_server.c -o vuln_pssp -fstack-protector-all  -z execstack -no-pie
LD_PRELOAD=../PolymorphicCanaries/Runtime_Environment/LIBPolymorphicCanaries.so ./vuln_pssp 1234
```

## Process 2
```
gcc attack.c -o attack
./attack 1234
```

This will 

gcc -fplugin=../PolymorphicCanaries/GCC_PLUGIN/PolymorphicCanaries.so vulnerable_server.c -o vuln_pssp -fstack-protector-all  -z execstack -no-pie
gcc vulnerable_server.c -o vuln -fstack-protector-all  -z execstack -no-pie
gcc attack.c -o attack

LD_PRELOAD=../PolymorphicCanaries/Runtime_Environment/LIBPolymorphicCanaries.so ./vuln_pssp 1234
./vuln 8080
