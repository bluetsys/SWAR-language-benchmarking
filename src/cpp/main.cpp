#include <iostream>
#include <fstream>
#include <chrono>
#include <cstring>
#include <vector>
#include <iomanip>

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
        std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
        return 1;
    }
    
    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open file: " << argv[1] << std::endl;
        return 1;
    }
    
    // 파일 크기
    file.seekg(0, std::ios::end);
    long file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::cout << "File size: " << file_size << " bytes" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    const size_t buffer_size = 64 * 1024 * 1024;  // 64MB
    std::vector<uint8_t> buffer(buffer_size);
    
    uint64_t total = 0;
    
    while (file.read(reinterpret_cast<char*>(buffer.data()), buffer_size) || file.gcount() > 0) {
        size_t n = file.gcount();
        total += count_newlines(buffer.data(), n);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
    
    std::cout << "Line count: " << total << std::endl;
    std::cout << "Time: " << std::fixed << std::setprecision(2) << elapsed_ms << " ms" << std::endl;
    
    file.close();
    return 0;
}