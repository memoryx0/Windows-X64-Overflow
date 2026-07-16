#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

int main() {
    WSADATA w;
    WSAStartup(MAKEWORD(2, 2), &w);

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(8080), .sin_addr.s_addr = INADDR_ANY };
    
    // Allow address reuse so serrver can restart instantly
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    bind(s, (struct sockaddr*)&addr, sizeof(addr));
    listen(s, SOMAXCONN);
    printf("Server listening on 8080...\n");

    while (1) {
        SOCKET c = accept(s, NULL, NULL);
        if (c != INVALID_SOCKET) {
            printf("Connection accepted.\n");
            
            // Reload the dll every time. 
            HMODULE h = LoadLibrary("vuln.dll");
            if (h) {
                typedef void (__cdecl *HandleSessionFunc)(SOCKET);
                HandleSessionFunc HandleSession = (HandleSessionFunc)GetProcAddress(h, "HandleSession");
                
                if (HandleSession) {
                    HandleSession(c);
                }
				// Unload after use
                FreeLibrary(h); 
            }
            closesocket(c);
        }
    }
    return 0;
}
