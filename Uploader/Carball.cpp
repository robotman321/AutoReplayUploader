#include "Carball.h"

#include "HttpClient.h"
#include <sstream>


Carball::Carball(std::string userAgent, void(*Log)(void *object, std::string message), void(*NotifyUploadResult)(void* object, bool result), void(*NotifyAuthResult)(void *object, bool result), void * Client)
{
	this->UserAgent = userAgent;
	this->Log = Log;
	this->NotifyUploadResult = NotifyUploadResult;
	this->NotifyAuthResult = NotifyAuthResult;
	this->Client = Client;
}

void CarballRequestComplete(PostFileRequest* ctx)
{
	auto carball = (Carball*)ctx->Requester;

	if (ctx->RequestId == 1)
	{
		carball->Log(carball->Client, "Carball::UploadCompleted with status: " + std::to_string(ctx->Status));
		if (ctx->Message.size() > 0)
		{
			carball->Log(carball->Client, ctx->Message);
		}
		if (ctx->ResponseBody.size() > 0)
		{
			carball->Log(carball->Client, ctx->ResponseBody);
		}
		carball->NotifyUploadResult(carball->Client, (ctx->Status >= 200 && ctx->Status < 300));

		DeleteFile(ctx->FilePath.c_str());

		delete ctx;
	}
}

void CarballRequestComplete(PostJsonRequest* ctx)
{
	auto carball = (Carball*)ctx->Requester;

	if (ctx->RequestId == 1)
	{
		carball->Log(carball->Client, "Carball::UploadMMRComplete with status: " + std::to_string(ctx->Status));
		if (ctx->Message.size() > 0)
		{
			carball->Log(carball->Client, ctx->Message);
		}
		if (ctx->ResponseBody.size() > 0)
		{
			carball->Log(carball->Client, ctx->ResponseBody);
		}
		carball->NotifyUploadResult(carball->Client, (ctx->Status >= 200 && ctx->Status < 300));

		delete ctx;
	}
}

void CarballRequestComplete(GetRequest* ctx)
{
	auto carball = (Carball*)ctx->Requester;

	if (ctx->RequestId == 2)
	{
		carball->Log(carball->Client, "Carball::AuthTest completed with status: " + std::to_string(ctx->Status));
		carball->NotifyAuthResult(carball->Client, ctx->Status == 200);

		delete ctx;
	}
}

void Carball::UploadReplay(std::string replayPath)
{
	if (!IsValid() || replayPath.empty())
	{
		Log(Client, "ReplayPath: " + replayPath);
		return;
	}

	std::string destPath = "./bakkesmod/data/carball/temp.replay";
	CreateDirectory("./bakkesmod/data/carball", NULL);
	bool resultOfCopy = CopyFile(replayPath.c_str(), destPath.c_str(), FALSE);

	Log(Client, "ReplayPath: " + replayPath);
	Log(Client, "DestPath: " + destPath);
	Log(Client, "File copy success: " + std::string(resultOfCopy ? "true" : "false"));

	PostFileRequest *request = new PostFileRequest();
	request->Url = AppendGetParams("https://carball.pro/api/upload", { {"visibility", *visibility} });
    request->FilePath = destPath;
	request->ParamName = "file";
	request->Headers.push_back("Authorization: " + *authKey);
	request->Headers.push_back("UserAgent: " + UserAgent);
	request->RequestComplete = &CarballRequestComplete;
	request->RequestId = 1;
	request->Requester = this;
	request->Message = "";

	PostFileAsync(request);
}

void Carball::UploadMMr(MMRData data)
{
	if (!IsValid())
	{
		return;
	}

	try {
		json json_body = data;
		std::string body = json_body.dump();

		PostJsonRequest* request = new PostJsonRequest();
		request->Url = "https://carball.pro/api/mmr";
		request->Headers.push_back("Authorization: " + *authKey);
		request->Headers.push_back("UserAgent: " + UserAgent);
		request->body = body;
		request->RequestComplete = &CarballRequestComplete;
		request->RequestId = 1;
		request->Requester = this;
		request->Message = "";

		PostJsonAsync(request);
	}
	catch (const std::exception & e) {
		Log(Client,  e.what());
		return;
	}

}

/**
* Tests the authorization key for Carball.pro
*/
void Carball::TestAuthKey()
{
	GetRequest *request = new GetRequest();
	request->Url = "https://carball.pro/api/";
	request->Headers.push_back("Authorization: " + *authKey);
	request->Headers.push_back("UserAgent: " + UserAgent);
	request->RequestComplete = &CarballRequestComplete;
	request->RequestId = 2;
	request->Requester = this;

	GetAsync(request);
}

bool Carball::IsValid()
{
	if (UserAgent.empty() || authKey->empty() || visibility->empty() )
	{
		Log(Client, "Carball::UploadReplay Parameters were empty.");
		Log(Client, "UserAgent: " + UserAgent);
		Log(Client, "AuthKey: " + *authKey);
		Log(Client, "Visibility: " + *visibility);
		return false;
	}
	return true;
}

Carball::~Carball()
{
}