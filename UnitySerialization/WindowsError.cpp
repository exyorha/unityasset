#include <UnityAsset/WindowsError.h>

#include <windows.h>
#include <cstdio>

namespace UnityAsset {

	WindowsError::WindowsError(unsigned int error) noexcept : m_error(error) {
		m_staticErrorBuffer[0] = 0;

	}

	WindowsError::~WindowsError() noexcept = default;

	WindowsError::WindowsError(const WindowsError& other) = default;

	WindowsError& WindowsError::operator =(const WindowsError& other) = default;

	WindowsError::WindowsError(WindowsError&& other) noexcept = default;

	WindowsError& WindowsError::operator =(WindowsError&& other) noexcept = default;

	void WindowsError::throwLastError() {
		throw WindowsError(GetLastError());
	}

	const char* WindowsError::what() const noexcept {
		if (m_staticErrorBuffer[0] != 0)
			return m_staticErrorBuffer;

		if (!m_dynamicErrorBuffer.empty())
			return m_dynamicErrorBuffer.data();

		struct FormatMessageBuf {
			FormatMessageBuf() : message(nullptr) {

			}

			~FormatMessageBuf() {
				if (message)
					LocalFree(message);
			}

			wchar_t* message;
		} message;

		auto characters = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, m_error, 0, reinterpret_cast<wchar_t*>(&message.message), 0, nullptr
		);

		if (characters == 0) {
			sprintf_s(m_staticErrorBuffer, "0x%08X\n", m_error);

			return m_staticErrorBuffer;
		}

		int result = WideCharToMultiByte(
			CP_UTF8,
			0,
			message.message,
			characters,
			m_staticErrorBuffer,
			sizeof(m_staticErrorBuffer) - 1,
			nullptr,
			nullptr
		);

		if (result) {
			m_staticErrorBuffer[result] = 0;
			return m_staticErrorBuffer;
		}

		try {
			int result =
				WideCharToMultiByte(
					CP_UTF8,
					0,
					message.message,
					characters,
					nullptr,
					0,
					nullptr,
					nullptr
				);

			if (!result) {
				throwLastError();
			}

			m_dynamicErrorBuffer.resize(result + 1);

			result = WideCharToMultiByte(
				CP_UTF8,
				0,
				message.message,
				characters,
				m_dynamicErrorBuffer.data(),
				m_dynamicErrorBuffer.size() - 1,
				nullptr,
				nullptr
			);

			if (!result) {
				throwLastError();
			}

			m_dynamicErrorBuffer[result] = 0;

			return m_dynamicErrorBuffer.data();
		}
		catch (const std::exception& e) {
			sprintf_s(m_staticErrorBuffer, "0x%08X\n", m_error);

			return m_staticErrorBuffer;
		}
	}
}
