// tokio keep-alive "Hello, World!" server: one task per connection, the same
// request scan (up to the end of each header) and the same fixed response.
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::TcpListener;

const RESP: &[u8] = b"HTTP/1.1 200 OK\r\nContent-Length: 13\r\nContent-Type: text/plain\r\n\r\nHello, World!";

fn find(h: &[u8]) -> Option<usize> {
    h.windows(4).position(|w| w == b"\r\n\r\n")
}

#[tokio::main]
async fn main() {
    let port = std::env::args().nth(1).unwrap_or_else(|| "8083".to_string());
    // a second argument "nodelay" sets TCP_NODELAY
    let nodelay = std::env::args().nth(2).is_some();
    let l = TcpListener::bind(format!("127.0.0.1:{}", port)).await.unwrap();
    loop {
        let (mut s, _) = match l.accept().await {
            Ok(x) => x,
            Err(_) => continue,
        };
        if nodelay {
            let _ = s.set_nodelay(true);
        }
        tokio::spawn(async move {
            let mut buf = vec![0u8; 4096];
            let mut have = 0;
            loop {
                let n = match s.read(&mut buf[have..]).await {
                    Ok(0) | Err(_) => return,
                    Ok(n) => n,
                };
                have += n;
                while let Some(i) = find(&buf[..have]) {
                    if s.write_all(RESP).await.is_err() {
                        return;
                    }
                    buf.copy_within(i + 4..have, 0);
                    have -= i + 4;
                }
                if have == buf.len() {
                    return;
                }
            }
        });
    }
}
