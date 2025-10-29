use std::env;
use std::fs::File;
use std::io::{self, Read};
use std::time::Instant;

fn count_newlines(buffer: &[u8]) -> u64 {
    let mut count: u64 = 0;
    let mut i = 0;
    
    // 8바이트씩 처리
    while i + 8 <= buffer.len() {
        let mut chunk: u64 = 0;
        for j in 0..8 {
            chunk = (chunk << 8) | (buffer[i + j] as u64);
        }
        
        // SWAR 알고리즘
        let x = chunk ^ 0x0A0A0A0A0A0A0A0A;
        let x = (x.wrapping_sub(0x0101010101010101)) & !x & 0x8080808080808080;
        count += ((x >> 7) & 0x0101010101010101).wrapping_mul(0x0101010101010101) >> 56;
        
        i += 8;
    }
    
    // 남은 바이트
    while i < buffer.len() {
        if buffer[i] == b'\n' {
            count += 1;
        }
        i += 1;
    }
    
    count
}

fn main() -> io::Result<()> {
    let args: Vec<String> = env::args().collect();
    
    if args.len() < 2 {
        eprintln!("Usage: {} <file>", args[0]);
        std::process::exit(1);
    }
    
    let mut file = File::open(&args[1])?;
    let metadata = file.metadata()?;
    let file_size = metadata.len();
    println!("File size: {} bytes", file_size);
    
    let start = Instant::now();
    
    const BUFFER_SIZE: usize = 64 * 1024 * 1024;  // 64MB
    let mut buffer = vec![0u8; BUFFER_SIZE];
    
    let mut total: u64 = 0;
    
    loop {
        match file.read(&mut buffer)? {
            0 => break,
            n => total += count_newlines(&buffer[..n]),
        }
    }
    
    let elapsed_ms = start.elapsed().as_secs_f64() * 1000.0;
    
    println!("Line count: {}", total);
    println!("Time: {:.2} ms", elapsed_ms);
    
    Ok(())
}