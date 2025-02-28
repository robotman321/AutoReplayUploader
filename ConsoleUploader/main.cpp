#include "Ballchasing.h"
#include "Calculated.h"
#include "Carball.h"
#include "Utils.h"
#include "Replay.h"

void Log(void* object, std::string message)
{
	std::cout << message << std::endl;
}

void SetVariable(void* object, std::string name, std::string value)
{
	std::cout << "Set: " << name << " to: " << value << std::endl;
}

void CalculatedUploadComplete(void* object, bool result)
{
	std::cout << "Calculated upload completed with result: " << result;
}

void BallchasingUploadComplete(void* object, bool result)
{
	std::cout << "Ballchasing upload completed with result: " << result;
}

void BallchasingAuthTestComplete(void* object, bool result)
{
	std::cout << "Ballchasing authtest completed with result: " << result;
}

void CarballUploadComplete(void* object, bool result)
{
	std::cout << "Carball upload completed with result: " << result;
}

void CarballAuthTestComplete(void* object, bool result)
{
	std::cout << "Carball authtest completed with result: " << result;
}

int main()
{
	std::string filename = "979B723811E971FCE06E328BDF9F6172.replay";
	std::string replayFile = "C:/Users/tyni/Desktop/" + filename;

	Ballchasing* ballchasing = new Ballchasing("consoleuploader", &Log, &BallchasingUploadComplete, &BallchasingAuthTestComplete, NULL);
	*(ballchasing->authKey) = "test";
	*(ballchasing->visibility) = "public";
	ballchasing->UploadReplay(replayFile);

	Calculated* calculated = new Calculated("consoleuploader", &Log, &CalculatedUploadComplete, NULL);
	*(calculated->visibility) = "PUBLIC";
	calculated->UploadReplay(replayFile, "76561198011976380");

	Carball* carball = new Carball("consoleuploader", &Log, &CarballUploadComplete, &CarballAuthTestComplete, NULL);
	*(carball->authKey) = "test";
	*(carball->visibility) = "public";
	carball->UploadReplay(replayFile);

	system("PAUSE");
}