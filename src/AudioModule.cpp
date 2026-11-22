#include <windows.h>
#include <mmsystem.h>

#include <iostream>
#include <fstream>
#include <string>

#include "AudioModule.h"

namespace audio {

    static std::wstring musicPath;
    static std::wstring debugLogPath;
    static bool musicInitialized = false;
    static bool musicMuted = false;
    static bool musicDeviceOpen = false;
    static DWORD lastStatusCheck = 0;
    static const wchar_t* musicAlias = L"DadahRangeMusic";

    static void writeDebugLog(const std::wstring& message) {
        if (debugLogPath.empty()) return;
        std::wofstream logFile(debugLogPath.c_str(), std::ios::app);
        if (logFile) logFile << message << L"\n";
    }

    static void reportMciError(const wchar_t* action, MCIERROR error) {
        wchar_t message[256] = {};
        mciGetErrorStringW(error, message, 256);
        std::wcerr << L"Warning: " << action << L" failed: " << message << L"\n";
        writeDebugLog(std::wstring(L"MCI failure while ") + action + L": " + message);
    }

    static void closeMusicDevice() {
        if (!musicDeviceOpen) return;
        mciSendStringW(L"stop DadahRangeMusic", nullptr, 0, nullptr);
        mciSendStringW(L"close DadahRangeMusic", nullptr, 0, nullptr);
        musicDeviceOpen = false;
        writeDebugLog(L"MCI device closed.");
    }

    static bool startMusic() {
        if (musicMuted || musicPath.empty()) return false;
        closeMusicDevice();

        const std::wstring openCommand = L"open \"" + musicPath +
            L"\" type waveaudio alias " + musicAlias;
        const MCIERROR openResult = mciSendStringW(openCommand.c_str(), nullptr, 0, nullptr);
        if (openResult != 0) {
            reportMciError(L"opening background music", openResult);
            return false;
        }
        musicDeviceOpen = true;
        writeDebugLog(L"MCI waveaudio device opened: " + musicPath);

        const MCIERROR playResult = mciSendStringW(L"play DadahRangeMusic", nullptr, 0, nullptr);
        if (playResult != 0) {
            reportMciError(L"starting background music", playResult);
            closeMusicDevice();
            return false;
        }
        wchar_t mode[64] = {};
        const MCIERROR statusResult = mciSendStringW(L"status DadahRangeMusic mode", mode, 64, nullptr);
        if (statusResult == 0) writeDebugLog(std::wstring(L"MCI play started; mode=") + mode);
        else reportMciError(L"querying background music mode", statusResult);
        return true;
    }

    void initializeMusic() {
        if (musicInitialized) return;
        musicInitialized = true;

        wchar_t executablePath[MAX_PATH] = {};
        const DWORD length = GetModuleFileNameW(nullptr, executablePath, MAX_PATH);
        if (length == 0 || length >= MAX_PATH) {
            std::cerr << "Warning: background music path could not be resolved.\n";
            return;
        }

        musicPath.assign(executablePath, length);
        const std::wstring::size_type separator = musicPath.find_last_of(L"\\/");
        if (separator == std::wstring::npos) {
            std::cerr << "Warning: background music path could not be resolved.\n";
            musicPath.clear();
            return;
        }
        musicPath.resize(separator + 1);
        debugLogPath = musicPath + L"audio_debug.log";
        std::wofstream logFile(debugLogPath.c_str(), std::ios::trunc);
        if (logFile) logFile << L"Dadah Range audio initialization started.\n";
        musicPath += L"assets\\audio\\music.wav";
        writeDebugLog(L"Resolved music path: " + musicPath);

        if (GetFileAttributesW(musicPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
            std::cerr << "Warning: background music file is missing: assets\\audio\\music.wav\n";
            return;
        }
        startMusic();
    }

    void updateMusic() {
        if (!musicDeviceOpen || musicMuted) return;
        const DWORD now = GetTickCount();
        if (now - lastStatusCheck < 250) return;
        lastStatusCheck = now;

        wchar_t mode[64] = {};
        const MCIERROR statusResult = mciSendStringW(L"status DadahRangeMusic mode", mode, 64, nullptr);
        if (statusResult != 0) {
            reportMciError(L"checking background music", statusResult);
            closeMusicDevice();
            return;
        }
        if (std::wstring(mode) == L"stopped") {
            const MCIERROR seekResult = mciSendStringW(L"seek DadahRangeMusic to start", nullptr, 0, nullptr);
            if (seekResult != 0) {
                reportMciError(L"rewinding background music", seekResult);
                closeMusicDevice();
                return;
            }
            const MCIERROR playResult = mciSendStringW(L"play DadahRangeMusic", nullptr, 0, nullptr);
            if (playResult != 0) {
                reportMciError(L"restarting background music", playResult);
                closeMusicDevice();
                return;
            }
            writeDebugLog(L"MCI playback reached the end and restarted.");
        }
    }

    void toggleMusicMute() {
        if (!musicInitialized) return;
        musicMuted = !musicMuted;
        if (musicMuted) {
            writeDebugLog(L"Music muted by M key.");
            closeMusicDevice();
        }
        else {
            writeDebugLog(L"Music unmuted by M key.");
            startMusic();
        }
    }

    void shutdownMusic() {
        writeDebugLog(L"Application audio shutdown requested.");
        closeMusicDevice();
    }

}
