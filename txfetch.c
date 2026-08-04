/** Rewrite of txfetch in C **
 *
 * Author: Rish <RishOnBash>
 * Date: August 4, 2026
 * License: MIT
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <sys/system_properties.h>
#define GN "\033[38;5;118m" // Green
#define GC "\033[38;5;249m" // Grey
#define RD "\033[38;5;196m" // Red
#define RT "\033[0m"        // Reset
#define khzFile "/sys/devices/system/cpu/cpufreq/policy0/cpuinfo_max_freq"

// get Shell name
char *shellName(void) {
    char *shell = getenv("SHELL");
    if (strstr(shell, "/bin/bash")) {
        return "bash";
    } else if (strstr(shell, "/bin/zsh")) {
        return "zsh";
    } else if (strstr(shell, "/bin/fish")) {
        return "fish";
    }  
    return "bash";
}

// get cpu frequency
float cpuFreq(void) {
    float Khz;
    FILE *getKhz = fopen(khzFile, "r");

    if (!getKhz) {
        dprintf(2, "cpu_freq: %s\n", strerror(errno));
        return -1;
    }

    fscanf(getKhz, "%f", &Khz);
    fclose(getKhz);
    return Khz / 1000000.0;
}

int main(void) {
    // check is Android
    if (!getenv("ANDROID_DATA")) exit(1);

    char os_ver[3];
    char device[10];
    char model[10];
    char cpu[10];

    __system_property_get("ro.build.version.release", os_ver);
    __system_property_get("ro.product.manufacturer", device);
    device[0] = toupper(device[0]);
    __system_property_get("ro.product.model", model);
    __system_property_get("ro.board.platform", cpu);

    // get arch & kernel 
    struct utsname buf;

    if (uname(&buf) < 0) {
        perror("uname");
        exit(EXIT_FAILURE);
    }
    
    // get disk stats
    struct statvfs disk;
    unsigned long gb, free, total, used;

    if (statvfs(getenv("PREFIX"), &disk) == 0) {
        gb = 1073741824;
        total = (disk.f_blocks * disk.f_frsize) / gb;
        free = (disk.f_bavail * disk.f_frsize) / gb;
        used = total - free;
    }

    // get cores
    int cores = get_nprocs_conf();

    // get RAM usage
    struct sysinfo info;

    if (sysinfo(&info) < 0) {
        perror("sysinfo");
        exit(EXIT_FAILURE);
    }

    unsigned long mb, used_ram, total_ram, free_ram;
    mb = 1048576;
    total_ram = (info.totalram * info.mem_unit) / mb;
    free_ram = (info.freeram * info.mem_unit) / mb;
    used_ram = total_ram - free_ram;

    // print output
    puts("");
    printf("   %sOS:%s      %sAndroid %s %s%s\n",
            GN, RT, GC, os_ver, buf.machine, RT);
    printf("   %sDEVICE:%s  %s%s %s%s\n",
            GN, RT, GC, device, model, RT);
    printf("   %sKERNEL:%s  %s%s%s\n",
            GN, RT, GC, buf.release, RT);
    printf("   %sCPU:%s     %s%s (%d) @ %.2f GHz%s\n",
            GN, RT, GC, cpu, cores, cpuFreq(), RT);
    printf("   %sSHELL:%s   %s%s%s\n",
            GN, RT, GC, shellName(), RT);
    printf("   %sDISK:%s    %s%lu / %lu GB%s\n",
            GN, RT, GC, used, total, RT);
    printf("   %sRAM:%s     %s%lu / %lu MB%s\n\n",
            GN, RT, GC, used_ram, total_ram, RT);
    /* To be fixed stuff
    printf("   %sUPTIME:%s  %s%lds%s\n\n",
            GN, RT, GC, info.uptime, RT);
    */

    exit(EXIT_SUCCESS);
}
