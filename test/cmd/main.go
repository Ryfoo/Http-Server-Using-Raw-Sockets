package main

import (
	"fmt"
	"net"
	"os"
	"sync"
	"sync/atomic"
	"time"
)

const (
	HOST        = "192.168.1.23:8080"
	PATH        = "/index.html"
	CLIENTS     = 10
	REQUESTS    = 50
	CONCURRENCY = 100
)

var successCount int64
var failureCount int64

func worker(wg *sync.WaitGroup, logFile *os.File) {
	defer wg.Done()

	conn, err := net.Dial("tcp", HOST)
	if err != nil {
		atomic.AddInt64(&failureCount, 1)
		return
	}
	defer conn.Close()

	request := fmt.Sprintf("GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n", PATH, HOST)

	buffer := make([]byte, 4096)

	for i := 0; i < REQUESTS; i++ {
		start := time.Now()

		_, err := conn.Write([]byte(request))
		if err != nil {
			atomic.AddInt64(&failureCount, 1)
			return
		}

		n, err := conn.Read(buffer)
		if err != nil {
			atomic.AddInt64(&failureCount, 1)
			return
		}

		duration := time.Since(start)

		if n > 0 {
			atomic.AddInt64(&successCount, 1)

			logLine := fmt.Sprintf("SUCCESS | %v\n", duration)
			logFile.WriteString(logLine)
		} else {
			atomic.AddInt64(&failureCount, 1)
		}

		time.Sleep(10 * time.Millisecond) // pacing
	}
}

func main() {
	var wg sync.WaitGroup

	logFile, err := os.Create("load_test.log")
	if err != nil {
		panic(err)
	}
	defer logFile.Close()

	sem := make(chan struct{}, CONCURRENCY)

	startTime := time.Now()

	for i := 0; i < CLIENTS; i++ {
		wg.Add(1)

		go func() {
			sem <- struct{}{}
			worker(&wg, logFile)
			<-sem
		}()
	}

	wg.Wait()

	totalTime := time.Since(startTime)

	fmt.Println("===== RESULTS =====")
	fmt.Println("Success:", successCount)
	fmt.Println("Failure:", failureCount)
	fmt.Println("Total time:", totalTime)

	logFile.WriteString(fmt.Sprintf(
		"\nFINAL | Success: %d | Failure: %d | Time: %v\n",
		successCount, failureCount, totalTime,
	))
}
