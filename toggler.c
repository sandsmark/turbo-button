#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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
        fprintf(stderr, "Unable to open file %s (%s)\n", filename, strerror(errno));
        return 0;
    }

    char *line = NULL;
    size_t len;
    ssize_t readLen = getline(&line, &len, fp);
    fclose(fp);
    if ((ssize_t)len + 1 != readLen) {
        fprintf(stderr, "differing line size %lu %lu\n", len, readLen);
    }
    const int ret = (strncmp(line, expected, strlen(expected)) == 0);
    free(line);
    return ret;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    static const char *govFile = "/sys/devices/system/cpu/cpufreq/policy2/scaling_governor";
    static const char *powersave = "powersave\n";

    if (!compareContents(govFile, powersave)) {
        if (!setContents(govFile, powersave)) {
            perror("Failed to set powersave governor");
            return 1;
        }
    }

    static const char *prefFile = "/sys/devices/system/cpu/cpufreq/policy2/energy_performance_preference";
    static const char *prefPerformance = "balance_performance\n";
    static const char *prefPower = "balance_power\n";
    const char *toSet = NULL;
    if (compareContents(prefFile, prefPerformance)) {
        toSet = prefPower;
    } else if (compareContents(prefFile, prefPower)) {
        toSet = prefPerformance;
    } else {
        toSet = prefPower;
        fprintf(stderr, "Unknown current preference, setting %s\n", toSet);
    }
    if (!setContents(prefFile, toSet)) {
        fprintf(stderr, "Failed to set preference to %s\n", toSet);
        return 1;
    }
    // Print what is the current preference
    printf("%s", toSet);

    return 0;
}
