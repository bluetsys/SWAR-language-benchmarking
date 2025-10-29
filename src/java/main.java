import java.io.*;
import java.nio.file.*;

public class main {
    
    static long countNewlines(byte[] buffer, int len) {
        long count = 0;
        int i = 0;
        
        // 8바이트씩 처리
        while (i + 8 <= len) {
            long chunk = 0;
            for (int j = 0; j < 8; j++) {
                chunk = (chunk << 8) | (buffer[i + j] & 0xFF);
            }
            
            // SWAR 알고리즘
            long x = chunk ^ 0x0A0A0A0A0A0A0A0AL;
            x = (x - 0x0101010101010101L) & ~x & 0x8080808080808080L;
            count += ((x >> 7) & 0x0101010101010101L) * 0x0101010101010101L >> 56;
            
            i += 8;
        }
        
        // 남은 바이트
        while (i < len) {
            if (buffer[i] == '\n') count++;
            i++;
        }
        
        return count;
    }
    
    public static void main(String[] args) throws IOException {
        if (args.length < 1) {
            System.err.println("Usage: java NewlineCounter <file>");
            System.exit(1);
        }
        
        Path path = Paths.get(args[0]);
        if (!Files.exists(path)) {
            System.err.println("File not found: " + args[0]);
            System.exit(1);
        }
        
        long fileSize = Files.size(path);
        System.out.println("File size: " + fileSize + " bytes");
        
        long startTime = System.currentTimeMillis();
        
        final int BUFFER_SIZE = 64 * 1024 * 1024;  // 64MB
        byte[] buffer = new byte[BUFFER_SIZE];
        
        long total = 0;
        
        try (FileInputStream fis = new FileInputStream(args[0])) {
            int n;
            while ((n = fis.read(buffer, 0, BUFFER_SIZE)) > 0) {
                total += countNewlines(buffer, n);
            }
        }
        
        long endTime = System.currentTimeMillis();
        double elapsedMs = endTime - startTime;
        
        System.out.println("Line count: " + total);
        System.out.printf("Time: %.2f ms%n", elapsedMs);
    }
}