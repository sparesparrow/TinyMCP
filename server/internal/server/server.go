package server

import (
	"context"
	"encoding/json"
	"io"
	"log/slog"
	"net/http"

	"github.com/golioth/tinymcp/server/internal/client"
	"github.com/mark3labs/mcp-go/mcp"
)

type mcpError struct {
	Code    int    `json:"code"`
	Message string `json:"message"`
	Data    any    `json:"data,omitempty"`
}

// Opt is a server option.
type Opt func(*Server)

// WithLogger sets the logger for a server.
func WithLogger(l *slog.Logger) Opt {
	return func(s *Server) {
		s.log = l
	}
}

// Server is a tinymcp server.
type Server struct {
	c   *client.Client
	log *slog.Logger
}

// New constructs a new tinymcp server.
func New(c *client.Client, opts ...Opt) *Server {
	s := &Server{
		c:   c,
		log: slog.New(slog.NewTextHandler(io.Discard, nil)),
	}
	for _, o := range opts {
		o(s)
	}
	return s
}

// Server handles MCP streamable HTTP requests.
func (s *Server) ServeHTTP(rw http.ResponseWriter, req *http.Request) {
	var res any
	status := http.StatusOK
	switch req.Method {
	case http.MethodPost:
		defer req.Body.Close()
		body, err := io.ReadAll(req.Body)
		if err != nil {
			status = http.StatusBadRequest
			res = &mcp.JSONRPCError{
				JSONRPC: mcp.JSONRPC_VERSION,
				ID:      mcp.NewRequestId(nil),
				Error: mcpError{
					Code: mcp.PARSE_ERROR,
				},
			}
			break
		}
		var mcpReq mcp.JSONRPCRequest
		if err := json.Unmarshal(body, &mcpReq); err != nil {
			status = http.StatusBadRequest
			res = &mcp.JSONRPCError{
				JSONRPC: mcp.JSONRPC_VERSION,
				ID:      mcp.NewRequestId(nil),
				Error: mcpError{
					Code: mcp.PARSE_ERROR,
				},
			}
			break
		}
		var mcpRes any
		switch mcp.MCPMethod(mcpReq.Method) {
		case mcp.MethodSetLogLevel,
			mcp.MethodResourcesList,
			mcp.MethodResourcesTemplatesList,
			mcp.MethodResourcesRead,
			mcp.MethodPromptsList,
			mcp.MethodPromptsGet:
			mcpRes = &mcpError{
				Code: mcp.METHOD_NOT_FOUND,
			}
		case mcp.MethodPing:
			mcpRes = &mcp.EmptyResult{}
		case mcp.MethodInitialize:
			var init mcp.InitializeRequest
			if err := json.Unmarshal(body, &init); err != nil {
				mcpRes = &mcpError{
					Code: mcp.INVALID_REQUEST,
				}
				break
			}
			mcpRes, err = s.handleInitialize(req.Context(), &init)
			if err != nil {
				mcpRes = &mcpError{
					Code:    mcp.INTERNAL_ERROR,
					Message: err.Error(),
				}
			}
		case mcp.MethodToolsList:
			var list mcp.ListToolsRequest
			if err := json.Unmarshal(body, &list); err != nil {
				mcpRes = &mcpError{
					Code:    mcp.INVALID_REQUEST,
					Message: err.Error(),
				}
				break
			}
			mcpRes, err = s.handleListTools(req.Context(), &list)
			if err != nil {
				mcpRes = &mcpError{
					Code:    mcp.INTERNAL_ERROR,
					Message: err.Error(),
				}
			}
		case mcp.MethodToolsCall:
			var call mcp.CallToolRequest
			if err := json.Unmarshal(body, &call); err != nil {
				mcpRes = &mcpError{
					Code:    mcp.INVALID_REQUEST,
					Message: err.Error(),
				}
				break
			}
			mcpRes, err = s.handleCallTool(req.Context(), &call)
			if err != nil {
				mcpRes = &mcpError{
					Code:    mcp.INTERNAL_ERROR,
					Message: err.Error(),
				}
			}
		default:
			mcpRes = &mcpError{
				Code:    mcp.METHOD_NOT_FOUND,
				Message: "method not found",
			}
		}
		res = &mcp.JSONRPCResponse{
			JSONRPC: mcp.JSONRPC_VERSION,
			ID:      mcpReq.ID,
			Result:  mcpRes,
		}
	case http.MethodGet, http.MethodDelete:
		rw.WriteHeader(http.StatusMethodNotAllowed)
		return
	default:
		http.NotFound(rw, req)
		return
	}

	rw.Header().Set("Content-Type", "application/json")
	rw.WriteHeader(status)
	if err := json.NewEncoder(rw).Encode(res); err != nil {
		s.log.Error("failed to write response", "error", err)
	}
}

// handleInitialize handles an MCP initialize request.
func (s *Server) handleInitialize(_ context.Context, req *mcp.InitializeRequest) (*mcp.InitializeResult, error) {
	s.log.Debug("handling initialize request", "params", req.Params)
	return &mcp.InitializeResult{
		ProtocolVersion: req.Params.ProtocolVersion,
		ServerInfo: mcp.Implementation{
			Name:    "tinymcp",
			Version: "0.0.1",
		},
		Capabilities: mcp.ServerCapabilities{
			Tools: &struct {
				ListChanged bool `json:"listChanged,omitempty"`
			}{
				ListChanged: true,
			},
		},
		Instructions: "Use this MCP Server to trigger actions on physical devices.",
	}, nil
}

// handleListTools handles an MCP list tools request.
func (s *Server) handleListTools(ctx context.Context, req *mcp.ListToolsRequest) (*mcp.ListToolsResult, error) {
	s.log.Debug("handling list tools request", "params", req.Params)
	tools, err := s.c.ListTools(ctx)
	if err != nil {
		return nil, err
	}
	return &mcp.ListToolsResult{
		Tools: tools,
	}, nil
}

// handleCallTool handles an MCP call tool request.
func (s *Server) handleCallTool(ctx context.Context, req *mcp.CallToolRequest) (*mcp.CallToolResult, error) {
	s.log.Debug("handling call tool request", "params", req.Params)
	// TODO(hasheddan): support parameters and responses.
	return &mcp.CallToolResult{
		Content: []mcp.Content{
			mcp.NewTextContent("success"),
		},
		IsError: s.c.CallTool(ctx, req.Params.Name) != nil,
	}, nil
}
