#include "Data/ModObjectManager.h"
#include "Hooks/Hooks.h"
#include "Papyrus/Papyrus.h"
#include "Settings/INI/INISettings.h"
#include "Settings/JSON/JSONSettings.h"

static void MessageEventCallback(SKSE::MessagingInterface::Message* a_msg)
{
	static auto* jsonHolder = Settings::JSON::Holder::GetSingleton();
	if (!jsonHolder) {
		SKSE::stl::report_and_fail("Failed to get internal JSON logger."sv);
	}

	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		if (!Data::PreloadModObjects()) {
			SKSE::stl::report_and_fail(
				fmt::format("Failed to preload mod objects. Check the log at Documents/My Games/Skyrim Special Edition/{}.log for more information."sv, Plugin::NAME));
		}
		SECTION_SEPARATOR;
		logger::info("Finished startup tasks, enjoy your game!"sv);
		break;
	default:
		break;
	}
}

SKSEPluginInfo(
	.Version = Plugin::VERSION,
	.Name = Plugin::NAME,
	.Author = "Ecelli"sv,
	.RuntimeCompatibility = SKSE::VersionIndependence::AddressLibrary
)

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);

	SECTION_SEPARATOR;
	logger::info("Author: Ecelli"sv);
	logger::info("Runtime: {} ({})"sv,
		REL::Module::get().version().string(),
		REL::Module::IsVR() ? "VR"sv : REL::Module::IsAE() ? "AE"sv : "SE"sv);
	SECTION_SEPARATOR;

	logger::info("Performing startup tasks..."sv);

	if (!Settings::INI::Read()) {
		SKSE::stl::report_and_fail(
			fmt::format("Failed to load the INI settings. Check the log at (Documents/My Games/Skyrim Special Edition/{}.log for more information."sv, Plugin::NAME));
	}
	SECTION_SEPARATOR;

	if (!Settings::JSON::Preload()) {
#ifdef NDEBUG
		SKSE::stl::report_and_fail(
			fmt::format("Failed to parse configs. Check the log (Documents/My Games/Skyrim Special Edition/{}.log for more information."sv, Plugin::NAME));
#endif
	}
	SECTION_SEPARATOR;

	if (!Hooks::Install()) {
		SKSE::stl::report_and_fail(
			fmt::format("Failed to install the necessary hooks. Check the log at (Documents/My Games/Skyrim Special Edition/{}.log for more information."sv, Plugin::NAME));
	}
	SECTION_SEPARATOR;

	if (!Papyrus::RegisterFunctions()) {
		SKSE::stl::report_and_fail(
			fmt::format("Failed to register the new Papyrus functions. Check the log at (Documents/My Games/Skyrim Special Edition/{}.log for more information."sv, Plugin::NAME));
	}

	const auto messaging = SKSE::GetMessagingInterface();
	if (!messaging) {
		SKSE::stl::report_and_fail(
			fmt::format("Failed to get the SKSE messaging interface. Check the log at (Documents/My Games/Skyrim Special Edition/{}.log for more information."sv, Plugin::NAME));
	}
	messaging->RegisterListener(&MessageEventCallback);

	return true;
}
