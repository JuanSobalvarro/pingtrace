#include "config.h"

void configure_firewall(int enable) 
{
    char exe_path[MAX_PATH];
    char command[1024];

    // get the full path to the running .exe
    if (GetModuleFileNameA(NULL, exe_path, MAX_PATH) == 0) {
        printf("[!] Error getting executable path.\n");
        return;
    }

    if (enable) {
        printf("[*] configuring firewall rule for: %s\n", exe_path);
        // Command to ADD a rule allowing ALL inbound traffic for this specific .exe
        snprintf(command, sizeof(command), 
            "netsh advfirewall firewall add rule name=\"PingTraceTool\" "
            "dir=in action=allow program=\"%s\" enable=yes profile=any > nul", 
            exe_path);
    } else {
        // Command to DELETE the rule (cleanup)
        snprintf(command, sizeof(command), 
            "netsh advfirewall firewall delete rule name=\"PingTraceTool\" > nul");
    }

    // Run the command
    int result = system(command);
    
    if (result == 0) 
    {
        if (enable) 
            printf("[+] Firewall rule added successfully.\n");
        else 
            printf("[-] Firewall rule removed.\n");
    } 
    else 
    {
        printf("[!] Failed to update firewall. (Are you running as Admin?)\n");
    }
}