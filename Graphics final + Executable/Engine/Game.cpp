//
// Game.cpp
//

#include "pch.h"
#include "Game.h"


//toreorganise
#include <fstream>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false) :
    m_retryAudio(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

//for audio
Game::~Game()
{
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
}
// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    xChange = 0;
	m_input.Initialise(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

	//setup light
	m_Light.setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(0.0f, 2.5f, 2.5f);
	m_Light.setDirection(0.0f, -1.0f, 0.0f);

	//setup camera
	m_Camera01.setPosition(Vector3(0.0f, 5.0f, 16.0f));
	m_Camera01.setRotation(Vector3(-90.0f, -180.0f, 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up. 

   
	// AUDIO STUFF [GAME LOOP WITH AUDIO ENGINE]
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags |= AudioEngine_Debug;
#endif
    m_audEngine = std::make_unique<AudioEngine>(eflags);

    
    //AUDIO STUFF      audio  initialize
    // audio files from : https://samplefocus.com/collections/mixed-progressive-house-kit
    m_backgroundSound1 = std::make_unique<SoundEffect>(m_audEngine.get(),
        L"pleasant_bg.wav");
    m_backgroundSound2 = std::make_unique<SoundEffect>(m_audEngine.get(),
        L"pleasant_bg2.wav");

    // AUDIO STUFF PLAY SOUND
    std::random_device rd;
    m_random = std::make_unique<std::mt19937>(rd());

    secondLoopDelay = 2.0f;
#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
#endif
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	//take in input
	m_input.Update();								//update the hardware
	m_gameInputCommands = m_input.getGameInput();
    xChange = m_input.getXChange();
    //retrieve the input for our game
	
	//Update all game objects
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

	//Render all game content. 
    Render();

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

	
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float deltaTime = timer.GetElapsedSeconds();
	//note that currently.  Delta-time is not considered in the game object movement. 
	if (m_gameInputCommands.left)
	{
        Vector3 position = m_Camera01.getPosition(); //get the position
		Vector3 rotation = m_Camera01.getRotation();
		rotation.y = rotation.y += m_Camera01.getRotationSpeed()*deltaTime;
		m_Camera01.setRotation(rotation);
        //position -= (m_Camera01.getForward() * m_Camera01.getMoveSpeed() * deltaTime); //add the forward vector
        //m_Camera01.setPosition(position);
	}
	if (m_gameInputCommands.right)
	{
        Vector3 position = m_Camera01.getPosition(); //get the position
		Vector3 rotation = m_Camera01.getRotation();
		rotation.y = rotation.y -= m_Camera01.getRotationSpeed()*deltaTime;
		m_Camera01.setRotation(rotation);
        //position += (m_Camera01.getForward() * m_Camera01.getMoveSpeed() * deltaTime); //add the forward vector
        //m_Camera01.setPosition(position);
	}
	if (m_gameInputCommands.forward)
	{
		Vector3 position = m_Camera01.getPosition(); //get the position
		position += (m_Camera01.getForward()*m_Camera01.getMoveSpeed()*deltaTime); //add the forward vector
		m_Camera01.setPosition(position);
	}
	if (m_gameInputCommands.back)
	{
		Vector3 position = m_Camera01.getPosition(); //get the position
		position -= (m_Camera01.getForward()*m_Camera01.getMoveSpeed()*deltaTime); //add the forward vector
		m_Camera01.setPosition(position);
	}
    
    // move up
    if (m_gameInputCommands.moveUp) {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position.y += 0.05;
        m_Camera01.setPosition(position);
    }

    // move up
    if (m_gameInputCommands.moveDown) {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position.y -= 0.05;
        m_Camera01.setPosition(position);
    }

    /*
    if (m_gameInputCommands.rotLeft) 
    {
        Vector3 rotation = m_Camera01.getRotation();
        rotation.y = rotation.y -= xChange* deltaTime;
        m_Camera01.setRotation(rotation);
    }   
    if (m_gameInputCommands.rotRight)
    {
        m_Camera01.Update();
    }
    */

    //getting the roomba vacuum to move based on user input
    if (m_gameInputCommands.toggleRoomba) {
        if (roombaTrigger) {
            roombaTrigger = false;
        }
        else {
            roombaTrigger = true;
        }
        //Vector3 position = m_Camera01.getPosition(); //get the position
        //position.y -= 0.05;
        //m_Camera01.setPosition(position);
    }

    



	m_Camera01.Update();	//camera update.

	m_view = m_Camera01.getCameraMatrix();
	m_world = Matrix::Identity;

    //AUDIO STUFF [PLAY SOUND]
    //elapsedTime = 0.01;
    
    secondLoopDelay -= 1.0; // hardcoded elapsedTime value here to 0.5
    if (secondLoopDelay < 0.f)
    {
        m_backgroundSound1->Play();

        std::uniform_real_distribution<float> dist(1.f, 10.f);
        secondLoopDelay = dist(*m_random);
    }

    //AUDIO STUFF

    if (m_retryAudio)
    {
        m_retryAudio = false;
        if (m_audEngine->Reset())
        {
            // TODO: restart any looped sounds here
        }
    }
    else if (!m_audEngine->Update())
    {
        if (m_audEngine->IsCriticalError())
        {
            m_retryAudio = true;
        }
    }
    
#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif

  
	if (m_input.Quit())
	{
		ExitGame();
	}
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{	
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // Draw Text to the screen
    m_deviceResources->PIXBeginEvent(L"Draw sprite");
    m_sprites->Begin();
		m_font->DrawString(m_sprites.get(), L"Project Demo CMP502", XMFLOAT2(10, 10), Colors::Black);
    m_sprites->End();
    m_deviceResources->PIXEndEvent();
	
	//Set Rendering states. 
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullClockwise());
//	context->RSSetState(m_states->Wireframe());


	// Turn our shaders on,  set parameters
	


    //for sideways rotation of lightsource [quaternion]
    float sideAngle = -0.5f;
    //if (incrementSideAngle == -45 || incrementSideAngle == -135 ) { //this should handle our movement
    //    sideAngle *= -1;
    //}

    // q * p * (q-1)
    Quaternion point = { 0.0f, 13.5f, 13.5f, 1.0f }; //p
    Quaternion rotAxis = SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0, 0, 1), (incrementSideAngle/3) * (XM_PI / 180));//q
    Quaternion invertRotAxis = rotAxis; invertRotAxis.Conjugate();

    Quaternion combineCalc = SimpleMath::Quaternion::Concatenate(point, rotAxis); //q * p
    combineCalc = SimpleMath::Quaternion::Concatenate(invertRotAxis, combineCalc); //q * p * q-1

    
    m_Light.setPosition(combineCalc.x,combineCalc.y,combineCalc.z);


    //rotation matrix
    //SimpleMath::Matrix lightSpin = SimpleMath::Matrix::CreateRotationY(incrementSideAngle * (XM_PI / 180) * -1);


    float xVal = -4.5f;
    float zVal = 4.5f;
    float range = 620.0f;
    
    //m_gameInputCommands.roombaDir = 1;
    
        switch (directionChoice)
        {
        case 1://move with frames here
            xVal = 4.5f;
            zVal = 4.5f;

            if (incrementMovement < range) {
                xVal -= (incrementMovement/70);

                // if not paused
                if (roombaTrigger) {
                    incrementMovement++;
                }
                
                m_world *= SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(xVal, 0.2f, zVal));
                
            }

            else if (rotationCounter < 91) {

                
                // rotation
                m_world *= SimpleMath::Matrix::CreateRotationY(-1 * rotationCounter * (XM_PI/180));
                
                m_world *= SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-4.5f, 0.2f, 4.5f));

                rotationCounter++;
            }

            else {
                // switch to next case
                directionChoice = 2;
                incrementMovement = 0;
                m_world *= SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-4.5f, 0.2f, 4.5f));

            }
            break;

        case 2:
            xVal = -4.5f;
            zVal = 4.5f;
            
            if (incrementMovement < range) {
                zVal -= (incrementMovement / 70);

                if (roombaTrigger) {
                    incrementMovement++;
                }
                // rotation
                m_world *= SimpleMath::Matrix::CreateRotationY(-90 * (XM_PI / 180));

                m_world *= SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(xVal, 0.2f, zVal));

            }
            else if (rotationCounter < 181) {


                // rotation
                m_world *= SimpleMath::Matrix::CreateRotationY(-1 * rotationCounter * (XM_PI / 180));

                m_world *= SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-4.5f, 0.2f, -4.5f));

                rotationCounter++;
            }

            else {
                // switch to next case
                directionChoice = 3;
                incrementMovement = 0;
                m_world *= SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-4.5f, 0.2f, -4.5f));

            }
            break;
        case 3:
            xVal = -4.5f;
            zVal = -4.5f;

            if (incrementMovement < range) {
                xVal += (incrementMovement / 70);

                if (roombaTrigger) {
                    incrementMovement++;
                }
                //rotation
                m_world *= SimpleMath::Matrix::CreateRotationY(-180 * (XM_PI / 180));
                m_world *= SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(xVal, 0.2f, zVal));

            }
            else if (rotationCounter < 271) {


                // rotation
                m_world *= SimpleMath::Matrix::CreateRotationY(-1 * rotationCounter * (XM_PI / 180));

                m_world *= SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(4.5f, 0.2f, -4.5f));

                rotationCounter++;
            }
            else {
                // switch to next case
                directionChoice = 4;
                incrementMovement = 0;
                m_world *= SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(4.5f, 0.2f, -4.5f));

            }
            break;
        case 4:
            xVal = 4.5f;
            zVal = -4.5f;

            if (incrementMovement < range) {
                zVal += (incrementMovement / 70);

                if (roombaTrigger) {
                    incrementMovement++;
                }
                //rotation
                m_world *= SimpleMath::Matrix::CreateRotationY(-270 * (XM_PI / 180));
                m_world *= SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(xVal, 0.2f, zVal));

                

            }
            else if (rotationCounter < 360) {


                // rotation
                m_world *= SimpleMath::Matrix::CreateRotationY(-1 * rotationCounter * (XM_PI / 180));

                m_world *= SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(4.5f, 0.2f, 4.5f));

                rotationCounter++;
            }

            else {
                // switch to next case
                directionChoice = 1;
                incrementMovement = 0;
                rotationCounter = 1;
                m_world *= SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(4.5f, 0.2f, 4.5f));

            }
            break;
        default:
            break;
        }


    //to get roomba to move
    if (!roombaTrigger) {

    }
    m_BasicShaderPair.EnableShader(context);

	//render our model
    if (roombaTrigger) {
        m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_roombaT1.Get());   
    }
    else {
        m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_roombaT2.Get());
    }
    m_Roomba.Render(context);

    //preparing fan blades
    m_world = SimpleMath::Matrix::Identity;

    //rotation matrix
    SimpleMath::Matrix rotateFan = SimpleMath::Matrix::CreateRotationY(-90 * (XM_PI/180));
    
    //rotation matrix
    SimpleMath::Matrix spinFan = SimpleMath::Matrix::CreateRotationZ(incrementSpin * (XM_PI/180) * -1);
    incrementSpin++;

     //translation matrix
    //SimpleMath::Matrix translateFan = SimpleMath::Matrix::CreateTranslation(combineCalc.x, combineCalc.y+2, combineCalc.z-7);
    SimpleMath::Matrix translateFan = SimpleMath::Matrix::CreateTranslation(0.0f, 8.5f, -9.6f);
    
    
    incrementSideAngle += sideAngle;
    m_world *= rotateFan;
    //m_world *= testSpin;
    m_world *= spinFan;
    
    
    //m_Light.setPosition()
    m_world *= translateFan;
  

    //fan blades
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_fanBladesT.Get());
    m_fanBlades.Render(context);

    //-----------------------//
    // 
    //preparing fan centre
    m_world = SimpleMath::Matrix::Identity;

    //rotation matrix
    SimpleMath::Matrix rotateFanCentre = SimpleMath::Matrix::CreateRotationY(90 * (XM_PI / 180));
    
    //rotation matrix
    SimpleMath::Matrix spinCentre = SimpleMath::Matrix::CreateRotationZ(incrementSpin * (XM_PI / 180) * 1);
    incrementSpin++;

    //translation matrix
   //SimpleMath::Matrix translateFan = SimpleMath::Matrix::CreateTranslation(combineCalc.x, combineCalc.y+2, combineCalc.z-7);
    SimpleMath::Matrix translateCentre = SimpleMath::Matrix::CreateTranslation(0.0f, 8.5f, -11.2f);
	

    incrementSideAngle += sideAngle;
    m_world *= rotateFanCentre;
    //m_world *= testSpin;
    m_world *= spinCentre;


    //m_Light.setPosition()
    m_world *= translateCentre;


    //fan blades
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_fanCentreT.Get());
    m_fanCenter.Render(context);
    
    


    //prepare transform for sphere object. 
    m_world = SimpleMath::Matrix::Identity;
	SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(-12.7f, 0.0f, 0.0f);
	m_world = m_world * newPosition;

	//setup and draw sphere
	m_BasicShaderPair2.EnableShader(context);
	m_BasicShaderPair2.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_wall2T.Get(), m_wall2T.Get());
    m_windowWall.Render(context);

    

	//prepare transform for floor object. 
	m_world = SimpleMath::Matrix::Identity; //set world back to identity
	SimpleMath::Matrix newPosition3 = SimpleMath::Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
	m_world = m_world * newPosition3;

	//setup and draw cube
	m_BasicShaderPair2.EnableShader(context);
	m_BasicShaderPair2.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_flooringT.Get(), m_flooringT.Get());
	m_BasicModel3.Render(context);

    //prepare transform for walls object. 
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition4 = SimpleMath::Matrix::CreateTranslation(0.0f, 5.9f, -11.8f);
    m_world = m_world * newPosition4;

    //setup and draw cube
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_wall1T.Get());
    m_walls1.Render(context);

    //prepare transform for beanbag object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition5 = SimpleMath::Matrix::CreateTranslation(-7.3f, 0.0f, 6.0f);
    m_world = m_world * newPosition5;

    //setup and draw cube
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_beanbagT.Get());
    m_beanbag.Render(context);
   
    //prepare transform for chair object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition6 = SimpleMath::Matrix::CreateTranslation(-10.5f, 0.0f, -8.5f);
    m_world = m_world * newPosition6;

    //setup and draw cube
    m_BasicShaderPair2.EnableShader(context);
    m_BasicShaderPair2.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture1.Get(), m_flooringT.Get());
    m_chair.Render(context);

    //prepare transform for decoration piece object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition7 = SimpleMath::Matrix::CreateTranslation(0.0f, 0.0f, -10.5f);
    m_world = m_world * newPosition7;

    //setup and draw cube
    m_BasicShaderPair2.EnableShader(context);
    m_BasicShaderPair2.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_marble1T.Get(), m_marble2T.Get());
    m_decoPiece.Render(context);


    //prepare transform for table object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition8 = SimpleMath::Matrix::CreateTranslation(8.0f, 0.0f, 0.0f);
    m_world = m_world * newPosition8;


    //setup and draw cube
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture1.Get());
    m_table.Render(context);

    //prepare transform for decoration books object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition9 = SimpleMath::Matrix::CreateTranslation(0.0f, 0.0f, -10.5f);
    m_world = m_world * newPosition9;

    //setup and draw cube
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_booksT.Get());
    m_decoBooks.Render(context);


    //prepare transform for frame object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition10 = SimpleMath::Matrix::CreateTranslation(-12.0f, 6.0f, 1.5f);
    m_world = m_world * newPosition10;

    //setup and draw cube
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_art1.Get());
    m_frame1.Render(context);

    //prepare transform for frame object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition11 = SimpleMath::Matrix::CreateTranslation(-12.0f, 6.0f, 5.5f);
    m_world = m_world * newPosition11;

    //setup and draw cube
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_art2.Get());
    m_frame2.Render(context);


    //prepare transform for frame object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition12 = SimpleMath::Matrix::CreateTranslation(-12.0f, 6.0f, 8.5f);
    m_world = m_world * newPosition12;

    //setup and draw cube
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_art3.Get());
    m_frame3.Render(context);

    //prepare transform for sofa object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition13 = SimpleMath::Matrix::CreateTranslation(-10.0f, 0.0f, 1.0f);
    m_world = m_world * newPosition13;

    //setup and draw cube
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture3.Get());
    m_sofa.Render(context);

    //prepare transform for carpet object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition14 = SimpleMath::Matrix::CreateTranslation(0.0f, 0.1f, -2.5f);
    m_world = m_world * newPosition14;

    //setup and draw cube
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_carpetT.Get());
    m_carpet.Render(context);

    //prepare transform for vase object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition15 = SimpleMath::Matrix::CreateTranslation(8.5f, 1.55f, 1.2f);
    m_world = m_world * newPosition15;

    //setup and draw cube
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_carpetT.Get());
    m_vase.Render(context);


    //prepare transform for leaves object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition16 = SimpleMath::Matrix::CreateTranslation(8.5f, 2.1f, 1.2f);
    m_world = m_world * newPosition15;

    //setup and draw cube
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture2.Get());
    m_leaves.Render(context);


    //----------CUSTOM GEOMETRY--------//
    context->IASetInputLayout(m_batchInputLayout.Get());

    //Arrays of geomtry data.
    //Drawing a single triangle and a square We re-used a vertex here
    const int vCount = 4;
    const int iCount = 6;
    uint16_t iArray[iCount];
    VertexPositionColor vArray[vCount];
    iArray[0] = 0;
    iArray[1] = 1;
    iArray[2] = 2;
    iArray[3] = 3;
    iArray[4] = 2;
    iArray[5] = 1;
    vArray[0] = VertexPositionColor(Vector3(-1.0f, 0.5f, -2.0f), Colors::Yellow);
    vArray[1] = VertexPositionColor(Vector3(-1.0f, 0.0f, -2.0f), Colors::Yellow);
    vArray[2] = VertexPositionColor(Vector3(1.0f, 0.5f, -2.0f), Colors::Yellow);
    vArray[3] = VertexPositionColor(Vector3(1.0f, 0.0f, -2.0f), Colors::Green);

    m_batch->Begin();
    m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, iArray, iCount, vArray, vCount);
    m_batch->End();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::Bisque);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
    m_audEngine->Suspend();
}

