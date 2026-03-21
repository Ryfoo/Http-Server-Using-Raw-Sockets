package main

import (
	"HTTP/internal/tcp"
	"log"
)

func main() {
	// Server configuration
	addr := ":8080"
	log.Println("Starting TCP server on", addr)

	tcp.Listen(addr)
}
