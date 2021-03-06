#include <cstdio>

#include "TES3AudioController.h"

namespace TES3 {
	const auto TES3_AudioController_changeMusicTrack = reinterpret_cast<void(__thiscall*)(AudioController*, const char*, int, float)>(0x403AC0);
	void AudioController::changeMusicTrack(const char* filename, int crossfadeMillis, float volume) {
		TES3_AudioController_changeMusicTrack(this, filename, crossfadeMillis, volume);
	}

	const auto TES3_AudioController_setMusicVolume = reinterpret_cast<void(__thiscall*)(AudioController*, float)>(0x403A10);
	void AudioController::setMusicVolume(float volume) {
		TES3_AudioController_setMusicVolume(this, volume);
	}

	float AudioController::getMixVolume(AudioMixType mix) {
		float volume = 0.004 * volumeMaster;
		switch (mix) {
		case AudioMixType::Master:
			break;
		case AudioMixType::Voice:
			volume *= 0.004 * volumeVoice;
			break;
		case AudioMixType::Effects:
			volume *= 0.004 * volumeEffects;
			break;
		case AudioMixType::Footsteps:
			volume *= 0.004 * volumeFootsteps;
			break;
		case AudioMixType::Music:
			// Music is not linked to master volume
			volume = volumeMusic;
			break;
		}
		return volume;
	}

	float AudioController::getNormalizedMasterVolume() {
		return 0.004f * volumeMaster;
	}

	void AudioController::setNormalizedMasterVolume(float value) {
		volumeMaster = std::fminf(std::fmaxf(value, 0.0f), 1.0f) * 250;
	}

	float AudioController::getNormalizedEffectsVolume() {
		return 0.004f * volumeEffects;
	}

	void AudioController::setNormalizedEffectsVolume(float value) {
		volumeEffects = std::fminf(std::fmaxf(value, 0.0f), 1.0f) * 250;
	}

	float AudioController::getNormalizedVoiceVolume() {
		return 0.004f * volumeVoice;
	}

	void AudioController::setNormalizedVoiceVolume(float value) {
		volumeVoice = std::fminf(std::fmaxf(value, 0.0f), 1.0f) * 250;
	}

	float AudioController::getNormalizedFootstepsVolume() {
		return 0.004f * volumeFootsteps;
	}

	void AudioController::setNormalizedFootstepsVolume(float value) {
		volumeFootsteps = std::fminf(std::fmaxf(value, 0.0f), 1.0f) * 250;
	}

	float AudioController::getMusicVolume() {
		return volumeMusic;
	}

	const char* AudioController::getCurrentMusicFilePath() {
		return currentMusicFilePath;
	}

	void AudioController::setCurrentMusicFilePath(const char* path) {
		size_t newLength = strlen(path) + 1;
		if (newLength > 260) {
			throw std::invalid_argument("Given path is longer than 260 characters.");
		}

		strncpy_s(currentMusicFilePath, path, newLength);
	}

	const char* AudioController::getNextMusicFilePath() {
		return nextMusicFilePath;
	}

	void AudioController::setNextMusicFilePath(const char* path) {
		size_t newLength = strlen(path) + 1;
		if (newLength > 260) {
			throw std::invalid_argument("Given path is longer than 260 characters.");
		}

		strncpy_s(nextMusicFilePath, path, newLength);
	}

	double AudioController::getMusicDuration() {
		IMediaPosition * positioning;
		if (musicGraph->QueryInterface(IID_IMediaPosition, (LPVOID*)&positioning) < 0) {
			throw std::runtime_error("Music Error: Could not query IMediaPosition interface.");
		}

		REFTIME duration;
		if (positioning->get_Duration(&duration) < 0) {
			positioning->Release();
			throw std::runtime_error("Music Error: Could not fetch media duration.");
		}

		positioning->Release();
		return duration;
	}

	double AudioController::getMusicPosition() {
		IMediaPosition * positioning;
		if (musicGraph->QueryInterface(IID_IMediaPosition, (LPVOID*)&positioning) < 0) {
			throw std::runtime_error("Music Error: Could not query IMediaPosition interface.");
		}

		REFTIME position;
		if (positioning->get_CurrentPosition(&position) < 0) {
			positioning->Release();
			throw std::runtime_error("Music Error: Could not fetch media position.");
		}

		positioning->Release();
		return position;
	}

	void AudioController::setMusicPosition(double position) {
		if (position < 0.0) {
			position = 0.0;
		}

		IMediaPosition * positioning;
		if (musicGraph->QueryInterface(IID_IMediaPosition, (LPVOID*)&positioning) < 0) {
			throw std::runtime_error("Music Error: Could not query IMediaPosition interface.");
		}

		REFTIME duration;
		if (positioning->get_Duration(&duration) < 0) {
			positioning->Release();
			throw std::runtime_error("Music Error: Could not fetch media duration.");
		}

		if (position > duration) {
			positioning->Release();
			return;
		}

		if (positioning->put_CurrentPosition(position) < 0) {
			positioning->Release();
			throw std::runtime_error("Music Error: Failed to put current media position.");
		}

		positioning->Release();
	}

}