#include "StdInc.h"
#include <ClientHttpHandler.h>

#include <ClientRegistry.h>

#include <ServerInstanceBase.h>

#include <boost/random/random_device.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

static InitFunction initFunction([]()
{
	fx::ServerInstanceBase::OnServerCreate.Connect([](fx::ServerInstanceBase* instance)
	{
		auto shVar = instance->AddVariable<bool>("sv_scriptHookAllowed", ConVar_ServerInfo, false);

		instance->GetComponent<fx::ClientMethodRegistry>()->AddHandler("initConnect", [=](std::map<std::string, std::string>& postMap, const fwRefContainer<net::HttpRequest>& request)
		{
			auto name = postMap["name"];
			auto guid = postMap["guid"];

			auto protocol = postMap["protocol"];

			if (name.empty() || guid.empty() || protocol.empty())
			{
				return json::object({ {"error", "fields missing"} });
			}

			std::string token = boost::uuids::to_string(boost::uuids::basic_random_generator<boost::random_device>()());

			json json = json::object();
			json["protocol"] = 4;
			json["sH"] = shVar->GetValue();
			json["token"] = token;
			json["netlibVersion"] = 2;

			auto clientRegistry = instance->GetComponent<fx::ClientRegistry>();

			auto client = clientRegistry->MakeClient(guid);
			client->SetName(name);
			client->SetConnectionToken(token);
			client->Touch();

			return json;
		});
	}, 50);
});
