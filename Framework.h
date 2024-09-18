#pragma once
#include "DXSample.h"
#include "Scene.h"

using Microsoft::WRL::ComPtr;

class Framework : public DXSample
{
public:
	Framework(UINT width, UINT height, std::wstring name);

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();

private:
	static const UINT FrameCount = 2;

	CD3DX12_VIEWPORT m_viewport;
	CD3DX12_RECT m_scissorRect;

	// Pipeline objects.
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12PipelineState> m_pipelineState;

	UINT m_rtvDescriptorSize;

	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;

	std::unordered_map<std::wstring, Scene> m_Scenes;

	void LoadPipeline();
	void LoadAssets();
	void PopulateCommandList();
	void BuildScenes();
	void WaitForPreviousFrame();
};

