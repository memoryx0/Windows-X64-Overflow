#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int length;       // Target for integer overflow bypass
    char data[1024];  
} Packet;

typedef struct {
    char name[64];
    void (*handler)(char*);
    int id;
} UserProfile;

void default_handler(char* msg) { printf("Log: %s\n", msg); }

void setup_environment() {
    char path[256];
    GetModuleFileName(NULL, path, 256);
    SetEnvironmentVariable("SRV_RUNTIME_MODE", "PRODUCTION");
    SetEnvironmentVariable("LOG_LEVEL_VERBOSE", "0");
}

int recv_line(SOCKET s, char *buf, int max) {
    int i = 0;
    char c;
    while (i < max - 1) {
        if (recv(s, &c, 1, 0) <= 0) break;
        if (c == '\n') break;
        if (c != '\r') buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}

__declspec(dllexport) void __cdecl HandleSession(SOCKET s) {
    setup_environment();
    
    char username[512]; 
    Packet pkt; 
    char inputPass[32];
    UserProfile user = { .handler = default_handler, .id = 1 };

    // send: 4-byte length + up to 1024 bytes of data
    if (recv(s, (char*)&pkt, sizeof(Packet), 0) <= 0) return;
    
    recv_line(s, inputPass, 32);

    // Bypass this check using Two's Complement integer overflow.
    if ((pkt.length + 10) <= 522) {
        memcpy(username, pkt.data, pkt.length);
    }

    if (strcmp(pkt.data, "ftp") == 0 && strcmp(inputPass, "ftpuser") == 0) {
        send(s, "LOGIN OK\n", 9, 0);
        char shellCmd[128];
        while(recv_line(s, shellCmd, 128) > 0) {
            FILE *fp = _popen(shellCmd, "r");
            if(fp) {
                char output[1024];
                while(fgets(output, sizeof(output), fp)) send(s, output, strlen(output), 0);
                _pclose(fp);
            }
        }
    } else {
        send(s, "AUTH FAILED\n", 12, 0);
    }
}
