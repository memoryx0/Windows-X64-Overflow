# Windows-X64-Overflow
toy application used for blog on X64 Windows

## Compile code 

```powershell
# compile vuln dll 
cl /LD /GS- vuln.c /link /DLL /OUT:vuln.dll /IMPLIB:vuln.lib /DYNAMICBASE:NO /FIXED /BASE:0x10000000 ws2_32.lib

# compiler server
cl /GS- server.c /Fe:server.exe /link /DYNAMICBASE:NO /BASE:0x20000000 ws2_32.lib vuln.lib 
```
