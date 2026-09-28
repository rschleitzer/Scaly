// load — the load generator of tools/bench/net: -c connections, each asking
// "GET / HTTP/1.1" (or -u's path) in a loop and reading the whole response (by its
// Content-Length) before the next request, for -d seconds; prints requests per
// second and the mean latency. -p N pipelines: N requests in ONE write, then
// the N responses (TechEmpower's plaintext test uses 16) — it cuts the
// client's cost per request by N, so the SERVER becomes the limit. -k=false
// opens a NEW connection per request (the accept path), closed with
// SO_LINGER 0 so that the client's ports do not pile up in TIME_WAIT. The
// same client measures every server.
package main

import (
	"bufio"
	"flag"
	"fmt"
	"net"
	"os"
	"strconv"
	"strings"
	"sync"
	"sync/atomic"
	"time"
)

var req = []byte("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n")

// one response: the status line and headers, then Content-Length bytes
func readResponse(r *bufio.Reader) error {
	length := -1
	for {
		line, err := r.ReadString('\n')
		if err != nil {
			return err
		}
		if line == "\r\n" {
			break
		}
		if strings.HasPrefix(strings.ToLower(line), "content-length:") {
			length, _ = strconv.Atoi(strings.TrimSpace(line[len("content-length:"):]))
		}
	}
	if length < 0 {
		return fmt.Errorf("no content-length")
	}
	_, err := r.Discard(length)
	return err
}

func main() {
	addr := flag.String("a", "127.0.0.1:8084", "server address")
	conns := flag.Int("c", 128, "connections")
	secs := flag.Int("d", 10, "seconds")
	keep := flag.Bool("k", true, "keep-alive (false: a new connection per request)")
	pipe := flag.Int("p", 1, "requests pipelined per round trip")
	path := flag.String("u", "/", "request path (and query)")
	flag.Parse()
	req = []byte("GET " + *path + " HTTP/1.1\r\nHost: localhost\r\n\r\n")
	batch := make([]byte, 0, len(req)**pipe)
	for i := 0; i < *pipe; i++ {
		batch = append(batch, req...)
	}

	var done, errs, nanos atomic.Int64
	deadline := time.Now().Add(time.Duration(*secs) * time.Second)
	var wg sync.WaitGroup
	for i := 0; i < *conns; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			var c net.Conn
			var r *bufio.Reader
			for time.Now().Before(deadline) {
				if c == nil {
					var err error
					c, err = net.Dial("tcp", *addr)
					if err != nil {
						errs.Add(1)
						time.Sleep(time.Millisecond)
						continue
					}
					r = bufio.NewReader(c)
					if !*keep {
						c.(*net.TCPConn).SetLinger(0)
					}
				}
				t0 := time.Now()
				if _, err := c.Write(batch); err != nil {
					errs.Add(1)
					c.Close()
					c = nil
					continue
				}
				var err error
				for k := 0; k < *pipe && err == nil; k++ {
					err = readResponse(r)
				}
				if err != nil {
					errs.Add(1)
					c.Close()
					c = nil
					continue
				}
				nanos.Add(int64(time.Since(t0)))
				done.Add(int64(*pipe))
				if !*keep {
					c.Close()
					c = nil
				}
			}
			if c != nil {
				c.Close()
			}
		}()
	}
	wg.Wait()
	n := done.Load()
	if n == 0 {
		fmt.Println("0 req/s (no responses)")
		os.Exit(1)
	}
	fmt.Printf("%.0f req/s  mean round trip %.3f ms  errors %d\n",
		float64(n)/float64(*secs), float64(nanos.Load())/float64(n/int64(*pipe))/1e6, errs.Load())
}