void Game::OnResuming()
{
    secondLoopDelay = 2.f;
    m_timer.ResetElapsedTime();

    m_audEngine->Resume();
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);
    m_fxFactory = std::make_unique<EffectFactory>(device);
    m_sprites = std::make_unique<SpriteBatch>(context);
    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	//setup our test model
    m_windowWall.InitializeModel(device, "wall_window.obj");
    
    
    m_Roomba.InitializeModel(device,"doomba.obj");
    //fan
    m_fanBlades.InitializeModel(device, "fanBlades.obj");
    //beanbag
    m_beanbag.InitializeModel(device, "beanbag.obj");
    //chair
    m_chair.InitializeModel(device, "chair.obj");
    //decoration Piece
    m_decoPiece.InitializeModel(device, "decoPiece.obj");
    //table
    m_table.InitializeModel(device, "table.obj");
    //decoration books
    m_decoBooks.InitializeModel(device, "decoBooks.obj");
    //fan centre
    m_fanCenter.InitializeModel(device, "fanCenter.obj");
    //frame
    m_frame1.InitializeModel(device, "frame.obj");
    m_frame2.InitializeModel(device, "frame.obj");
    m_frame3.InitializeModel(device, "frame.obj");
    //sofa
    m_sofa.InitializeModel(device, "sofa.obj");
    m_carpet.InitializeModel(device, "carpet.obj");
    m_vase.InitializeModel(device, "vase.obj");
    m_leaves.InitializeModel(device, "leaves.obj");

    //wall that is drawn
    m_walls1.InitializeBox(device, 24.0f, 12.2f, 0.6f);

    //floor
	m_BasicModel3.InitializeBox(device, 24.1f, 0.1f, 24.1f);	//box includes dimensions

	//load and set up our Vertex and Pixel Shaders
	m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"light_ps.cso");

    //blended
	m_BasicShaderPair2.InitStandard(device, L"light_vs2.cso", L"light_ps2.cso");

	//load Textures
	CreateDDSTextureFromFile(device, L"floor1.dds",		    nullptr,	m_texture1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"moss.dds",           nullptr,	m_texture2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"sofa.dds",           nullptr,	m_texture3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"roomba_normal1.dds", nullptr,	m_roombaT1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"roomba_stop2.dds",   nullptr,	m_roombaT2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"beanbag.dds",        nullptr,	m_beanbagT.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"art1.dds",           nullptr,	m_art1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"art2.dds",           nullptr,	m_art2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"art3.dds",           nullptr,	m_art3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"carpet.dds",         nullptr,	m_carpetT.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"books.dds",          nullptr,	m_booksT.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"marble.dds",         nullptr,	m_marble1T.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"marble2.dds",        nullptr,	m_marble2T.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"flooring.dds",       nullptr,	m_flooringT.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"wall_Image.dds",     nullptr,	m_wall1T.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"wall2.dds",          nullptr,	m_wall2T.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"hex.dds",            nullptr,    m_fanCentreT.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"wallTex.dds",        nullptr,    m_fanBladesT.ReleaseAndGetAddressOf());
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        100.0f
    );
}


void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_font.reset();
	m_batch.reset();
	m_testmodel.reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
