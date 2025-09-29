package main

import (
	"context"
	"fmt"
	"log/slog"
	"net/http"
	"os"
	"time"

	"github.com/golioth/tinymcp/server/internal/client"
	"github.com/golioth/tinymcp/server/internal/server"
)

func main() {
	l := slog.New(slog.NewJSONHandler(os.Stdout, &slog.HandlerOptions{
		// TODO(hasheddan): support configurable log level.
		Level: slog.LevelDebug,
	}))
	c := client.New(
		os.Getenv("TINYMCP_PROJECT"),
		os.Getenv("TINYMCP_DEVICE"),
		os.Getenv("TINYMCP_API_KEY"),
	)
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	tools, err := c.ListTools(ctx)
	cancel()
	if err != nil {
		l.Error("Failed to connect to Golioth", "error", err)
		os.Exit(-1)
	}
	if len(tools) == 0 {
		l.Warn("No tools identified for device. Make sure to update LightDB State with tools schema.")
	} else {
		l.Info("Identified device tools.", "tools", tools)
	}
	srv := &http.Server{
		Addr:         fmt.Sprintf(":%d", 8080),
		ReadTimeout:  5 * time.Second,
		WriteTimeout: 10 * time.Second,
		IdleTimeout:  120 * time.Second,
		Handler:      server.New(c, server.WithLogger(l)),
	}
	l.Info("Starting server.", "port", 8080)
	if err := srv.ListenAndServe(); err != nil {
		panic(err)
	}
}
