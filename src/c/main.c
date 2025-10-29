#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

uint64_t count_newlines(const uint8_t *buffer, size_t len) {
    uint64_t count = 0;
    size_t i = 0;
    
    // 8바이트씩 처리
    while (i + 8 <= len) {
        uint64_t chunk = *(const uint64_t *)(buffer + i);
        
        // SWAR 알고리즘
        uint64_t x = chunk ^ 0x0A0A0A0A0A0A0A0AUL;
        x = (x - 0x0101010101010101UL) & ~x & 0x8080808080808080UL;
        count += ((x >> 7) & 0x0101010101010101UL) * 0x0101010101010101UL >> 56;
        
        i += 8;
    }
    
    // 남은 바이트
    while (i < len) {
        if (buffer[i] == '\n') count++;
        i++;
    }
    
    return count;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }
    
    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }
    
    // 파일 크기
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    rewind(f);
    printf("File size: %ld bytes\n", file_size);
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    size_t buffer_size = 64 * 1024 * 1024;  // 64MB
    uint8_t *buffer = (uint8_t *)malloc(buffer_size);
    if (!buffer) {
        fprintf(stderr, "malloc failed\n");
        fclose(f);
        return 1;
    }
    
    uint64_t total = 0;
    size_t n;
    
    while ((n = fread(buffer, 1, buffer_size, f)) > 0) {
        total += count_newlines(buffer, n);
    }
    
    gettimeofday(&end, NULL);
    
    double elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 + 
                        (end.tv_usec - start.tv_usec) / 1000.0;
    
    printf("Line count: %lu\n", total);
    printf("Time: %.2f ms\n", elapsed_ms);
    
    free(buffer);
    fclose(f);
    return 0;
}