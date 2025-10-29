package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"os"
	"time"
)

func countNewlines(buffer []byte) uint64 {
	count := uint64(0)
	i := 0

	// 8바이트씩 처리
	for i+8 <= len(buffer) {
		chunk := binary.LittleEndian.Uint64(buffer[i : i+8])

		// SWAR 알고리즘
		x := chunk ^ 0x0A0A0A0A0A0A0A0A
		x = (x - 0x0101010101010101) & ^x & 0x8080808080808080
		count += ((x >> 7) & 0x0101010101010101) * 0x0101010101010101 >> 56

		i += 8
	}

	// 남은 바이트
	for i < len(buffer) {
		if buffer[i] == '\n' {
			count++
		}
		i++
	}

	return count
}

func main() {
	flag.Parse()
	args := flag.Args()

	if len(args) < 1 {
		fmt.Fprintf(os.Stderr, "Usage: %s <file>\n", os.Args[0])
		os.Exit(1)
	}

	file, err := os.Open(args[0])
	if err != nil {
		fmt.Fprintf(os.Stderr, "Cannot open file: %v\n", err)
		os.Exit(1)
	}
	defer file.Close()

	// 파일 크기
	fileInfo, err := file.Stat()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Cannot stat file: %v\n", err)
		os.Exit(1)
	}
	fileSize := fileInfo.Size()
	fmt.Printf("File size: %d bytes\n", fileSize)

	start := time.Now()

	const bufferSize = 64 * 1024 * 1024  // 64MB
	buffer := make([]byte, bufferSize)

	total := uint64(0)

	for {
		n, err := file.Read(buffer)
		if err != nil && err.Error() != "EOF" {
			fmt.Fprintf(os.Stderr, "Read error: %v\n", err)
			os.Exit(1)
		}

		if n == 0 {
			break
		}

		total += countNewlines(buffer[:n])
	}

	elapsed := time.Since(start)
	elapsedMs := elapsed.Seconds() * 1000

	fmt.Printf("Line count: %d\n", total)
	fmt.Printf("Time: %.2f ms\n", elapsedMs)
}