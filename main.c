#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

#define PHYS_ADDR 0x80000000
#define SIZE 0x1000000

#define IOMUX_REGS 0x30330014
#define IOMUX_REGS_SIZE 0x4000

void msleep(long milliseconds);

void msleep(long milliseconds) 
{
    struct timespec req, rem;

    // Convert milliseconds to seconds and nanoseconds
    req.tv_sec = milliseconds / 1000;         // Convert milliseconds to seconds
    req.tv_nsec = (milliseconds % 1000) * 1000000L; // Convert the remainder to nanoseconds

    while (nanosleep(&req, &rem) == -1) {
        // In case nanosleep is interrupted by a signal, sleep for the remaining time
        req = rem;
    }
}

int main() {
    int mem_fd;
    void *mapped_base;
    off_t dev_base = PHYS_ADDR;

    // Открыть /dev/mem 
    mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd == -1) {
        printf("ne otkrilos /dev/mem");
        return -1;
    }

    // Отобразить область памяти
    mapped_base = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, dev_base);
    if (mapped_base == (void *) -1) {
        printf("error alloc mem");
        close(mem_fd);
        return -1;
    }

    volatile __uint16_t* shared_data = (__uint16_t*)mapped_base;
    while(1) {
        //system("clear");
        printf("rewerse print\n");
        for(int i = 63; i >= 0; i--) {
            printf("0x%X ", shared_data[i]);
        }
        printf("\n");
        msleep(200);
    }

    // while(1) {
    //     printf("\rdata: %d         ", *shared_data);
    //     fflush(stdout);
    // }
    // Освободить ресурсы
    if (munmap(mapped_base, SIZE) == -1) {
        printf("debik");
    }
    close(mem_fd);
    return 0;
}
