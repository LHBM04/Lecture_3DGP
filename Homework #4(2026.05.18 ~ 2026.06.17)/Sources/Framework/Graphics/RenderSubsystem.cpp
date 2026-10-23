#include "Precompiled.hpp"
#include "RenderSubsystem.hpp"

namespace
{
	/** DXGI Factory */
	IDXGIFactory6* dxgiFactory = nullptr;
}

namespace TUK::Framework
{
	RenderSubsystem::RenderSubsystem() noexcept
		: EngineSubsystem(100)
		, device(std::nullopt)
	{
	}

	RenderSubsystem::~RenderSubsystem() noexcept
	{
		
	}

	SystemStatus RenderSubsystem::OnStartup()
	{
		// GPU 디바이스 초기화
		if (auto expected = InitializeGPUDevice(); !expected)
		{
			std::cerr << "Render Subsystem Startup 실패: " << expected.error() << std::endl;
			return SystemStatus::Failure;
		}




		return SystemStatus::Continue;
	}

	SystemStatus RenderSubsystem::OnPreTick()
	{
		
	}

	SystemStatus RenderSubsystem::OnPostTick()
	{
		return SystemStatus::Continue;
	}

	void RenderSubsystem::OnShutdown()
	{
	}

	std::expected<void, std::string> RenderSubsystem::InitializeGPUDevice()
	{
		UINT factoryFlags = 0;

#ifdef _DEBUG
		ID3D12Debug* debugController = nullptr;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			debugController->Release();
		}
#endif

		if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&dxgiFactory))))
		{
			return std::unexpected("DXGI Factory 생성 실패!");
		}

		for (UINT adapterIndex = 0; ; ++adapterIndex)
		{
			IDXGIAdapter1* adapter = nullptr;
			if (FAILED(dxgiFactory->EnumAdapters1(adapterIndex, &adapter)))
			{
				break; // 더 이상 어댑터가 없음
			}

			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				adapter->Release();
				continue; // 소프트웨어 어댑터는 건너뜀
			}

			ID3D12Device* d3dDevice = nullptr;
			if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&d3dDevice))))
			{
				device.emplace(d3dDevice);
				adapter->Release();
				return {}; // 성공
			}
			
			adapter->Release();
		}

		return std::unexpected("GPU 장치 생성 실패!");
	}
}
