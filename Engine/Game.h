//
// Game.h
//
#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Shader.h"
#include "modelclass.h"
#include "Light.h"
#include "Input.h"
#include "Camera.h"
#include "ShaderBlend.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    bool roombaTrigger;
    float incrementMovement = 0;
    float incrementSpin = 0;
    float incrementSideAngle = 0;
    float rotationCounter = 1;

    int directionChoice = 1;
#ifdef DXTK_AUDIO
    void NewAudioDevice();
#endif

    //AUDIO STUFF
    void OnNewAudioDevice() noexcept { m_retryAudio = true; }

    // Properties
    void GetDefaultSize( int& width, int& height ) const;
	
private:

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	}; 

    void Update(DX::StepTimer const& timer);
    void Render();
    void Clear();
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

	//input manager. 
	Input									m_input;
	InputCommands							m_gameInputCommands;

    // DirectXTK objects.
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;	
    std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
    std::unique_ptr<DirectX::SpriteFont>                                    m_font;
    float xChange;
	// Scene Objects
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;
	std::unique_ptr<DirectX::GeometricPrimitive>                            m_testmodel;

	//lights
	Light																	m_Light;

	//Cameras
	Camera																	m_Camera01;

	//textures 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture3;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_roombaT1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_roombaT2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_beanbagT;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_art1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_art2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_art3;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_carpetT;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_booksT;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_marble1T;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_marble2T;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_flooringT;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_wall1T;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_wall2T;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_fanCentreT;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_fanBladesT;

	//Shaders
	Shader																	m_BasicShaderPair;
	Shader2                                                                 m_BasicShaderPair2;


	ModelClass																m_Roomba;
    ModelClass																m_fanBlades;
    ModelClass																m_walls1;
	ModelClass																m_windowWall;
    ModelClass																m_beanbag;
    ModelClass																m_chair;
    ModelClass																m_decoPiece;
    ModelClass																m_table;
    ModelClass																m_decoBooks;
    ModelClass																m_fanCenter;
    ModelClass																m_frame1;
    ModelClass																m_frame2;
    ModelClass																m_frame3;
    ModelClass																m_sofa;
    ModelClass																m_carpet;
    ModelClass																m_vase;
    ModelClass																m_leaves;
	ModelClass																m_BasicModel3;



    // AUDIO STUFF 
    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;

    //std::unique_ptr<DirectX::WaveBank>                                      m_waveBank;
    //std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
    //std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
    //std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect2;

    

#ifdef DXTK_AUDIO
    uint32_t                                                                m_audioEvent;
    float                                                                   m_audioTimerAcc;

    bool                                                                    m_retryDefault;
#endif

    DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_view;
    DirectX::SimpleMath::Matrix                                             m_projection;

    //MORE AUDIO STUFF
    bool m_retryAudio;

    // AUDIO STUFF     audio declaration
    std::unique_ptr<DirectX::SoundEffect>                                   m_backgroundSound1;
    std::unique_ptr<DirectX::SoundEffect>                                   m_backgroundSound2;

    //AUDIO STUFF [PLAY SOUND]
    std::unique_ptr<std::mt19937>                                           m_random;
    float                                                                   secondLoopDelay;
    

};