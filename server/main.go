package main

import (
	"fmt"
	"log/slog"
	"net/http"
	"os"
	"time"

	"github.com/golioth/tinymcp/server/internal/client"
	"github.com/golioth/tinymcp/server/internal/server"
)

func main() {
	c := client.New(
		os.Getenv("TINYMCP_PROJECT"),
		os.Getenv("TINYMCP_DEVICE"),
		os.Getenv("TINYMCP_API_KEY"),
	)
	srv := &http.Server{
		Addr:         fmt.Sprintf(":%d", 8080),
		ReadTimeout:  5 * time.Second,
		WriteTimeout: 10 * time.Second,
		IdleTimeout:  120 * time.Second,
		Handler: server.New(c,
			server.WithLogger(slog.New(slog.NewJSONHandler(os.Stdout, &slog.HandlerOptions{
				Level: slog.LevelDebug.Level(),
			})))),
	}
	if err := srv.ListenAndServe(); err != nil {
		panic(err)
	}
}
