package client

import (
	"bytes"
	"context"
	"encoding/json"
	"fmt"
	"io"
	"net/http"

	"github.com/mark3labs/mcp-go/mcp"
)

const (
	rpcURLFmt   = "https://api.golioth.io/v1/projects/%s/devices/%s/rpc"
	stateURLFmt = "https://api.golioth.io/v1/projects/%s/devices/%s/data/mcp/tools/schema"
)

// Client is a Golioth API client.
type Client struct {
	c        *http.Client
	rpcURL   string
	stateURL string
	apiKey   string
}

// New constructs a new Golioth API client.
func New(project, device, apiKey string) *Client {
	return &Client{
		c:        &http.Client{},
		rpcURL:   fmt.Sprintf(rpcURLFmt, project, device),
		stateURL: fmt.Sprintf(stateURLFmt, project, device),
		apiKey:   apiKey,
	}
}

type tool struct {
	Name string `json:"name"`
}

type toolsRes struct {
	Data map[string]tool `json:"data"`
}

// List tools supported by device by calling LightDB State API.
func (c *Client) ListTools(ctx context.Context) ([]mcp.Tool, error) {
	req, err := http.NewRequestWithContext(ctx, "GET", c.stateURL, http.NoBody)
	if err != nil {
		return nil, err
	}
	req.Header.Set("X-API-KEY", c.apiKey)
	res, err := c.c.Do(req)
	if err != nil {
		return nil, err
	}
	defer res.Body.Close()
	b, err := io.ReadAll(res.Body)
	if err != nil {
		return nil, err
	}
	if res.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("unexpected response (%d): %s", res.StatusCode, b)
	}
	var tools toolsRes
	if err := json.Unmarshal(b, &tools); err != nil {
		return nil, err
	}
	mcpTools := make([]mcp.Tool, len(tools.Data))
	i := 0
	for _, t := range tools.Data {
		mcpTools[i] = mcp.NewTool(t.Name)
		i++
	}
	return mcpTools, nil
}

// Call tool supported by device using RPC API.
func (c *Client) CallTool(ctx context.Context, name string) error {
	body := &struct {
		Method string   `json:"method"`
		Params []string `json:"params,omitempty"`
	}{
		Method: name,
	}
	jb, err := json.Marshal(body)
	if err != nil {
		return err
	}
	req, err := http.NewRequestWithContext(ctx, "POST", c.rpcURL, bytes.NewReader(jb))
	if err != nil {
		return err
	}
	req.Header.Set("X-API-KEY", c.apiKey)
	res, err := c.c.Do(req)
	if err != nil {
		return err
	}
	defer res.Body.Close()
	b, err := io.ReadAll(res.Body)
	if err != nil {
		return err
	}
	if res.StatusCode != http.StatusOK {
		return fmt.Errorf("unexpected response (%d): %s", res.StatusCode, b)
	}
	return nil
}
