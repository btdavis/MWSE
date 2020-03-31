#include "NISourceTextureLua.h"

#include "NIObjectLua.h"

#include "sol.hpp"

#include "LuaManager.h"
#include "LuaUtil.h"

#include "NISourceTexture.h"
#include "NIRTTI.h"

namespace mwse {
	namespace lua {
		void bindNISourceTexture() {
			// Get our lua state.
			auto stateHandle = LuaManager::getInstance().getThreadSafeStateHandle();
			sol::state& state = stateHandle.state;

			// Binding for NI::SourceTexture::FormatPrefs.
			{
				// Start our usertype. We must finish this with state.set_usertype.
				auto usertypeDefinition = state.new_usertype<NI::SourceTexture::FormatPrefs>("niFormatPrefs");
				usertypeDefinition["new"] = sol::no_constructor;

				// Basic property binding.
				usertypeDefinition["alphaLayout"] = &NI::SourceTexture::FormatPrefs::alphaFormat;
				usertypeDefinition["mipLayout"] = &NI::SourceTexture::FormatPrefs::mipMapped;
				usertypeDefinition["pixelLayout"] = &NI::SourceTexture::FormatPrefs::pixelLayout;
			}

			// Binding for NI::SourceTexture.
			{
				// Start our usertype. We must finish this with state.set_usertype.
				auto usertypeDefinition = state.new_usertype<NI::SourceTexture>("niSourceTexture");
				usertypeDefinition["new"] = sol::no_constructor;

				// Define inheritance structures. These must be defined in order from top to bottom. The complete chain must be defined.
				usertypeDefinition[sol::base_classes] = sol::bases<NI::ObjectNET, NI::Object>();
				setUserdataForNIObjectNET(usertypeDefinition);

				// Basic property binding.
				usertypeDefinition["formatPrefs"] = &NI::SourceTexture::formatPrefs;
				usertypeDefinition["isStatic"] = &NI::SourceTexture::isStatic;
				usertypeDefinition["pixelData"] = &NI::SourceTexture::pixelData;

				// Functions bound as properties.
				usertypeDefinition["fileName"] = sol::readonly_property(&NI::SourceTexture::fileName);
				usertypeDefinition["height"] = sol::readonly_property(&NI::SourceTexture::getHeight);
				usertypeDefinition["platformFileName"] = sol::readonly_property(&NI::SourceTexture::platformFileName);
				usertypeDefinition["width"] = sol::readonly_property(&NI::SourceTexture::getWidth);
				// Functions that need their results wrapped.
				usertypeDefinition.set("createFromPath", 
					[](const char* path) { 
						using FormatPrefs = NI::Texture::FormatPrefs;
						FormatPrefs prefs = { FormatPrefs::PixelLayout::PIX_DEFAULT, FormatPrefs::MipFlag::MIP_DEFAULT, FormatPrefs::AlphaFormat::ALPHA_DEFAULT };
						return NI::SourceTexture::createFromPath(path, &prefs);
					}
				);

			}
		}
	}
}
