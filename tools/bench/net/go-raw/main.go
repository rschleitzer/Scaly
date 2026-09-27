// Raw TCP keep-alive "Hello, World!" server: one goroutine per connection,
// reads until the end of each request header, answers with a fixed response.
// Nagle stays ON, as it is in the Scaly and tokio servers: Go sets TCP_NODELAY
// by default, and with pipelined requests that alone halved its throughput
// (every response its own packet) — measured 2026-09-27, see ../README.md.
package main

import (
	"bytes"
	"net"
	"os"
)

var resp = []byte("HTTP/1.1 200 OK\r\nContent-Length: 13\r\nContent-Type: text/plain\r\n\r\nHello, World!")

func handle(c net.Conn) {
	c.(*net.TCPConn).SetNoDelay(false)
	defer c.Close()
	buf := make([]byte, 4096)
	have := 0
	for {
		n, err := c.Read(buf[have:])
		if err != nil || n == 0 {
			return
		}
		have += n
		for {
			i := bytes.Index(buf[:have], []byte("\r\n\r\n"))
			if i < 0 {
				break
			}
			if _, err := c.Write(resp); err != nil {
				return
			}
			copy(buf, buf[i+4:have])
			have -= i + 4
		}
		if have == len(buf) {
			return
		}
	}
}

func main() {
	port := "8081"
	if len(os.Args) > 1 {
		port = os.Args[1]
	}
	l, err := net.Listen("tcp", "127.0.0.1:"+port)
	if err != nil {
		panic(err)
	}
	for {
		c, err := l.Accept()
		if err != nil {
			continue
		}
		go handle(c)
	}
}
