#pragma once
// To ensure good cross-platform compatibility, the MCP namespace code uses standard C++ only.
// Avoid using platform-specific system APIs unless absolutely necessary.

#include "Task.h"
#include "../Message/Request.h"
#include "../Message/Response.h"
#include <memory>

namespace MCP
{
	class ProcessRequest : public MCP::CMCPTask
	{
	public:
		ProcessRequest(const std::shared_ptr<MCP::Request>& spRequest)
			: m_spRequest(spRequest)
		{
		}

		bool IsValid() const override;
		bool IsFinished() const override;
		bool IsCancelled() const override;
		int Execute() override;
		int Cancel() override;

		void SetRequest(const std::shared_ptr<MCP::Request>& spRequest);
		std::shared_ptr<MCP::Request> GetRequest() const;

	protected:
		std::shared_ptr<MCP::Request> m_spRequest;
	};

	class ProcessErrorRequest : public ProcessRequest
	{
	public:
		ProcessErrorRequest(const std::shared_ptr<MCP::Request>& spRequest)
			: ProcessRequest(spRequest)
		{

		}

		std::shared_ptr<CMCPTask> Clone() const override;
		int Execute() override;

		void SetErrorCode(int iCode);
		void SetErrorMessage(const std::string& strMessage);

	private:
		int m_iCode{ 0 };
		std::string m_strMessage;
	};

	class ProcessInitializeRequest : public ProcessRequest
	{
	public:
		ProcessInitializeRequest(const std::shared_ptr<MCP::Request>& spRequest)
			: ProcessRequest(spRequest)
		{

		}

		std::shared_ptr<CMCPTask> Clone() const override;
		int Execute() override;
	};

	class ProcessListToolsRequest : public ProcessRequest
	{
	public:
		ProcessListToolsRequest(const std::shared_ptr<MCP::Request>& spRequest)
			: ProcessRequest(spRequest)
		{

		}

		std::shared_ptr<CMCPTask> Clone() const override;
		int Execute() override;
	};

	class ProcessListResourcesRequest : public ProcessRequest
	{
	public:
		ProcessListResourcesRequest(const std::shared_ptr<MCP::Request>& spRequest)
			: ProcessRequest(spRequest)
		{

		}

		std::shared_ptr<CMCPTask> Clone() const override;
		int Execute() override;
	};

	class ProcessReadResourceRequest : public ProcessRequest
	{
	public:
		ProcessReadResourceRequest(const std::shared_ptr<MCP::Request>& spRequest)
			: ProcessRequest(spRequest)
		{

		}

		std::shared_ptr<CMCPTask> Clone() const override;
		int Execute() override;
	};

	class ProcessListPromptsRequest : public ProcessRequest
	{
	public:
		ProcessListPromptsRequest(const std::shared_ptr<MCP::Request>& spRequest)
			: ProcessRequest(spRequest)
		{

		}

		std::shared_ptr<CMCPTask> Clone() const override;
		int Execute() override;
	};

	class ProcessCallToolRequest : public ProcessRequest
	{
	public:
		ProcessCallToolRequest(const std::shared_ptr<MCP::Request>& spRequest)
			: ProcessRequest(spRequest)
		{

		}

		bool IsFinished() const override;
		bool IsCancelled() const override;
		std::shared_ptr<MCP::CallToolResult> BuildResult();
		int NotifyProgress(int iProgress, int iTotal);
		int NotifyResult(std::shared_ptr<MCP::CallToolResult> spResult);

	private:
		bool m_bFinished{ false };
		bool m_bCancelled{ false };
	};
}