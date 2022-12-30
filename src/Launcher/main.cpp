#include <Windows.h>
#include <string>

#include <MaiSense/Launcher/Process.hpp>
#include <MaiSense/Launcher/Injector.hpp>

std::string get_client_from_launch_arguments()
{
    const auto client = "maimai_dump_.exe";

    for (int i = 0; i < __argc; i++)
    {
        if (std::string(__argv[i]) == "-exe")
        {
            if (i + 1 < __argc)
            {
                return std::string(__argv[i + 1]);
            }
        }
    }

    return client;
}

std::string get_dll_from_launch_arguments()
{
    const auto dll = "MaiSense.dll";

    for (int i = 0; i < __argc; i++)
    {
        if (std::string(__argv[i]) == "-dll")
        {
            if (i + 1 < __argc)
            {
                return std::string(__argv[i + 1]);
            }
        }
    }

    return dll;
}

int main()
{
    const std::string client = get_client_from_launch_arguments();
    auto process  = MaiSense::Process::Create(client, true);
    auto injector = MaiSense::Launcher::Injector(&process);
    auto dll = get_dll_from_launch_arguments();
    auto remote   = injector.Inject(dll);

    process.Resume();
    return 0;
}