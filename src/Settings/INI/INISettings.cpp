#include "INISettings.h"

#include <ClibUtil/simpleINI.hpp>
#undef max
#undef min
#undef ERROR

namespace 
{
	void write_settings(const std::string& path, std::map<std::string, std::string>& map)
	{
		CSimpleIniA ini{};
		ini.SetUnicode();
		ini.LoadFile(path.data());

		std::list<CSimpleIniA::Entry> sections{};
		ini.GetAllSections(sections);

		std::string setting{};
		std::list<CSimpleIniA::Entry> sectionKeys{};

		for (const auto& section : sections) {
			ini.GetAllKeys(section.pItem, sectionKeys);
			for (const auto& key : sectionKeys) {
				setting = fmt::format<std::string>("{}|{}"sv, section.pItem, key.pItem);
				map[setting] = ini.GetValue(section.pItem, key.pItem);
			}
		}
	}
}

namespace Settings::INI
{
	bool Read() {
		logger::info("Reading INI settings..."sv);
		auto* holder = Holder::GetSingleton();
		if (!holder) {
			logger::critical("  >Couldn't get INI settings holder."sv);
			return false;
		}
		holder->ReadSettings();
		return true;
	}

	void Holder::ReadSettings() {
		std::string iniPath = fmt::format(R"(.\Data\SKSE\Plugins\{}.ini)"sv, Plugin::NAME);
		logger::info("Reading and validating INI settings from {}.ini"sv, Plugin::NAME);

		if (!std::filesystem::exists(iniPath)) {
			logger::warn("  >INI file not found, aborting."sv);
		}

		try {
			write_settings(iniPath, _settings);
		}
		catch (std::exception& e) {
			logger::error("  > Caught {} while parsing data."sv, e.what());
		}

		logger::info("  >Read {} settings from the default INI file."sv, _settings.size());
		logger::info("  >Checking to see if there is a custom INI..."sv);
		OverrideSettings();
		DumpSettings();
	}

	void Holder::OverrideSettings() {
		std::string iniPath = fmt::format(R"(.\Data\SKSE\Plugins\{}_custom.ini)"sv, Plugin::NAME);
		if (!std::filesystem::exists(iniPath)) {
			logger::warn("  >Custom INI file not found, aborting."sv);
		}

		try {
			write_settings(iniPath, _settings);
		}
		catch (std::exception& e) {
			logger::error("  > Caught {} while parsing data."sv, e.what());
		}
	}

    void Holder::DumpSettings()
    {
		if (_settings.empty()) {
			return;
		}

		logger::info("Stored settings:"sv);
		for (const auto& [key, val] : _settings) {
			logger::info("  - {}: {}"sv, key, val);
		}
    }
}