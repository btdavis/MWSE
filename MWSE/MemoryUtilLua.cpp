#include "MemoryUtilLua.h"

#include "LuaUtil.h"
#include "LuaManager.h"

#include "MemAccess.h"
#include "MemoryUtil.h"
#include "Log.h"

#include "TES3Inventory.h"
#include "TES3MagicEffectInstance.h"

namespace mwse {
	namespace lua {
		// Structure that keeps track of an overwritten C function, telling how we convert arguments before sending them to lua.
		class FunctionDefinition {
		public:
			bool thisCall = false;
			sol::protected_function thisCallConverter = sol::nil;
			bool fastCall = false;
			sol::protected_function fastCallConverter = sol::nil;
			std::vector<sol::protected_function> argumentConverters;
			sol::protected_function returnConverter = sol::nil;
		};

		// Utility map that matches a Morrowind.exe function (or calling address) to its FunctionDefinition.
		std::unordered_map<DWORD, FunctionDefinition> functionDefinitions;

		// Utility map that matches a calling address to the previous Morrowind.exe calling address.
		std::unordered_map<DWORD, DWORD> existingFunctionCalls;

		// Utility map that matches a calling address to the lua function that will be called instead.
		std::unordered_map<DWORD, sol::protected_function> luaFunctionOverrides;

		// Temporary container that holds the current argument list to be sent to the called lua function.
		std::vector<sol::object> luaFunctionArguments;

		// Fills luaFunctionArguments based on given argX parameters and what converters it expects.
		FunctionDefinition * fillLuaCallArguments(DWORD callingAddress, DWORD functionAt, DWORD ecx, DWORD edx, DWORD arg0 = 0, DWORD arg1 = 0, DWORD arg2 = 0, DWORD arg3 = 0, DWORD arg4 = 0, DWORD arg5 = 0) {
			auto stateHandle = LuaManager::getInstance().getThreadSafeStateHandle();
			sol::state& state = stateHandle.state;

			luaFunctionArguments.clear();

			// Figure out the definition based on the previous function address or calling address.
			auto definitionItt = functionDefinitions.find(functionAt);
			if (definitionItt == functionDefinitions.end()) {
				definitionItt = functionDefinitions.find(callingAddress);
				if (definitionItt == functionDefinitions.end()) {
					return nullptr;
				}
			}

			FunctionDefinition& definition = definitionItt->second;

			if (definition.thisCall) {
				luaFunctionArguments.push_back(definition.thisCallConverter(ecx));
			}

			if (definition.fastCall) {
				luaFunctionArguments.push_back(definition.fastCallConverter(edx));
			}

			auto argCount = definition.argumentConverters.size();
			if (argCount > 0) {
				luaFunctionArguments.push_back(definition.argumentConverters[0](arg0));
				if (argCount > 1) {
					luaFunctionArguments.push_back(definition.argumentConverters[1](arg1));
					if (argCount > 2) {
						luaFunctionArguments.push_back(definition.argumentConverters[2](arg2));
						if (argCount > 3) {
							luaFunctionArguments.push_back(definition.argumentConverters[3](arg3));
							if (argCount > 4) {
								luaFunctionArguments.push_back(definition.argumentConverters[4](arg4));
								if (argCount > 5) {
									luaFunctionArguments.push_back(definition.argumentConverters[5](arg5));
								}
							}
						}
					}
				}
			}

			return &definition;
		}

		// Actual dispatching function
		DWORD callGenericLuaFunctionFinal(DWORD callingAddress, FunctionDefinition * definition = nullptr) {
			auto stateHandle = LuaManager::getInstance().getThreadSafeStateHandle();
			sol::state& state = stateHandle.state;

			auto function = luaFunctionOverrides[callingAddress];
			sol::protected_function_result result = function(sol::as_args(luaFunctionArguments));
			if (!result.valid()) {
				sol::error error = result;
				log::getLog() << "Lua error encountered in injected lua function:" << std::endl << error.what() << std::endl;
				return 0;
			}

			sol::object returnValue = result;
			if (definition && definition->returnConverter != sol::nil) {
				returnValue = definition->returnConverter(returnValue);
				return returnValue.as<DWORD>();
			}
			// Guess at the return type.
			else {
				if (returnValue.is<double>()) {
					// Fix this! Need some way of defining the return type.
					return returnValue.as<double>();
				}
				else if (returnValue.is<bool>()) {
					return returnValue.as<bool>();
				}
				else if (returnValue.is<void*>()) {
					return (DWORD)returnValue.as<void*>();
				}
				else {
					return 0;
				}
			}
		}

