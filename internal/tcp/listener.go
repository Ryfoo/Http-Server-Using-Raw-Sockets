package tcp

import (
	"fmt"
	"io"
	"log"
	"net"
)

func Listen(addr string) {
	l, err := net.Listen("tcp", addr)
	if err != nil {
		log.Fatal("connection cannot be established!")
		l.Close()
	}

	for {
		// start a new connection
		conn, err := l.Accept()
		if err != nil {
			log.Fatal(err)
			fmt.Println("Error occured starting the connection :", err)
		}
		go func(c net.Conn) {
			defer c.Close()
			if _, err := io.Copy(c, c); err != nil {
				log.Println("copy error:", err)
			}

		}(conn)
	}
}
