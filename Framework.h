#pragma once
#include "stdafx.h"
#include "Scene.h"
#include "Win32Application.h"
#include "GameTimer.h"

class Framework
{
public:
	Framework() = default;
	Framework(HINSTANCE hInstance, int nCmdShow, UINT width, UINT height, std::wstring name);

	//시발점 시발점 시발점 시발점
	virtual int Run(HINSTANCE hInstance, int nCmdShow);
	//시발점 시발점 시발점 시발점
	virtual void OnInit(HINSTANCE hInstance, int nCmdShow);
	virtual void OnUpdate(GameTimer& gTimer);
	virtual void OnRender();
	virtual void OnResize(UINT width, UINT height, bool minimized);
	virtual void OnDestroy();

	virtual void OnKeyDown(UINT8 key);
	virtual void OnKeyUp(UINT8 key);
	GameTimer& GetTimer();
	Scene& GetScene(const wstring& name);
	const wstring& GetCurrentSceneName();
	Win32Application& GetWin32App();
private:
	void GetHardwareAdapter(
		_In_ IDXGIFactory1* pFactory,
		_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter = false
	);
	void InitWnd(HINSTANCE hInstance);
	void BuildFactoryAndDevice();
	void BuildCommandQueueAndSwapChain();
	void BuildCommandListAndAllocator();
	void BuildRtvDescriptorHeap();
	void BuildRtv();
	void BuildDsvDescriptorHeap();
	void BuildDepthStencilBuffer(UINT width, UINT height);
	void BuildDsv();
	void BuildFence();

	void CalculateFrame();
	void PopulateCommandList();
	void BuildScenes(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	void WaitForPreviousFrame();

	unique_ptr<Win32Application> m_win32App;

	GameTimer m_Timer;

	// Adapter info.
	bool m_useWarpDevice;

	static const UINT FrameCount = 2;

	// Pipeline objects.
	ComPtr<IDXGIFactory4> m_factory;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	ComPtr<ID3D12Resource> m_depthStencilBuffer;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;

	UINT m_rtvDescriptorSize;
	UINT m_dsvDescriptorSize;

	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;

	unordered_map<wstring, Scene> m_scenes;
	wstring m_currentSceneName;

};