		// ret __stdcall/__thiscall/__fastcall(this, edx)
		DWORD __fastcall callGenericLuaFunction_fastcall_0arg(DWORD ecx, DWORD edx) {
			// Get the calling address.
			byte ** asmEBP;
			__asm { mov asmEBP, ebp };
			DWORD callingAddress = DWORD(asmEBP[1] - 0x5);

			auto definition = fillLuaCallArguments(callingAddress, existingFunctionCalls[callingAddress], ecx, edx);
			return callGenericLuaFunctionFinal(callingAddress, definition);
		}

		// ret __stdcall/__thiscall/__fastcall(this, edx, arg0)
		DWORD __fastcall callGenericLuaFunction_fastcall_1arg(DWORD ecx, DWORD edx, DWORD arg0) {
			// Get the calling address.
			byte ** asmEBP;
			__asm { mov asmEBP, ebp };
			DWORD callingAddress = DWORD(asmEBP[1] - 0x5);

			auto definition = fillLuaCallArguments(callingAddress, existingFunctionCalls[callingAddress], ecx, edx, arg0);
			return callGenericLuaFunctionFinal(callingAddress, definition);
		}

		// ret __stdcall/__thiscall/__fastcall(this, edx, arg0, arg1)
		DWORD __fastcall callGenericLuaFunction_fastcall_2arg(DWORD ecx, DWORD edx, DWORD arg0, DWORD arg1) {
			// Get the calling address.
			byte ** asmEBP;
			__asm { mov asmEBP, ebp };
			DWORD callingAddress = DWORD(asmEBP[1] - 0x5);

			auto definition = fillLuaCallArguments(callingAddress, existingFunctionCalls[callingAddress], ecx, edx, arg0, arg1);
			return callGenericLuaFunctionFinal(callingAddress, definition);
		}

		// ret __stdcall/__thiscall/__fastcall(this, edx, arg0, arg1, arg2)
		DWORD __fastcall callGenericLuaFunction_fastcall_3arg(DWORD ecx, DWORD edx, DWORD arg0, DWORD arg1, DWORD arg2) {
			// Get the calling address.
			byte ** asmEBP;
			__asm { mov asmEBP, ebp };
			DWORD callingAddress = DWORD(asmEBP[1] - 0x5);

			auto definition = fillLuaCallArguments(callingAddress, existingFunctionCalls[callingAddress], ecx, edx, arg0, arg1, arg2);
			return callGenericLuaFunctionFinal(callingAddress, definition);
		}

		// ret __stdcall/__thiscall/__fastcall(this, edx, arg0, arg1, arg2, arg3)
		DWORD __fastcall callGenericLuaFunction_fastcall_4arg(DWORD ecx, DWORD edx, DWORD arg0, DWORD arg1, DWORD arg2, DWORD arg3) {
			// Get the calling address.
			byte ** asmEBP;
			__asm { mov asmEBP, ebp };
			DWORD callingAddress = DWORD(asmEBP[1] - 0x5);

			auto definition = fillLuaCallArguments(callingAddress, existingFunctionCalls[callingAddress], ecx, edx, arg0, arg1, arg2, arg3);
			return callGenericLuaFunctionFinal(callingAddress, definition);
		}

		// ret __stdcall/__thiscall/__fastcall(this, edx, arg0, arg1, arg2, arg3, arg4)
		DWORD __fastcall callGenericLuaFunction_fastcall_5arg(DWORD ecx, DWORD edx, DWORD arg0, DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4) {
			// Get the calling address.
			byte ** asmEBP;
			__asm { mov asmEBP, ebp };
			DWORD callingAddress = DWORD(asmEBP[1] - 0x5);

			auto definition = fillLuaCallArguments(callingAddress, existingFunctionCalls[callingAddress], ecx, edx, arg0, arg1, arg2, arg3, arg4);
			return callGenericLuaFunctionFinal(callingAddress, definition);
		}

		// ret __stdcall/__thiscall/__fastcall(this, edx, arg0, arg1, arg2, arg3, arg4, arg5)
		DWORD __fastcall callGenericLuaFunction_fastcall_6arg(DWORD ecx, DWORD edx, DWORD arg0, DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5) {
			// Get the calling address.
			byte ** asmEBP;
			__asm { mov asmEBP, ebp };
			DWORD callingAddress = DWORD(asmEBP[1] - 0x5);

			auto definition = fillLuaCallArguments(callingAddress, existingFunctionCalls[callingAddress], ecx, edx, arg0, arg1, arg2, arg3, arg4, arg5);
			return callGenericLuaFunctionFinal(callingAddress, definition);
		}

