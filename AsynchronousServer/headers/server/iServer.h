#pragma once

namespace clientServerApp
{
	class iServer
	{
	public:
		iServer() = default;
		virtual ~iServer() = default;
		virtual void do_accept() = 0;
	};
}