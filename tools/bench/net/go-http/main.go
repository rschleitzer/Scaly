// net/http reference: the standard library server, same response.
package main

import (
	"net/http"
	"os"
)

func main() {
	port := "8082"
	if len(os.Args) > 1 {
		port = os.Args[1]
	}
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "text/plain")
		w.Write([]byte("Hello, World!"))
	})
	http.ListenAndServe("127.0.0.1:"+port, nil)
}