		void bindMWSEMemoryUtil() {
			auto stateHandle = LuaManager::getInstance().getThreadSafeStateHandle();
			sol::state& state = stateHandle.state;

			auto memory = state.create_table();

			//
			// Memory reinterpretation functions.
			//

			memory["reinterpret"] = [](sol::table params) -> sol::object {
				sol::optional<DWORD> value = params["value"];
				if (!value) {
					throw std::invalid_argument("Invalid 'value' parameter provided.");
				}

				sol::optional<std::string> as = params["as"];
				if (!as) {
					throw std::invalid_argument("Invalid 'as' parameter provided.");
				}

				auto stateHandle = LuaManager::getInstance().getThreadSafeStateHandle();
				sol::protected_function converter = stateHandle.state["mwse"]["convertTo"][as.value()];
				sol::protected_function_result result = converter(value.value());
				if (!result.valid()) {
					sol::error error = result;
					throw std::exception(error.what());
				}

				return result;
			};

			//
			// Read operations.
			//

			memory["readCallAddress"] = [](sol::table params) {
				sol::optional<DWORD> address = params["address"];
				if (!address) {
					throw std::invalid_argument("Invalid 'address' parameter provided.");
				}

				return getCallAddress(address.value());
			};

			//
			// Write operations.
			//

			memory["writeNoOperation"] = [](sol::table params) {
				sol::optional<DWORD> address = params["address"];
				if (!address) {
					throw std::invalid_argument("Invalid 'address' parameter provided.");
				}

				sol::optional<DWORD> length = params["length"];

				return genNOPUnprotected(address.value(), length.value_or(1U));
			};

			memory["writeByte"] = [](sol::table params) {
				sol::optional<DWORD> address = params["address"];
				if (!address) {
					throw std::invalid_argument("Invalid 'address' parameter provided.");
				}

				sol::optional<DWORD> byte = params["byte"];
				if (!byte) {
					throw std::invalid_argument("Invalid 'byte' parameter provided.");
				}

				writeByteUnprotected(address.value(), byte.value());
				return true;
			};

			memory["writeBytes"] = [](sol::table params) {
				sol::optional<DWORD> address = params["address"];
				if (!address) {
					throw std::invalid_argument("Invalid 'address' parameter provided.");
				}

				sol::optional<sol::table> bytes = params["bytes"];
				if (!bytes) {
					throw std::invalid_argument("Invalid 'bytes' parameter provided.");
				}

				size_t byteCount = bytes.value().size();
				BYTE * data = new BYTE[byteCount];
				for (size_t i = 0; i < byteCount; i++) {
					sol::object byte = bytes.value()[i+1];
					if (byte.is<double>()) {
						data[i] = byte.as<double>();
					}
					else {
						data[i] = 0x90;
					}
				}

				writePatchCodeUnprotected(address.value(), data, byteCount);

				delete[] data;

				return true;
			};

			memory["writeFunctionCall"] = [](sol::table params) {
				sol::optional<DWORD> address = params["address"];
				if (!address) {
					throw std::invalid_argument("Invalid 'address' parameter provided.");
				}

				auto stateHandle = LuaManager::getInstance().getThreadSafeStateHandle();
				auto& state = stateHandle.state;

				sol::optional<DWORD> previousCall = params["previousCall"];
				sol::optional<DWORD> length = params["length"];

				sol::object newCall = params["call"];
				if (newCall.is<double>()) {
					if (previousCall) {
						return genCallEnforced(address.value(), previousCall.value(), newCall.as<double>(), length.value_or(5U));
					}
					else {
						genNOPUnprotected(address.value(), length.value_or(5U));
						genCallUnprotected(address.value(), newCall.as<double>());
						return true;
					}
				}
				else if (newCall.is<sol::protected_function>()) {
					// Backup what we used to call to.
					auto destinationAddr = getCallAddress(address.value());
					DWORD definitionAddress = destinationAddr;
					if (destinationAddr != NULL) {
						existingFunctionCalls[address.value()] = destinationAddr;
					}
					else {
						definitionAddress = address.value();
					}

					// Inject the right function call.
					sol::optional<sol::table> signature = params["signature"];
					if (signature) {
						// Setup function definition for destination.
						FunctionDefinition definition;
						sol::table argumentConverters = state["mwse"]["memory"]["convertTo"];

						sol::optional<std::string> thisConverter = signature.value()["this"];
						if (thisConverter) {
							definition.thisCall = true;
							definition.thisCallConverter = argumentConverters[thisConverter.value()];
						}

						sol::optional<std::string> fastConverter = signature.value()["edx"];
						if (fastConverter) {
							definition.fastCall = true;
							definition.fastCallConverter = argumentConverters[fastConverter.value()];
						}

						size_t argumentCount = 0;
						sol::optional<sol::table> arguments = signature.value()["arguments"];
						if (arguments) {
							argumentCount = arguments.value().size();
							for (size_t i = 0; i < argumentCount; i++) {
								definition.argumentConverters.push_back(argumentConverters[arguments.value()[i + 1]]);
							}
						}

						sol::optional<std::string> returnConverter = signature.value()["returns"];
						if (returnConverter) {
							sol::table returnConverters = state["mwse"]["memory"]["convertFrom"];
							definition.returnConverter = returnConverters[returnConverter.value()];
						}

						functionDefinitions[definitionAddress] = std::move(definition);

						// Figure out what alias function to call.
						DWORD overwritingFunction = 0x0;
						switch (argumentCount) {
						case 0:
							overwritingFunction = reinterpret_cast<DWORD>(callGenericLuaFunction_fastcall_0arg);
							break;
						case 1:
							overwritingFunction = reinterpret_cast<DWORD>(callGenericLuaFunction_fastcall_1arg);
							break;
						case 2:
							overwritingFunction = reinterpret_cast<DWORD>(callGenericLuaFunction_fastcall_2arg);
							break;
						case 3:
							overwritingFunction = reinterpret_cast<DWORD>(callGenericLuaFunction_fastcall_3arg);
							break;
						case 4:
							overwritingFunction = reinterpret_cast<DWORD>(callGenericLuaFunction_fastcall_4arg);
							break;
						case 5:
							overwritingFunction = reinterpret_cast<DWORD>(callGenericLuaFunction_fastcall_5arg);
							break;
						case 6:
							overwritingFunction = reinterpret_cast<DWORD>(callGenericLuaFunction_fastcall_6arg);
							break;
						default:
							throw std::invalid_argument("No overload could be mapped for the given argument count.");
						}

						genCallUnprotected(address.value(), overwritingFunction);
					}
					else {
						functionDefinitions[address.value()] = {};
						genCallUnprotected(address.value(), reinterpret_cast<DWORD>(callGenericLuaFunction_fastcall_0arg));
					}

					// Make sure we can look back up our lua function from this address.
					luaFunctionOverrides[address.value()] = newCall.as<sol::protected_function>();

					return true;
				}

				return false;
			};

			state["mwse"]["memory"] = memory;


			//
			// Converters for various types of arguments.
			//

			auto convertTo = memory.create();
			convertTo["bool"] = [](DWORD arg) { return arg != 0; };
			convertTo["int"] = [](DWORD arg) { return *reinterpret_cast<int*>(&arg); };
			convertTo["uint"] = [](DWORD arg) { return arg; };
			convertTo["float"] = [](DWORD arg) { return *reinterpret_cast<float*>(&arg); };
			convertTo["string"] = [](DWORD arg) { return reinterpret_cast<const char*>(arg); };
			convertTo["tes3object"] = [](DWORD arg) { return makeLuaObject(reinterpret_cast<TES3::BaseObject*>(arg)); };
			convertTo["tes3mobileObject"] = [](DWORD arg) { return makeLuaObject(reinterpret_cast<TES3::MobileObject*>(arg)); };
			convertTo["tes3inventory"] = [](DWORD arg) { return reinterpret_cast<TES3::Inventory*>(arg); };
			convertTo["tes3equipmentStackIterator"] = [](DWORD arg) { return reinterpret_cast<TES3::Iterator<TES3::EquipmentStack>*>(arg); };
			convertTo["tes3equipmentStackIteratorNode"] = [](DWORD arg) { return reinterpret_cast<TES3::IteratorNode<TES3::EquipmentStack>*>(arg); };
			convertTo["tes3magicEffectInstance"] = [](DWORD arg) { return reinterpret_cast<TES3::MagicEffectInstance*>(arg); };
			memory["convertTo"] = convertTo;

			auto convertFrom = memory.create();
			convertFrom["bool"] = [](bool arg) { return static_cast<DWORD>(arg); };
			convertFrom["int"] = [](int arg) { return *reinterpret_cast<DWORD*>(&arg); };
			convertFrom["uint"] = [](unsigned int arg) { return arg; };
			convertFrom["float"] = [](float arg) { return *reinterpret_cast<DWORD*>(&arg); };
			convertFrom["tes3object"] = [](TES3::BaseObject* arg) { return reinterpret_cast<DWORD>(arg); };
			convertFrom["tes3mobileObject"] = [](TES3::MobileObject* arg) { return reinterpret_cast<DWORD>(arg); };
			convertFrom["tes3inventory"] = [](TES3::Inventory* arg) { return reinterpret_cast<DWORD>(arg); };
			convertFrom["tes3equipmentStackIterator"] = [](TES3::Iterator<TES3::EquipmentStack>* arg) { return reinterpret_cast<DWORD>(arg); };
			convertFrom["tes3equipmentStackIteratorNode"] = [](TES3::IteratorNode<TES3::EquipmentStack>* arg) { return reinterpret_cast<DWORD>(arg); };
			convertFrom["tes3magicEffectInstance"] = [](TES3::MagicEffectInstance* arg) { return reinterpret_cast<DWORD>(arg); };
			memory["convertFrom"] = convertFrom;
		}
	}
}
