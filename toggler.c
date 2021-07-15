#include "names.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

static int setContents(const char *filename, const char *contents)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Unable to open file %s for writing (%s)\n", filename, strerror(errno));
        return 0;
    }
    int ret = (fwrite(contents, sizeof(char), strlen(contents), fp) == strlen(contents));
    fclose(fp);
    return ret;
}
static int compareContents(const char *filename, const char *expected)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Unable to open file %s for reading (%s)\n", filename, strerror(errno));
        return 0;
    }

    char *line = NULL;
    size_t len;
    ssize_t readLen = getline(&line, &len, fp);
    fclose(fp);
    if ((ssize_t)len + 1 < readLen) {
        fprintf(stderr, "differing line size %lu %lu\n", len, readLen);
        free(line);
        return 0;
    }
    const int ret = strncmp(line, expected, strlen(expected)) == 0;
    free(line);
    return ret;
}
static const char *cpufreqPath = "/sys/devices/system/cpu/cpufreq";

static int updateDir(const struct dirent *entry, const char *filename, const char *content)
{
    if (entry->d_type != DT_DIR) {
        return 1;
    }
    if (entry->d_name[0] == '.') {
        return 1;
    }

    char path[1024]; // I'm lazy, just give us some space
    memset(path, 0, sizeof path);
    snprintf(path, sizeof path, "%s/%s/%s", cpufreqPath, entry->d_name, filename);

    if (!compareContents(path, content)) {
        if (!setContents(path, content)) {
            perror("Failed to set content");
            return 0;
        }
    }

    return 1;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    DIR *dir = opendir(cpufreqPath);
    if (!dir) {
        perror("Failed to open dir to set governor");
        return 1;
    }

    struct dirent *ent;
    static const char *governorFile = "scaling_governor";
    static const char *powersave = "powersave\n";
    while ((ent = readdir(dir))) {
        if (!updateDir(ent, governorFile, powersave)) {
            closedir(dir);
            return 1;
        }
    }
    closedir(dir);

    dir = opendir(cpufreqPath);
    if (!dir) {
        perror("Failed to open dir to set preference");
        return 1;
    }

    const char *toSet = NULL;

    if (compareContents(s_prefPath, s_balancedPerformance) || compareContents(s_prefPath, s_performance)) {
        toSet = s_power;
    } else if (compareContents(s_prefPath, s_power)) {
        toSet = s_performance;
    } else {
        toSet = s_balancedPower;
        fprintf(stderr, "Unknown current preference, setting %s\n", toSet);
    }

    while ((ent = readdir(dir))) {
        if (!updateDir(ent, s_prefFile, toSet)) {
            closedir(dir);
            return 1;
        }
    }

    closedir(dir);

    printf("%s", toSet);

    return 0;
}
