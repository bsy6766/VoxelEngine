#ifndef LOCALIZATION_TAG_H
#define LOCALIZATION_TAG_H

// cpp
#include <string>

namespace Voxel
{
	namespace Localization
	{
		enum class Tag
		{
			en_US = 0,			// English - United States (default)
			// Planned localization
			ko_KR,				// Korean - South Korea
			ja_JP,				// Japanese - Japan
			zh_CHT,				// Chinese (Traditional)

		};

		static std::string toString(const Tag localizationTag)
		{
			switch (localizationTag)
			{
			case Voxel::Localization::Tag::en_US:
				return "en-US";
			case Voxel::Localization::Tag::ko_KR:
				return "ko-KR";
			default:
				return "";
				break;
			}
		}

		static Voxel::Localization::Tag toTag(const std::string& localizationTagStr)
		{
			if (localizationTagStr == "en-US")
			{
				return Voxel::Localization::Tag::en_US;
			}
			else if (localizationTagStr == "ko-KR")
			{
				return Voxel::Localization::Tag::ko_KR;
			}
			else
			{
				return Voxel::Localization::Tag::en_US;
			}
		}
	}
}

#endif