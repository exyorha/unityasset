#ifndef UNITY_ASSET_WINDOWS_ERROR_H
#define UNITY_ASSET_WINDOWS_ERROR_H

#include <exception>
#include <vector>

namespace UnityAsset {
	class WindowsError final : public std::exception {
	public:
		explicit WindowsError(unsigned int error) noexcept;
		~WindowsError() noexcept override;

		WindowsError(const WindowsError& other);
		WindowsError& operator =(const WindowsError& other);

		WindowsError(WindowsError&& other) noexcept;
		WindowsError& operator =(WindowsError&& other) noexcept;

		inline unsigned int error() const { return m_error; }

		const char* what() const noexcept override;

		[[noreturn]] static void throwLastError();

	private:
		unsigned int m_error;
		mutable char m_staticErrorBuffer[64];
		mutable std::vector<char> m_dynamicErrorBuffer;
	};
}

#endif
